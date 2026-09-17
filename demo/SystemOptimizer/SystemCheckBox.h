#pragma once

#include "EzUI/EzUI.h"

class SystemCheckBox : public ezui::CheckBox {
public:
    SystemCheckBox();

protected:
    void OnForePaint(ezui::PaintEventArgs* args) override;
};
