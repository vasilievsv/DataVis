#include "KcHeatMap.h"
#include "KvPaint.h"
#include "KtGeometryImpl.h"
#include "KuPrimitiveFactory.h"
#include "KvDiscreted.h"
#include <sstream>


KcHeatMap::KcHeatMap(const std::string_view& name)
	: super_(name)
{
	setForceDefaultZ(true); // 在3d空间显示热图平面
	setFlatShading(true);
	setColoringMode(k_colorbar_gradiant);
}


void KcHeatMap::setData(const_data_ptr d)
{
	super_::setData(d);
	setLabelingDim(odim());
}


unsigned KcHeatMap::objectCount() const
{
	return empty() ? 0 : channels_() + 1/*label*/;
}


bool KcHeatMap::objectVisible_(unsigned objIdx) const
{
	if (objIdx == channels_()) 
		return showLabel();
	else
		return super_::objectVisible_(objIdx);
}



bool KcHeatMap::objectReusable_(unsigned objIdx) const
{
	if (objIdx == channels_()) {
		return !dataChanged() && !labelChanged();
	}
	else {
		return super_::objectReusable_(objIdx);
	}
}


void KcHeatMap::setObjectState_(KvPaint* paint, unsigned objIdx) const
{
	if (objIdx == channels_()) {
		paint->apply(label().font);
		paint->setColor(label().color);
	}
	else
		super_::setObjectState_(paint, objIdx);
}


std::pair<KcHeatMap::float_t, KcHeatMap::float_t> KcHeatMap::xyshift_() const
{
	auto disc = discreted_();
	auto dx = (xdim() == odim()) ? 1 : std::abs(disc->step(xdim()));
	auto dy = (ydim() == odim()) ? 1 : std::abs(disc->step(ydim()));
	assert(KuMath::isDefined(dx) && dx != 0);
	assert(KuMath::isDefined(dy) && dy != 0);

	return { dx / 2, dy / 2 };
}


KcHeatMap::aabb_t KcHeatMap::calcBoundingBox_() const
{
	// TODO：暂不支持维度映射
	//const_cast<KcHeatMap*>(this)->setXdim(odim() - 2);
	//const_cast<KcHeatMap*>(this)->setYdim(odim() - 1);
	//const_cast<KcHeatMap*>(this)->setZdim(odim());

	auto aabb = super_::calcBoundingBox_();

	if (!empty() && odata()->dim() > 1) {
		auto xyshift = xyshift_();
		aabb.inflate(xyshift.first, xyshift.second);
	}

	return aabb;
}


void* KcHeatMap::drawObject_(KvPaint* paint, unsigned objIdx) const
{
	// TODO：暂不支持维度映射
	//const_cast<KcHeatMap*>(this)->setXdim(odim() - 2);
	//const_cast<KcHeatMap*>(this)->setYdim(odim() - 1);
	//const_cast<KcHeatMap*>(this)->setZdim(odim());

	if (objIdx == channels_())
		return drawLabel_(paint);
	else 
		return drawGrid_(paint, objIdx);
}


void* KcHeatMap::drawGrid_(KvPaint* paint, unsigned ch) const
{
	auto xyshift = xyshift_();

	struct KpVtxBuffer_
	{
		point3f pos;
		point4f clr;
	};

	// 热图数据点居中，所以grid多一行多一列
	auto nx = linesPerGrid_() + 1; // 列数
	auto ny = sizePerLine_() + 1; // 行数
	auto grids = gridsPerChannel_();

	auto geom = std::make_shared<KtGeometryImpl<KpVtxBuffer_>>(k_quads);
	auto idxPerGrid = KuPrimitiveFactory::indexGrid<std::uint32_t>(nx, ny, nullptr);
	geom->reserve(nx * ny * gridsTotal_(), idxPerGrid * gridsTotal_());
	std::uint32_t idxBase(0);

	auto shape = KuDataUtil::shape(*discreted_());

	for (unsigned i = 0; i < grids; i++) {
		for (unsigned j = 0; j < nx; j++) {
			auto line = gridLineAt_(ch, i, j > 0 ? j - 1 : 0);
			assert(line.size == ny - 1);

			auto vtx = geom->newVertex(ny);

			for (unsigned k = 0; k < ny; k++) {
				auto pt = line.getter(k > 0 ? k - 1 : 0);

				if (j > 0)
					pt[xdim()] += xyshift.first;
				else 
					pt[xdim()] -= xyshift.first; // 首列数据为多出的一列

				if (k > 0)
					pt[ydim()] += xyshift.second;
				else 
					pt[ydim()] -= xyshift.second; // 首行数据为多出的一行

				vtx->pos = toPoint_(pt.data(), ch);
				vtx->clr = mapValueToColor_(pt.data(), ch);
				++vtx;
			}
		}

		auto idx = geom->newIndex(idxPerGrid);

		// 由于grid整体向右上方偏移，所以应保证quad的最后一个顶点是右上角（opengl默认falt渲染模式使用最后一个顶点的数据）
		// 因此设定startVtx为3，即起点为右下角，如此顺时针旋转，可保证右上角为终点
		KuPrimitiveFactory::indexGrid<>(ny, nx, idx, 3, idxBase); // 由于顶点按ny的顺序依次写入的，所以此处ny在前
		idxBase += nx * ny;
	}

	return paint->drawGeomColor(geom);
}


void* KcHeatMap::drawLabel_(KvPaint* paint) const
{
	auto xyshift = xyshift_();

	auto leng = paint->projectv({ xyshift.first * 2, xyshift.second * 2, 0 }).abs();
	auto minSize = paint->textSize("0");
	if (leng.x() < minSize.x() || leng.y() < minSize.y()) // 加1个总体判断，否则当nx*ny很大时，非常耗时
		return nullptr;

	auto count = discreted_()->count();
	std::vector<point3> anchors; anchors.reserve(count);
	std::vector<std::string> texts; texts.reserve(count);
	
	std::ostringstream strm;
	formatter().apply(strm);

	auto nx = sizePerLine_();
	auto ny = linesPerGrid_();
	auto grids = gridsPerChannel_();
	for (unsigned ch = 0; ch < channels_(); ch++) {
		for (unsigned i = 0; i < grids; i++) {
			for (unsigned j = 0; j < ny; j++) {
				auto line = gridLineAt_(ch, i, j);
				assert(line.size == nx);
				for (unsigned k = 0; k < nx; k++) {
					auto pt = line.getter(k);
					anchors.push_back(toPoint_(pt.data(), ch));
					strm.str("");
					strm << pt[labelingDim()];
					texts.push_back(strm.str());
				}
			}
		}
	}

	assert(anchors.size() == count);

	const_cast<KcHeatMap*>(this)->labelChanged() = false;
	return paint->drawTexts(anchors, texts, label().align, label().spacing);
}
