#include "framework.h"
#include "Resource.h"
#include <thread>
#include "ezui.h"
#include "VLayout.h"
#include "HLayout.h"
#include "Form.h" 
#include "Button.h"
#include "VList.h"
#include "Edit.h"
#include "MainFrm.h"
#include <UIManager.h>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Application app;
	StopWatch sw;
	MainFrm form(1022, 670);

	auto layouts = UIManager::LoadControl(utf8("../html/layout.html"));
	form.SetLayout((Layout*)layouts.at(0));

	form.Show();
	Debug::Log("窗口启动耗时 %d ms", sw.ElapsedMilliseconds());
	return	app.exec();
}


