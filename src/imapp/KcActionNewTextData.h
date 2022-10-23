#pragma once
#include "KcActionSequence.h"


// LoadText动作: 由3个子动作构成
// 一是KcActionShowFileDialog，弹出对话框选取文件
// 二是KcActionDataClean，数据清洗与类型转换
// 三是KcActionNewNode，向NodeEditor插入provider节点

class KcActionNewTextData : public KcActionSequence
{
public:
	KcActionNewTextData();
};
