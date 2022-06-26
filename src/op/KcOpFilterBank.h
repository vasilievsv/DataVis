﻿#pragma once
#include "KvOpHelper1d.h"
#include <memory>

class KgFilterBank;

class KcOpFilterBank : public KvOpHelper1d
{
public:
	KcOpFilterBank(KvDataProvider* prov);

	kRange range(kIndex axis) const override;

	kReal step(kIndex axis) const override;

	kIndex size(kIndex axis) const override;

	kPropertySet propertySet() const override;

	unsigned ins() const final { return 1u; }

	unsigned outs() const final { return 1u; }

private:
	void setPropertyImpl_(int id, const QVariant& newVal) override;

	void processNaive_(const kReal* in, unsigned len, kReal* out) override;

	void preRender_() override;

private:
	std::unique_ptr<KgFilterBank> fbank_;

	kReal df_;
	kReal low_, high_;
};

