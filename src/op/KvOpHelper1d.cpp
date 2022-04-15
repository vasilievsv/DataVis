﻿#include "KvOpHelper1d.h"
#include "KcSampled1d.h"
#include "KcSampled2d.h"
#include <assert.h>


std::shared_ptr<KvData> KvOpHelper1d::processImpl_(std::shared_ptr<KvData> data)
{
    if (data->dim() == 1) {
        std::shared_ptr<KcSampled1d> res = std::make_shared<KcSampled1d>();
        res->resize(length(0), data->channels());
        res->reset(0, range(0).low(), step(0), 0.5); // TODO: nonuniform
        auto data1d = std::dynamic_pointer_cast<KcSampled1d>(data);

        if (data->channels() == 1) {
            processNaive_(data1d->data(), data1d->count(), (kReal*)res->data());
        }
        else {
            std::vector<kReal> rawData(data1d->length(0));
            std::vector<kReal> out(length(0));
            for (kIndex c = 0; c < data1d->channels(); c++) {
                for (kIndex i = 0; i < data1d->length(0); i++) 
                    rawData[i] = data1d->value(i, c);

                processNaive_(rawData.data(), rawData.size(), out.data());
                res->setChannel(nullptr, c, out.data());
            }
        }
        
        return res;
    }

    assert(data->dim() == 2);
    auto data2d = std::dynamic_pointer_cast<KcSampled2d>(data);
    assert(data2d);

    auto res = std::make_shared<KcSampled2d>();

    res->resize(data2d->length(0), length(1), data2d->channels());
    res->reset(0, range(0).low(), step(0), 0.5);
    res->reset(1, range(1).low(), step(1), 0.5); // TODO: nonuniform

    if (data->channels() == 1) {
        for (kIndex i = 0; i < data2d->length(0); i++) 
            processNaive_(data2d->row(i), data2d->length(1), res->row(i));
    }
    else {
        std::vector<kReal> rawData(data2d->length(1));
        std::vector<kReal> out(length(1));
        for (kIndex c = 0; c < data2d->channels(); c++) {
            for (kIndex i = 0; i < data2d->length(0); i++) {
                for (kIndex j = 0; j < data2d->length(1); j++)
                    rawData[j] = data2d->value(i, j, c);

                processNaive_(rawData.data(), rawData.size(), out.data());
                res->setChannel(&i, c, out.data());
            }
        }
    }

    return res;
}
