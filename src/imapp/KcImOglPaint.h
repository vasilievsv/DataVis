#pragma once
#include "KcImPaint.h"
#include <vector>
#include <memory>
#include <functional>
#include <tuple>
#include <map>
#include <string_view>
#include "opengl/KcRenderObject.h"


// 基于ImGui环境下OpenGL的KvPaint实现

class KcImOglPaint : public KcImPaint
{
	using super_ = KcImPaint;
	using aabb_t = KtAABB<float_t, 3>;

public:

	KcImOglPaint(camera_type& cam);

	void setViewport(const rect_t& vp) override;

	void pushClipRect(const rect_t& cr) override;
	void popClipRect() override;

	void enableClipBox(point3 lower, point3 upper) override;
	void disableClipBox() override;

	void enablePolygonOffset(bool b) override { polygonOffset_ = b; }
	void enableDepthTest(bool b) override { depthTest_ = b; }
	bool depthTest() const override { return depthTest_; }

	void enableAntialiasing(bool b) override { antialiasing_ = b; }
	bool antialiasing() const override { return antialiasing_; }

	void enableFlatShading(bool b) override { flatShading_ = b; }
	bool flatShading() const override { return flatShading_; }

	void enableLighting(bool b) override { lighting_ = b; }
	bool lighting() const override { return lighting_; }

	void beginPaint() override;
	void endPaint() override;

	void* redraw(void* obj) override;

	void drawMarker(const point3& pt) override;

	void* drawMarkers(point_getter fn, unsigned count) override;

	void* drawMarkers(point_getter fn, color_getter clr, size_getter size, unsigned count) override;

	void drawLine(const point3& from, const point3& to) override;

	void* drawLineStrip(point_getter fn, unsigned count) override;

	void* drawLineStrips(const std::vector<point_getter>& fns, const std::vector<unsigned>& cnts) override;

	void drawRect(const point3& lower, const point3& upper) override;

	void drawText(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text) override;

	void drawText(const point3& anchor, const char* text, int align) override;

	void* drawTexts(const std::vector<point3>& anchors, const std::vector<std::string>& texts, int align, const point2f& spacing) override;

	void* drawGeom(vtx_decl_ptr decl, geom_ptr geom) override;

	void* fillBetween(point_getter line1, point_getter line2, unsigned count) override;

	void grab(int x, int y, int width, int height, void* data) override;

	point3 lightDirection() const override { return lightDir_; }
	void setLightDirection(const point3& dir) override { lightDir_ = dir; }

	color3f lightColor() const { return lightColor_; }
	void setLightColor(const color3f& clr) { lightColor_ = clr; }

	color3f ambientColor() const { return ambientColor_; }
	void setAmbientColor(const color3f& clr) { ambientColor_ = clr; }

	color3f specularColor() const { return specularColor_; }
	void setSpecularColor(const color3f& clr) { specularColor_ = clr; }

	float_t shininess() const { return shininess_; }
	void setShininess(float_t s) { shininess_ = s; }

	// 辅助函数
	KpMarker marker() const; // 装配marker绘制上下文

	// 内部函数，由ImGui回调，以绘制renderList_保存的渲染对象
	void drawRenderList_();

private:

	point3 toNdc_(const point3& pt) const;

	// 绘制点云
	void* drawPoints_(point_getter fn, unsigned count);

	void drawCircles_(point_getter fn, unsigned count);

	void drawQuadMarkers_(point_getter fn, unsigned count, const point2 quad[4]);

	void drawTriMarkers_(point_getter fn, unsigned count, const point2 tri[3]);

	struct KpRenderList_;
	KpRenderList_& currentRenderList();

	void pushRenderObject_(KpRenderList_& rl, KcRenderObject* obj);

	void pushRenderObject_(KcRenderObject* obj) {
		pushRenderObject_(currentRenderList(), obj);
	}

	KcRenderObject* lastRenderObject_();

	void configOglState_(); // 配置opengl属性

	void glViewport_(unsigned id);
	void glScissor_(unsigned id);
	void glClipPlane_(unsigned id);

	struct KpUvVbo;
	KcRenderObject* makeTextVbo_(std::vector<KpUvVbo>& vbo); // 生成绘制文本所需的vbo
	void pushTextVbo_(KpRenderList_& rl); // 生成绘制文本所需的vbo
	void pushColorVbo_(KpRenderList_& rl); // 生成绘制tris成员所需的vbo

	// 以下成员向KpRenderList_::tris写入数据

	void addLine_(const point3& pt0, const point3& pt1, const float4& clr); // 以tris的形式绘制直线

	// 填充凸多边形，fn返回凸多边形的顶点
	void addConvexPolyFilled_(point_getter fn, unsigned count, const float4& clr);

	void addLineLoop_(point_getter fn, unsigned count, const float4& clr);

