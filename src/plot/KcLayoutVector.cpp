#include "KcLayoutVector.h"


void KcLayoutVector::arrange(const rect_t& rc)
{
	if (rowMajor_) {
        if (rc.width()) arrangeStack_(rc, 0);
		if (rc.height()) arrangeOverlay_(rc, 1);
	}
	else {
        if (rc.width()) arrangeOverlay_(rc, 0);
        if (rc.height()) arrangeStack_(rc, 1);
	}
}


KcLayoutVector::size_t KcLayoutVector::calcSize_() const
{
	for (auto& i : elements_)
		if (i) i->calcSize();

	return  rowMajor_ ? size_t(calcSizeStacked_(0), calcSizeOverlayed_(1))
		              : size_t(calcSizeOverlayed_(0), calcSizeStacked_(1));
}


KcLayoutVector::float_t KcLayoutVector::calcSizeStacked_(int dim) const
{
	float_t sz(0);
	int squeezedItems(0);

	for (auto& i : elements_) {
		if (i) {
			sz += i->expectRoom()[dim];
			squeezedItems += i->squeezeNeeded()[dim];
		}
	}
	
	squeezeNeeded_[dim] = squeezedItems;

	return sz;
}


KcLayoutVector::float_t KcLayoutVector::calcSizeOverlayed_(int dim) const
{
	float_t sz(0);
	int squeezedItems(0);

	for (auto& i : elements_) {
		if (i) {
			sz = std::max(sz, i->expectRoom()[dim]);
			squeezedItems = std::max(squeezedItems, i->squeezeNeeded()[dim]);
		}
	}

	squeezeNeeded_[dim] = squeezedItems;

	return sz;
}


void KcLayoutVector::arrangeOverlay_(const rect_t& rc, int dim)
{
	__super::arrange_(rc, dim);
	auto rcLay = rc;
	rcLay.upper()[~dim] = rcLay.lower()[~dim]; // 屏蔽另一个维度
	for (auto& i : elements_)
		if (i) i->arrange(rcLay);
}


void KcLayoutVector::arrangeStack_(const rect_t& rc, int dim)
{
	auto totalSpace = rc.upper()[dim] - rc.lower()[dim];
	auto fixedSpace = expectRoom()[dim];
	auto extraSpace = std::max(0., totalSpace - fixedSpace);

	// TODO: 1. 暂时使用均匀分配策略; 2. 未考虑squeezeNeeded == 0时，仍有extraSpace的情况
	auto squeezedItemSpace = squeezeNeeded()[dim] ? extraSpace / squeezeNeeded()[dim] : 0;

	__super::arrange_(rc, dim); // 不能在expectRoom之前调用，否则会破坏expectRoom依赖的iRect

	rect_t rcItem = iRect_;
	rcItem.upper()[~dim] = rcItem.lower()[~dim]; // 屏蔽另一个维度
	auto unusedSpace = iRect_.upper()[dim] - iRect_.lower()[dim]; // 可用的空间
	for (auto& i : elements_) {
		if (i == nullptr)
			continue;

		// 支持fixd-item和squeezed-item的混合体
		auto itemSpace = i->expectRoom()[dim] + i->squeezeNeeded()[dim] * squeezedItemSpace;

		rcItem.upper()[dim] = rcItem.lower()[dim] + itemSpace;
		i->arrange(rcItem);

		rcItem.lower() = rcItem.upper();
	}
}
