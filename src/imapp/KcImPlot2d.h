#pragma once
#include "KvImWindow.h"
#include "plot/KvPlot2d.h"
#include "KtCamera.h"


class KcImPlot2d : public KvImWindow, public KvPlot2d
{
public:

	KcImPlot2d(const std::string_view& name);

	void setVisible(bool b) override {
		KvImWindow::setVisible(b);
	}

	bool visible() const override {
		return KvImWindow::visible();
	}

	std::string title() const override {
		return KvImWindow::name();
	}

	std::string& title() override {
		return KvImWindow::name();
	}

	std::string label() const override;

	mat4 projMatrix() const override {
		return camera_.projMatrix();
	}
	void setProjMatrix(const mat4& pm) override {
		camera_.projMatrix() = pm;
	}

	const KtCamera<float_t>& camera() const { return camera_; }

private:
	void updateImpl_() override;

private:
	KtCamera<float_t> camera_;
};
