#include "KcImActionPanel.h"
#include "imgui.h"
#include "KvAction.h"


KcImActionPanel::KcImActionPanel(const std::string_view& name)
	: KvImWindow(name)
{
	minSize_[0] = 120, minSize_[1] = 240;
}


void KcImActionPanel::addAction(const std::string_view& groupName, action_ptr act)
{
	groupMaps_[std::string(groupName)].push_back(act);
}


int KcImActionPanel::flags() const
{
	return ImGuiWindowFlags_NoBringToFrontOnFocus;
}


void KcImActionPanel::updateImpl_()
{
	// 水平平铺button
	const auto btnSize = ImVec2{ ImGui::GetContentRegionAvail().x, 0.0f };

	// 更新界面(被分组的按钮列表)并视情触发action
	for (auto& group : groupMaps_) {
		auto& groupName = group.first;
		auto& actionList = group.second;

		if (ImGui::CollapsingHeader(groupName.c_str())) // 按分组创建header
			for (auto& act : actionList) // 依次绘制该分组下的action触发按钮
				if (ImGui::Button(act->name().c_str(), btnSize)) {
					if (!act->triggered() && act->trigger())
						triggered_.push_back(act);
				}
	}


	// 更新处于触发态的actions
	auto iter = triggered_.begin();
	while (iter != triggered_.end()) {
		auto& act = *iter;
		if (act->triggered()) {
			act->update();
			iter++;
		}
		else {
			iter = triggered_.erase(iter);
		}
	}
}
