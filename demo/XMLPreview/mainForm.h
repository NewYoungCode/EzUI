#include "EzUI/Window.h"
#include "EzUI/Application.h"
#include "EzUI/VLayout.h"
#include "EzUI/HLayout.h"
#include "EzUI/LayeredWindow.h"
#include "EzUI/TextBox.h"
#include "EzUI/Button.h"
#include "EzUI/Task.h"
#include "EzUI/Timer.h"
#include "EzUI/Frame.h"

using namespace ezui;

//主窗口
class MainFrm :public Window {
public:
	Task* task = NULL;
	VLayout main;
	HLayout hbox;
	Window* previewWnd = NULL;
	MainFrm(int cx, int cy);
	virtual void OnSize(const Size& sz)override;
	virtual void OnNotify(Control* sd, EventArgs& args)override;
	virtual LRESULT WndProc(UINT msg, WPARAM wp, LPARAM lp)override;
	void OnClose(bool& b)override;
	void SetWorkDir(const UIString& dir);
	void CreatePreview(const UIString& dir);
	virtual ~MainFrm();
};
