﻿#pragma once
#include "prov/KvDataProvider.h"
#include <memory>

class KvData;


// 数据渲染的抽象类
class KvDataRender : public KvPropertiedObject
{
	Q_OBJECT

public:

	// 导入基类构造函数
	using KvPropertiedObject::KvPropertiedObject;

	// 提供发生错误时的文本描述
	virtual std::string errorText() const {
		return ""; // 空字符串表示无错误
	}


public slots:

	// 渲染数据
	bool render(std::shared_ptr<KvData> data) {
		// TODO: if (canShown() && !isVisible())
		//	return true; // skip render when widget hidden
		preRender_();
		return doRender_(data);
	}

	// 重置
	virtual void reset() = 0;

protected:
	virtual void preRender_() {}
	virtual bool doRender_(std::shared_ptr<KvData> data) = 0;
};

