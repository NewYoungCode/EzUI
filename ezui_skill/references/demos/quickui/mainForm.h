#include "EzUI/EzUI.h"

using namespace ezui;

class MainForm : public Window {
public:
    MainForm();
    void OnNotify(Control* sender, EventArgs* args) override;
    void OnClose(bool& bClose) override;
    ~MainForm() override;

private:
    void ApplySearchFilter(const UIString& query);
    void UpdateSearchMeta(int visibleCount, int totalCount, const UIString& query);
};
