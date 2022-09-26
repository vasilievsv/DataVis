#pragma once
#include "KvAction.h"
#include <memory>
#include <vector>

class KcImDataView;

class KcActionDataClean : public KvAction
{
public:
	KcActionDataClean(const std::string& filepath);

	bool trigger() override;

	void update() override;

private:
	bool loadData_(); // load raw data from filepath_ to rawData_

private:
	const std::string& filepath_;  // 数据文件路径
	std::unique_ptr<KcImDataView> dataView_;
	std::vector<std::vector<std::string>> rawData_; // 从filepath_读取的字符串矩阵
};
