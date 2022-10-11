#pragma once
#include "plot/KvPlottable.h"


class KcImPlottable3d : public KvPlottable
{
public:

	// 支持的6种一维图类型. TODO: 混合类型的支持
	enum KeType
	{
		k_scatter,  // 散点图
		k_line,   // 连线图
		k_line_scatter, // 点线图
		k_line_fill, // 填充图
		k_bars,   // 层叠柱状图
		k_bars_grouped, // 分组柱状图
		k_type_count
	};

	using KvPlottable::KvPlottable;

	void draw(KvPaint3d*) const override;

	int type() const { return type_; }
	int& type() { return type_; }

private:
	int type_{ k_line };
};


