#pragma once
#include "KvPlottable.h"
#include "KvPaint.h"

class KvSampled;

// image图的基类，主要处理grid（sampled2d）数据，用于绘制热图heatmap、曲面图surface等
// 兼容2d和3d模式

class KvPlottable2d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;

	const color4f& minorColor() const override;

	void setMinorColor_(const color4f& minor) override;

	unsigned objectCount() const override;

	bool showFill() const { return filled_; }
	bool& showFill() { return filled_; }

	bool showBorder() const { return edged_; }
	bool& showBorder() { return edged_; }

	const KpPen& borderPen() const { return borderPen_; }
	KpPen& borderPen() { return borderPen_; }

protected:

	bool objectVisible_(unsigned objIdx) const override;

	void setObjectState_(KvPaint*, unsigned objIdx) const override;

	void* drawObject_(KvPaint*, unsigned objIdx) const override;

	using GETTER = std::function<std::vector<float_t>(unsigned ix, unsigned iy)>;

	virtual void* drawImpl_(KvPaint*, GETTER, unsigned nx, unsigned ny, unsigned ch) const;

private:
	bool filled_{ true };
	bool edged_{ false };
	KpPen borderPen_;
};
