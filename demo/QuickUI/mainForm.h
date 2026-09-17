#include "EzUI/EzUI.h"

using namespace ezui;

class MainForm : public Window {
public:
    MainForm();
    void OnNotify(Control* sender, EventArgs* args) EZUI_OVERRIDE;
    void OnClose(bool& allowClose) EZUI_OVERRIDE;
    ~MainForm() EZUI_OVERRIDE;

private:
    void ApplySearchFilter(const UIString& query);
    void UpdateSearchMeta(int visibleCount, int totalCount, const UIString& query);
};
