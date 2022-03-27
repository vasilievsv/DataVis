﻿#pragma once
#include "KvDataOperator.h"


class KgRdft;

class KcFftOp : public KvDataOperator
{
public:

	KcFftOp(KvDataProvider* prov);

	kPropertySet propertySet() const override;

	// key轴为频率轴，非时间轴，故非stream
	bool isStream() const override { return false; }

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }


private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;
	void syncParent() override;

	std::shared_ptr<KvData> processImpl_(std::shared_ptr<KvData> data) override;

	std::shared_ptr<KvData> process1d_(std::shared_ptr<KvData> data);
	std::shared_ptr<KvData> process2d_(std::shared_ptr<KvData> data);

private:
	std::unique_ptr<KgRdft> rdft_;
	kReal df_;
	kReal nyquistFreq_;
};

