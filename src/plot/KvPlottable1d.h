#pragma once
#include "KvPlottable.h"
#include "KvPaint.h" // for KvPaint::point_getter1
#include "KuDataUtil.h"
#include <map>


// 序列图的基类，主要处理串行数据，用于绘制graph、scatter、bars、area等类型图
// 兼容2d和3d模式，抽象grouped、stacked、ridged两类arrange模式

class KvPlottable1d : public KvPlottable
{
	using super_ = KvPlottable;

public:

	using super_::super_;


protected:

	using GETTER = std::function<std::vector<float_t>(unsigned ix)>;

	// 将GETTER变量转换为KvPaint需要的函数型
	typename KvPaint::point_getter1 toPoint3Getter_(GETTER g, unsigned channel) const;


	// 返回每个通道包含的1d数据数目. 
	// 对于samp2d数据，返回size(0); samp3d数据，返回size(0)*size(1)
	unsigned linesPerChannel_() const;

	// data1d数据总数
	unsigned linesTotal_() const;

	aabb_t calcBoundingBox_() const override;

	// 更新stacked数据
	bool output_() override;

	//////////////////////////////////////////////////////////////////////

	/// 维度映射（相对输出数据而言）

public:

	unsigned xdim() const { return axisDim_[0]; } // 返回x轴对应的数据维度
	unsigned ydim() const { return axisDim_[1]; } // 返回y轴对应的数据维度
	unsigned zdim() const { return axisDim_[2]; } // 返回z轴对应的数据维度

	void setXdim(unsigned dim); // 将维度dim映射到x轴
	void setYdim(unsigned dim); // 将维度dim映射到x轴
	void setZdim(unsigned dim); // 将维度dim映射到x轴

protected:

	point3 toPoint_(const float_t* valp, unsigned ch) const {
		return { valp[xdim()], valp[ydim()], usingDefaultZ_() ? defaultZ(ch) : valp[zdim()] };
	}

private:

	unsigned axisDim_[3]{ 0, 1, 2 }; // x/y/z轴对应的数据维度

	//////////////////////////////////////////////////////////////////////


	////////////////////////////////////////////////////////////////////////
	// 
	// arrange模式

public:

	enum KeArrangeMode
	{
		k_arrange_none, 
		k_arrange_overlay = k_arrange_none,
		k_arrange_group, // x轴偏移
		k_arrange_ridge, // y轴偏移
		k_arrange_stack  // y轴堆叠
	};

	// dim == odata()->dim()时，返回channel的arrange模式
	int arrangeMode(unsigned dim) const {
		return arrangeMode_[dim];
	}

	void setArrangeMode(unsigned dim, int mode);

	float_t ridgeOffset(unsigned dim) const { return ridgeOffset_[dim]; }
	void setRidgeOffset(unsigned dim, float_t offset);

	float_t groupOffset(unsigned dim) const { return groupOffset_[dim]; }
	void setGroupOffset(unsigned dim, float_t offset);

	float_t groupSpacing(unsigned dim) const { return groupSpacing_[dim]; }
	void setGroupSpacing(unsigned dim, float_t spacing);

protected:

	// 返回第ch通道的第idx条1d数据访问接口
	KuDataUtil::KpPointGetter1d lineAt_(unsigned ch, unsigned idx) const {
		return lineArranged_(ch, idx, 0);
	}

	// 按照从高维到低维的顺序（通道为最高维），依次处理arrange模式到dim维度（含dim）
	KuDataUtil::KpPointGetter1d lineArranged_(unsigned ch, unsigned idx, unsigned dim) const;

	// 由于可能有多个维度为ridge模式，所以此处传递参数dim用于区分
	GETTER lineGrouped_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;
	GETTER lineRidged_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;
	GETTER lineStacked_(const KuDataUtil::KpPointGetter1d& g, unsigned ch, unsigned idx, unsigned dim) const;

	float_t ridgeOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const;

	float_t groupOffsetAt_(unsigned ch, unsigned idx, unsigned dim) const;

	float_t ridgeOffsetAt_(unsigned ch, unsigned idx) const;

private:

	void calcStackData_(unsigned dim) const; // 计算stack数据，内部调用

private:

	std::vector<int> arrangeMode_; // 各维度的arrange模式，大小等于odata()->dim()
	                               // 最后一个值代表channel的arrange模式

	std::vector<float_t> ridgeOffset_;

	std::vector<float_t> groupOffset_, groupSpacing_;

	mutable std::map<unsigned, std::shared_ptr<KvDiscreted>> stackedData_; // 保存stack数据计算结果, dim -> data
};
