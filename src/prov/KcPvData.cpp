﻿#include "KcPvData.h"
#include "KvDiscreted.h"


KcPvData::KcPvData(const std::string_view& name, std::shared_ptr<KvData> data)
	: KvDataProvider(name), data_(data) 
{

}


kIndex KcPvData::dim() const
{
	return data_->dim();
}


kIndex KcPvData::channels() const
{
	return data_->channels();
}


kRange KcPvData::range(kIndex axis) const
{
	return data_->range(axis);
}


kReal KcPvData::step(kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->step(axis) : 0;
}


kIndex KcPvData::size(kIndex axis) const
{
	auto dis = std::dynamic_pointer_cast<KvDiscreted>(data_);
	return dis ? dis->size(axis) : KvData::k_inf_size;
}


std::shared_ptr<KvData> KcPvData::fetchData(kIndex outPort)
{
	assert(outPort == 0);
	return data_;
}


namespace kPrivate
{
	enum KeDataProperty
	{
		k_range
	};
}


/*
bool KcPvData::getOption(KeObjectOption opt) const
{
	assert(opt == k_show);

	return kAppEventHub->isDocked(this);
}


void KcPvData::setOption(KeObjectOption opt, bool on)
{
	assert(opt == k_show);

	if (on) {
		auto dv = new QtDataView;
		auto disc = std::dynamic_pointer_cast<KvDiscreted>(data_);
		dv->fill(*disc);
		kAppEventHub->showDock(this, dv);
	}
	else
		kAppEventHub->closeDock(this);
}
*/