﻿#include "KgFbank.h"
#include <assert.h>
#include <functional>
#include "KtSampling.h"
#include "KuFreqUnit.h"
#include "KuMath.h"


KgFbank::KgFbank(KgFbank&& fbank) noexcept
    : opts_(fbank.opts_)
    , firstIdx_(std::move(fbank.firstIdx_))
    , fc_(std::move(fbank.fc_))
    , weights_(std::move(fbank.weights_))
    , range_(fbank.range_)
    , binStep_(fbank.binStep_)
{


}


KgFbank::KgFbank(const KpOptions& opts)
    : opts_(opts)
{
    if (opts_.highFreq <= 0)
        opts_.highFreq = opts.sampleRate / 2; // 取奈奎斯特频率

    // 边界检测
    assert(opts_.lowFreq >= 0 && opts_.highFreq > opts_.lowFreq);
    assert(opts_.type >= k_linear && opts_.type <= k_erb);

    initWeights_();
}


unsigned KgFbank::idim() const
{
    return opts_.fftBins;
}


unsigned KgFbank::odim() const
{
    return opts_.numBanks;
}


void KgFbank::process(const double* in, double* out) const
{
    for (unsigned i = 0; i < opts_.numBanks; i++) 
        // 若weights_[i].size() == 0, dot返回0.0
        out[i] = KuMath::dot(in + firstIdx_[i], weights_[i].data(),
            static_cast<unsigned>(weights_[i].size()));
}


double KgFbank::toHertz(KeType type, double scale)
{
    static std::function<double(double)> cvt[] = {
        [](double f) { return f; }, // k_linear
        [](double f) { return expm1(f); }, // k_log
        [](double f) { return KuFreqUnit::melToHertz(f); }, // k_mel
        [](double f) { return KuFreqUnit::barkToHertz(f); }, // k_bark
        [](double f) { return KuFreqUnit::camToHertz(f); }  // k_erb
    };

    return cvt[type](scale);
}


double KgFbank::fromHertz(KeType type, double hz)
{
    static std::function<double(double)> cvt[] = {
        [](double hz) { return hz; }, // k_linear
        [](double hz) { return log1p(hz); }, // k_log
        [](double hz) { return KuFreqUnit::hertzToMel(hz); }, // k_mel
        [](double hz) { return KuFreqUnit::hertzToBark(hz); }, // k_bark
        [](double hz) { return KuFreqUnit::hertzToCam(hz); }  // k_erb
    };

    return cvt[type](hz);
}


void KgFbank::setNormalize(bool b)
{
    opts_.normalize = b;
}


void KgFbank::initWeights_()
{
    range_ = { fromHertz(opts_.type, opts_.lowFreq), fromHertz(opts_.type, opts_.highFreq) };
    KtSampling<double> sampScale;
    sampScale.resetn(opts_.numBanks + 1, range_.first, range_.second, 0); // 在目标尺度上均匀划分各bin，相邻的bin有1/2重叠
    binStep_ = sampScale.dx();

    // Hz尺度的采样参数
    KtSampling<double> sanpHertz;
    sanpHertz.resetn(idim() - 1, opts_.lowFreq, opts_.highFreq, 0); // 兼容kaldi, x0ref取0, n取idim - 1

    firstIdx_.resize(opts_.numBanks);
    fc_.resize(opts_.numBanks);
    weights_.resize(opts_.numBanks);
    for (unsigned bin = 0; bin < opts_.numBanks; bin++) {

        // 计算当前bin在目标尺度上的规格（左边频率，右边频率，中心频率）
        auto fl = sampScale.indexToX(bin);
        auto fc = sampScale.indexToX(bin + 1);
        auto fr = sampScale.indexToX(bin + 2);

        // 计算当前bin的频点范围
        auto flhz = toHertz(opts_.type, fl);
        auto frhz = toHertz(opts_.type, fr);
        auto lowIdx = sanpHertz.xToHighIndex(flhz);
        auto highIdx = sanpHertz.xToLowIndex(frhz);
        lowIdx = std::max(lowIdx, long(0));
        highIdx = std::min(highIdx, long(idim() - 1));
        firstIdx_[bin] = lowIdx;
        fc_[bin] = toHertz(opts_.type, fc);

        // 计算当前bin的权值
        weights_[bin].clear();
        if (lowIdx <= highIdx) {
            // 计算当前bin的权值数组（目标尺度上的三角滤波）
            auto& wt = weights_[bin];
            wt.resize(highIdx - lowIdx + 1, 0);
            for (long i = lowIdx; i <= highIdx; i++)
                wt[i - lowIdx] = calcFilterWeight_(fl, fr, 
                    fromHertz(opts_.type, sanpHertz.indexToX(i)));

            if (opts_.normalize)
                KuMath::scale(wt.data(), 
                    static_cast<unsigned>(wt.size()), 1. / (frhz - flhz));
        }
    }
}


double KgFbank::calcFilterWeight_(double low, double high, double f)
{
    auto half = 0.5 * (high - low);
    return f <= low + half ? (f - low) / half : (high - f) / half;
}


const char* KgFbank::type2Str(KeType type)
{
    switch (type) {
    case k_linear:	return "linear";
    case k_log:		return "log";
    case k_mel:		return "mel";
    case k_bark:	return "bark";
    case k_erb:		return "erb";
    default:		return "unknown";
    }
}


KgFbank::KeType KgFbank::str2Type(const char* str)
{
    if (0 == _stricmp(str, type2Str(k_linear)))
        return k_linear;

    if (0 == _stricmp(str, type2Str(k_log)))
        return k_log;

    if (0 == _stricmp(str, type2Str(k_mel)))
        return k_mel;

    if (0 == _stricmp(str, type2Str(k_bark)))
        return k_bark;

    if (0 == _stricmp(str, type2Str(k_erb)))
        return k_erb;

    return k_mel;
}