#include "EzUI/Application.h"
#include "EzUI/VLayout.h"
#include "EzUI/TextBox.h"
#include "EzUI/Button.h"
#include "EzUI/Window.h"
#include "EzUI/Resource.h"
#include "EzUI/Task.h"
#include "EzUI/HLayout.h"
#include "EzUI/UIManager.h"

#include "WinTool.h"
#include "WebClient.h"

using namespace EzUI;
class MainFrm :public Window {
public:
	UIManager ui;
	MainFrm(const EString& cmdLine);
	void OnClose(bool& close)  override;
	bool OnNotify(Control* sd, EventArgs& args)override;
private:
	Task* task = NULL;
private:
	Label* rate_bar;
};