﻿#include "KcPvAudioInput.h"
#include "audio/KcAudio.h"
#include "audio/KcAudioDevice.h"
#include "KtSampling.h"
#include "imgui.h"
#include "readerwriterqueue/readerwriterqueue.h"
#include "imapp/KsImApp.h"
#include "imapp/KgPipeline.h"


namespace kPrivate
{
	using data_queue = moodycamel::ReaderWriterQueue<std::shared_ptr<KcSampled1d>>;

    class KcAudioStreamObserver : public KcAudioDevice::observer_type
    {
    public:

        KcAudioStreamObserver(KcPvAudioInput* ais) {
            stream_ = ais;
        }

        bool update(void*/*outputBuffer*/, void* inputBuffer, unsigned frames, double streamTime) override {
            auto data = std::make_shared<KcSampled1d>();
			data->resizeChannel(stream_->channels(0));
			data->reset(0, streamTime, stream_->step(0, 0));
            data->pushBack(static_cast<kReal*>(inputBuffer), frames);
			stream_->enqueue(data);
            return true;
        }

    private:
        KcPvAudioInput* stream_; 
    };


	enum KeAudioInputPropertyId
	{
		k_device_id,
		k_channels,
		k_sample_rate,
		k_frame_time,
	};
}


KcPvAudioInput::KcPvAudioInput() : KvDataProvider("AudioInput")
{
    dptr_ = new KcAudioDevice;
    deviceId_ = ((KcAudioDevice*)dptr_)->defaultInput();
    sampleRate_ = ((KcAudioDevice*)dptr_)->preferredSampleRate(deviceId_);
    frameTime_ = 0.1;
	queue_ = new kPrivate::data_queue;

	spec_.stream = true;
	spec_.dynamic = true;
	spec_.type = k_sampled;
	spec_.dim = 1;
	spec_.channels = 1;
}


KcPvAudioInput::~KcPvAudioInput()
{
    delete (KcAudioDevice*)dptr_;
	delete (kPrivate::data_queue*)queue_;
}


int KcPvAudioInput::spec(kIndex outPort) const
{
	return spec_.spec;
}


bool KcPvAudioInput::onStartPipeline(const std::vector<std::pair<unsigned, KcPortNode*>>& ins)
{
	assert(ins.empty());

	auto device = (KcAudioDevice*)dptr_;
	if (device->opened())
		device->close(); // TODO: 若参数一致，则不关闭

	device->remove<kPrivate::KcAudioStreamObserver>(); // 根据当前参数重新添加观察者

	KcAudioDevice::KpStreamParameters iParam;
	iParam.deviceId = deviceId_;
	iParam.channels = spec_.channels;

	KtSampling<kReal> samp(kReal(0), kReal(frameTime_), kReal(1) / sampleRate_, 0);
	unsigned bufferFrames = samp.size(); // unsigned(sampleRate_ * frameTime_ + 0.5);

	if (!device->open(nullptr, &iParam,
		std::is_same<kReal, double>::value ? KcAudioDevice::k_float64 : KcAudioDevice::k_float32,
		sampleRate_, bufferFrames))
		return false;

	device->pushBack(std::make_shared<kPrivate::KcAudioStreamObserver>(this));
	data_ = std::make_shared<KcSampled1d>(samp.dx(), iParam.channels);
	dataStamp_ = 0;
	return device->start();
}


void KcPvAudioInput::onStopPipeline()
{
	((KcAudioDevice*)dptr_)->stop(true);
	while (((kPrivate::data_queue*)queue_)->pop()); // clear the queued audio input data
}


void KcPvAudioInput::output()
{
	assert(data_);

	auto q = (kPrivate::data_queue*)queue_;

	if (q->size_approx() > 0) { // 组装数据
		auto data = std::make_shared<KcSampled1d>(data_->step(0), data_->channels());

		std::shared_ptr<KcSampled1d> d;
		while (q->try_dequeue(d))
			data->pushBack(*d);

		std::swap(data_, data);
		dataStamp_ = currentFrameIndex_();
	}
}


std::shared_ptr<KvData> KcPvAudioInput::fetchData(kIndex outPort) const
{
	assert(outPort == 0);
	return dataStamp_ == currentFrameIndex_() ? data_ : nullptr;
}


unsigned KcPvAudioInput::dataStamp(kIndex outPort) const
{
	return dataStamp_;
}


#include <Windows.h>
#include <Stringapiset.h>
namespace kPrivate
{
	std::string localToUtf8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
		ZeroMemory(pwBuf, nwLen * 2 + 2);

		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);

		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string retStr(pBuf);

		delete[]pwBuf;
		delete[]pBuf;

		pwBuf = NULL;
		pBuf = NULL;

		return retStr;
	}
}

void KcPvAudioInput::showPropertySet()
{
	super_::showPropertySet();
	ImGui::Separator();

	auto device = (KcAudioDevice*)dptr_;
	auto info = device->info(deviceId_);

	ImGui::BeginDisabled(working_());

	if (ImGui::BeginCombo("Device", kPrivate::localToUtf8(info.name).c_str())) {
		for (unsigned i = 0; i < device->count(); i++) {
			info = device->info(i);
			auto name = kPrivate::localToUtf8(info.name);
			if (info.inputChannels > 0 && ImGui::Selectable(name.c_str(), i == deviceId_)) {
				deviceId_ = i;
				notifyChanged();
			}
		}

		ImGui::EndCombo();
	}

	info = device->info(deviceId_);
	assert(spec_.channels <= info.inputChannels);
	auto channles = spec_.channels;
	if (ImGui::SliderInt("Channles", &channles, 1, info.inputChannels)) {
		spec_.channels = KuMath::clamp<unsigned>(channles, 1, info.inputChannels);
		notifyChanged();
	}

	auto rateStr = std::to_string(sampleRate_);
	if (ImGui::BeginCombo("SampleRate", rateStr.c_str())) {
		for (auto rate : info.sampleRates)
			if (ImGui::Selectable(std::to_string(rate).c_str(), sampleRate_ == rate)) {
				sampleRate_ = rate;
				notifyChanged();
			}

		ImGui::EndCombo();
	}

	if (ImGui::DragFloat("FrameTime", &frameTime_, 0.001, 0.01, 1.0)) {
		frameTime_ = KuMath::clampFloor(frameTime_, 1e-4f);
		notifyChanged();
	}

	ImGui::EndDisabled();
}


kRange KcPvAudioInput::range(kIndex outPort, kIndex axis) const
{
	return axis == 0 ? kRange{ 0, frameTime_ } : kRange{ -1, 1 };
}


kReal KcPvAudioInput::step(kIndex outPort, kIndex axis) const
{
	return axis == 0 ? static_cast<kReal>(1) / sampleRate() : 
		KvDiscreted::k_nonuniform_step;
}


kIndex KcPvAudioInput::size(kIndex outPort, kIndex) const
{
	KtSampling<kReal> samp(kReal(0), kReal(frameTime_), kReal(1) / sampleRate_, 0);
	return samp.size(); // TODO: 使用open时的bufferFrames参数
}


void KcPvAudioInput::enqueue(const std::shared_ptr<KcSampled1d>& data)
{
	((kPrivate::data_queue*)queue_)->enqueue(data);
}
