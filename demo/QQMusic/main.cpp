#include "MainForm.h"
#include "LrcPanel.h"
#include "MusicSeekBar.h"
#include "RecordCover.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    ezui::Application app(hInstance);
    app.EnableHighDpi();
    app.SetResource("my_res");
    ezui::RegisterControl<LrcPanel>("lrcPanel");
    ezui::RegisterControl<MusicSeekBar>("musicSeekBar");
    ezui::RegisterControl<RecordCover>("recordCover");

    MainForm form;
    form.CenterToScreen();
    form.Show();
    ::SetForegroundWindow(form.GetWindowHandle());

    return app.Exec();
}
