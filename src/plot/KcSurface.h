#pragma once
#include "KvPlottable2d.h"


// 绘制grid(sampled2d)数据的quads曲面
class KcSurface : public KvPlottable2d
{
	using super_ = KvPlottable2d;

public:

	using super_::super_;

private:

	void* drawObject_(KvPaint*, unsigned) const override;

	void* drawSolid_(KvPaint*, unsigned) const;
	void* drawColor_(KvPaint*, unsigned) const;
};