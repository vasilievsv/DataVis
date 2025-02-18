﻿#pragma once
#include "KvDataOperator.h"
#include <memory>


class KgHist;

// TODO: 支持多通道数据

class KcOpHist : public KvDataOperator
{
	using super_ = KvDataOperator;

public:
	KcOpHist();

	int spec(kIndex outPort) const final;

	kRange range(kIndex outPort, kIndex axis) const final;

	kReal step(kIndex outPort, kIndex axis) const final;

	kIndex size(kIndex outPort, kIndex axis) const final;

	bool onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins) final;

	void onStopPipeline() final;

	void showPropertySet() final;

	bool permitInput(int dataSpec, unsigned inPort) const final;

	// 根据输入同步low_ & high_
	bool onNewLink(KcPortNode* from, KcPortNode* to) final;

private:

	bool prepareOutput_() final;

	void outputImpl_() final;

private:
	std::unique_ptr<KgHist> hist_;
	double min_, max_; // 做hist的x轴范围
	int bins_; // hist的bin数
};

