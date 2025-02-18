#include "KcActionShowFileDialog.h"
#include <assert.h>
#include "ImFileDialog/ImFileDialog.h"
#include "KuStrUtil.h"


KcActionShowFileDialog::KcActionShowFileDialog(KeType type, const std::string_view& title,
	const std::string_view& filter, const std::string_view& dir)
	: KvAction(title)
{
	key_ = "##_filedialog_" + KuStrUtil::toString(id());
	type_ = type;
	filter_ = filter;
	dir_ = dir;
}


bool KcActionShowFileDialog::trigger()
{
	bool res{};

	if (type_ == KeType::k_save) {
		res = ifd::FileDialog::Instance().Save(key_, name(), filter_, dir_);
	}
	else {
		res = ifd::FileDialog::Instance().Open(key_, name(), filter_, false, dir_);
	}

	if (res)
	    state_ = KeState::k_triggered;

	return res;
}


void KcActionShowFileDialog::update()
{
	assert(triggered());

	if (ifd::FileDialog::Instance().IsDone(key_)) {
		if (ifd::FileDialog::Instance().HasResult()) {
			result_ = ifd::FileDialog::Instance().GetResult().string();
			state_ = KeState::k_done;
		}
		else {
			result_.clear();
			state_ = KeState::k_cancelled;
		}

		ifd::FileDialog::Instance().Close();
	}
}
