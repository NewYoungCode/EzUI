#include "MainForm.h"

using namespace ezui;

MainForm::MainForm() : Form()
{
    SetText(L"迅雷下载 Demo");
    SetResizable(true);
    LoadXml("res/mainForm.htm");
    SetSize({ 1026, 682 });
    SetMinSize({ 980, 640 });
}

void MainForm::OnClose(bool& bClose)
{
    Application::Exit(0);
}

MainForm::~MainForm() = default;
