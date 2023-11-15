#include "EzUI/Application.h"
#include "EzUI/VLayout.h"
#include "EzUI/TextBox.h"
#include "EzUI/Button.h"
#include "EzUI/Window.h"
#include "EzUI/Resource.h"
#include "EzUI/Task.h"
#include "EzUI/HLayout.h"
using namespace EzUI;


class MainFrm :public Window {
	VLayout layout;
	TextBox edit;
	Label tips;
	Button btn;
	Label tips2;
	EString resFile;
	EString resDir;

	HLayout bottom;
	Label bar;
	Task* task = NULL;
public:
	MainFrm(const EString& cmdLine);
	void OnClose(bool& close)  override;
	bool FileExists(const EString& fileName);
	bool OnNotify(Control* sd, EventArgs& args)override;
	virtual ~MainFrm();
};
