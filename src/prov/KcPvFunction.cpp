#include "KcPvFunction.h"
#include "dsp/KcContinuedFn.h"
#include "dsp/KtSampledExpr.h"
#include "exprtkX/KcExprtk2d.h"
#include "imguix.h"


KcPvFunction::KcPvFunction()
    : super_("Function", nullptr)
    , exprs_(3) // Ŀǰ�̶�dim = 2
{
    exprs_[0] = "sin(x) * cos(y)";
    exprs_[1] = "sin(x) * sin(y)";
    exprs_[2] = "cos(x)";

    iranges_[0] = { 0, KuMath::pi };
    iranges_[1] = { 0, KuMath::pi * 2 };

    counts_[0] = counts_[1] = 32;

    std::array<std::shared_ptr<KvContinued>, 3> ptrs;

    for (int i = 0; i < 3; i++) {
        auto expr = std::make_shared<KcExprtk2d>();
        expr->compile(exprs_[i]);
        assert(expr->ok());
        ptrs[i] = std::make_shared<KcContinuedFn>(
            [expr](kReal x[]) { return expr->value(x); }, 1);
    }

    auto d = std::make_shared<KtSampledExpr<2>>(ptrs);
    setData(d);
    updateDataSamplings_();
}


void KcPvFunction::updateDataSamplings_()
{
    auto d = std::dynamic_pointer_cast<KtSampledExpr<2>>(data());

    for (int i = 0; i < 2; i++)
        d->reset(i, iranges_[i].first, (iranges_[i].second - iranges_[i].first) / (counts_[i] - 1));

    kIndex shape[2]{ counts_[0], counts_[1] };
    d->resize(shape, 0);

    notifyChanged(true, true);
}


void KcPvFunction::showPropertySet()
{
    super_::showPropertySet();
    ImGui::Separator();

    auto d = std::dynamic_pointer_cast<KtSampledExpr<2>>(data());

    if (ImGuiX::treePush("Expressions", true)) {
        for (unsigned i = 0; i < exprs_.size(); i++) {
            std::string label = "f1";
            label.back() += i;
            ImGui::PushID(i);
            ImGuiX::exprEdit(label.c_str(), exprs_[i].c_str(), 2,
                [this, i, d](std::shared_ptr<KvData> data, const char* text) {
                    auto expr = std::dynamic_pointer_cast<KvContinued>(data);
                    assert(expr);

                    exprs_[i] = text;     
                    d->setExpr(i, expr);
                });
            ImGui::PopID();
        }
        ImGuiX::treePop();
    }

    if (ImGuiX::treePush("Sampling", true)) {
        for (unsigned i = 0; i < exprs_.size() - 1; i++) {
            std::string label = "RangeX";
            label.back() += i;      
            if (ImGui::DragFloatRange2(label.c_str(), &iranges_[i].first, &iranges_[i].second))
                updateDataSamplings_();
        }

        if (ImGui::DragInt2("Count", counts_, 1, 2, 1024)) {
            if (counts_[0] < 2) counts_[0] = 2;
            if (counts_[1] < 2) counts_[1] = 2;
            updateDataSamplings_();
        }

        ImGuiX::treePop();
    }
}


kRange KcPvFunction::range(kIndex outPort, kIndex axis) const
{
    return axis < 2 ? oranges_[axis] : super_::range(outPort, axis);
}



void KcPvFunction::notifyChanged(bool specChanged, bool dataChanged)
{
    super_::notifyChanged(specChanged, dataChanged);

    oranges_[0] = data() ? data()->range(0) : kRange(0, 0);
    oranges_[1] = data() ? data()->range(1) : kRange(0, 0);
}
