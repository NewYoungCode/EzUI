#include "MainForm.h"
#include "SystemCheckBox.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    ezui::Application app(hInstance);
    app.EnableHighDpi();
    app.SetResource("my_res");
    ezui::RegisterControl<SystemCheckBox>("checkbox");

    MainForm form;
    form.Show();
    form.CenterToScreen();
    ::SetForegroundWindow(form.GetWindowHandle());

    return app.Exec();
}