	void addRect_(const point3& lower, const point3& upper);
	void addRectFilled_(const point3& lower, const point3& upper);
	void addCircle_(const point3& center, float_t radius);
	void addCircleFilled_(const point3& center, float_t radius);

	void addQuadFilled_(const point3& p0, const point3& p1, const point3& p2, const point3& p3, const float4& clr);

	// NB: fillVtx使用主色绘制，outlineVtx使用辅色绘制
	void addMarkers_(point_getter fn, unsigned count, const point2* fillVtx, unsigned numFill,
		const point2* outlineVtx, unsigned numOutline);

	void addMarkers_(point_getter fn, unsigned count, const point2* fillVtx, unsigned numFill);

	// 在当前渲染列表的tris成员分配count个KpColorVbo_对象
	struct KpColorVbo_;
	KpColorVbo_* newColorVbo_(unsigned count);

	// pos为屏幕坐标
	void pushTrisSoild_(const point2 pos[], unsigned c, const float4& clr);

	// pos为带深度的屏幕坐标
	void pushTrisSoild_(const float3 pos[], unsigned c, const float4& clr);

	// 存储render-object，以便下帧复用
	void saveObjList_();

	// 设置对象obj的渲染属性
	void syncObjProps_(KcRenderObject* obj);

	// @normToNdc: 若true，则将text坐标规范到ndc坐标系
	void drawText_(const point3& topLeft, const point3& hDir, const point3& vDir, const char* text, std::vector<KpUvVbo>& vbo, bool normToNdc);
	void drawText_(const point3& anchor, const char* text, int align, std::vector<KpUvVbo>& vbo, bool normToNdc);

	// pos存储顺序（屏幕坐标）: dx, dy, width, height
	// 其中(dx, dy)为第i个文字quad中心点相对于text-rect左上角的偏移
	// uv存储顺序（归一化坐标）：u1, v1, u2, v2
	void pushTextData_(const std::string_view& text, std::vector<point4f>& pos, std::vector<point4f>& uvs) const;

	// 自动生成法线vbo
	static void pushNormals_(KcRenderObject* obj);

	bool hasClipBox_() const;

private:

	// render states stacks

	std::vector<rect_t> viewportHistList_; // 视口列表
	std::vector<rect_t> clipRectHistList_; // 裁剪rect列表
	std::vector<aabb_t> clipBoxHistList_; // 三维空间裁剪box列表

	std::vector<unsigned> clipRectStack_;
	unsigned curViewport_{ unsigned(-1) }; // -1表示未设置
	unsigned curClipBox_{ unsigned(-1) };
	bool polygonOffset_{ false };

	bool depthTest_{ false }; // 启动深度测试？
	bool antialiasing_{ false };
	bool flatShading_{ false };
	bool lighting_{ false };

	// [0]: viewport idx
	// [1]: clipRect idx
	// [2]: clipBox idx
	// [3]: polygonOffset?
	using kRenderState_ = std::tuple<unsigned, unsigned, unsigned, bool>;

	struct KpColorVbo_
	{
		float3 pos;
		float4 clr;
	};

	struct KpUvVbo
	{
		float3 pos;
		float4 clr;
		point2f uv;
	};

	struct KpRenderList_
	{
		std::vector<std::shared_ptr<KcRenderObject>> objs; // 绘制plottables

		// NB: 为了减少渲染状态保存和切换，以下元素均使用ndc坐标，如此只需统一将投影矩阵设为单位阵即可
		std::vector<std::function<void(void)>> fns; // 绘制点线
		std::vector<KpUvVbo> texts; // 绘制文本

		// NB: 绘制scatter2d图时，希望各marker（包括fill和outline）按照绘制顺序堆叠
		// 而不能先批量fill，再批量outline（EdgedObject做法），这样的渲染结果会造成outline的相互干扰
		// 此外，诸如legend的图标、边框等基本元素，也需要提供一个绘制模式
		// 为解决以上问题，参照ImGui实现，在KpRenderList_中加入了tris成员，专门用三角形元组绘制各类要素
		// 包括line、rect、circle等，其中pos统一使用【屏幕坐标】，同时为了简化，不使用索引
		std::vector<KpColorVbo_> tris; // 绘制由三角形构成的各类元素，无索引
	};

	// 按渲染状态排序的待渲染列表
	std::map<kRenderState_, KpRenderList_> renderList_;

	std::map<void*, std::shared_ptr<KcRenderObject>> savedObjList_;

	point3 lightDir_{ -1., -1., -1. };
	color3f lightColor_{ 1.f, 1.f, 1.f };
	color3f ambientColor_{ 0.3f, 0.3f, 0.3f };
	color3f specularColor_{ 0.5f, 0.5f, 0.5f };
	float_t shininess_{ 32 };
};
