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
#include "UIManager.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Application app;
	StopWatch sw;
	MainFrm form(1022, 670);
	form.Zoom = true;


	//form.SetLayout(UIManager::LoadLayout(utf8("../html/layout.html")));
	//form._layout->Style.BackgroundColor = "rgb(255,100,255)";
	//form.FindControl("title")->Style.ForeColor = "rgb(0,0,0,0.8)";
	//$(form._layout->FindControl("name", "song")).Css("active{color:#003d6a;background-color:rgb(5,200,200,1)}").Refresh();
	//$(form._layout->FindControl("name", "song")).Attr("style", "color:#0x101d32").Refresh();
	//$(form._layout).Css("background-color:rgb(0,255,0,0.5)").Refresh();
	form.Show();
	Debug::Log(utf8("窗口启动耗时 %d ms"), sw.ElapsedMilliseconds());
	return	app.exec();
}


