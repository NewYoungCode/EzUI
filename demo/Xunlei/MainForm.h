#pragma once

#include "global.h"

class MainForm : public Form {
public:
    MainForm();
    ~MainForm() override;

protected:
    void OnClose(bool& allowClose) override;
};
