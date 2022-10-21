#include "KcBars2d.h"
#include "KvPaint.h"
#include "KvDiscreted.h"


void KcBars2d::drawImpl_(KvPaint* paint, point_getter getter, const color4f& majorColor) const
{
	auto barWidth = barWidth_();
	bool drawFill = fill_.style != KpBrush::k_none && majorColor.a() != 0;
	bool drawBorder = border_.style != KpPen::k_none && minorColor().a() != 0 && minorColor() != majorColor;

	for (unsigned i = 0; i < data()->size(); i++) {
		auto pt0 = getter(i);
		pt0.x() -= barWidth * 0.5;
		point3 pt1(pt0.x() + barWidth, 0, pt0.z());

		if (drawFill) {
			auto cxt = fill_;
			cxt.color = majorColor;
			paint->apply(cxt);
			paint->fillRect(pt0, pt1);
		}

		if (drawBorder) {
			auto cxt = border_;
			cxt.color = minorColor();
			paint->apply(cxt);
			paint->drawRect(pt0, pt1);
		}
	}
}

float KcBars2d::barWidth_() const
{
	auto disc = std::dynamic_pointer_cast<KvDiscreted>(data());
	return disc->step(0) != 0 ? 
		disc->step(0) * barWidthRatio_ : 
		boundingBox().width() / disc->size() * barWidthRatio_;
}