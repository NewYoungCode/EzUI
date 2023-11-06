#include "UIWindow.h"
namespace EzUI {
    UIWindow::UIWindow(int w, int h):Window(w,h)
    {
    }
    Control* UIWindow::InsertControl(Control* ctl)
    {

        return nullptr;
    }
    bool UIWindow::OnNotify(Control* sender, EventArgs& arg) {
        return __super::OnNotify(sender, arg);
    }

};
