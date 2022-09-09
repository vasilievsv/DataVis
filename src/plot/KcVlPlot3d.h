#pragma once
#include "KvPlot.h"
#include <vlCore/Object.hpp>
#include <memory>

namespace vlQt6 { class Qt6Widget; }
namespace vl { class Applet; }
class QWidget;
class KglPaint;

class KcVlPlot3d : public KvPlot
{
public:
    KcVlPlot3d(QWidget* parent = nullptr);
    ~KcVlPlot3d();

    vl::Applet* applet() { return applet_.get(); }

    void show(bool b) override;

    bool visible() const override;

    void* widget() const override;

    void update(bool immediately = true) override;

    vec4 background() const override;
    void setBackground(const vec4& clr) override;

    void autoProject();

private:
    vl::ref<vl::Applet> applet_;
    vl::ref<vlQt6::Qt6Widget> widget_;
    std::unique_ptr<KglPaint> paint_;
};
