#include "KcActionTextLoadAndClean.h"
#include "KcImTextCleaner.h"
#include <regex>
#include <fstream>
#include <assert.h>
#include "KuStrUtil.h"
#include "KuFileUtil.h"


KcActionTextLoadAndClean::KcActionTextLoadAndClean(const std::string& filepath)
    : KvAction("DataClean")
    , filepath_(filepath)
{

}


bool KcActionTextLoadAndClean::trigger()
{
    // 加载数据文件
    if (!loadData_()) {
        state_ = KeState::k_failed;
        return false;
    }

    if (rawData_.empty()) { // 空文件
        return false;
    }
    
    // 创建数据窗口
    cleanData_.clear();
    cleanWindow_ = std::make_unique<KcImTextCleaner>(filepath_, rawData_, cleanData_);
    if (cleanWindow_ == nullptr) {
        state_ = KeState::k_failed;
        return false;
    }

    state_ = KeState::k_triggered;
    return true;
}


void KcActionTextLoadAndClean::update()
{
    assert(cleanWindow_ != nullptr);

    if (cleanWindow_->visible())
        cleanWindow_->update();
    else {
        state_ = cleanData_.empty() ? KeState::k_cancelled : KeState::k_done;  
        cleanWindow_ = nullptr;
    }
}


bool KcActionTextLoadAndClean::loadData_()
{
    const std::string rexpDelim = "\\s+";

    rawData_.clear();
    rawData_.reserve(25000); // 预留一定数量的空间

    text_ = KuFileUtil::readAsString(filepath_); // 暂存所有文本，以便后面使用string_view类型
    auto lines = KuStrUtil::split(text_, "\n", false); // 空行有特殊语义，保留

    for (auto& line : lines) {

        auto tl = KuStrUtil::trim(line);
        if (tl.empty()) {
            rawData_.push_back({}); // 插入空行
        }
        else if (tl[0] != '#' && tl[0] != '!') {
            assert(tl[0] != '\0');

            auto tokens = KuStrUtil::split(tl, ", \t", true);
            rawData_.emplace_back(std::move(tokens));
        }
        else {
            ; // 跳过注释行
        }
    }

    return true;
}