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

	struct Selector
	{
	public:
		Selector(const char* str) {}
		size_t css(const char* key, const char* value) {}
		size_t attr(const char* key, const char* value) {}
	};

#define $ Selector
	$("").css("color", "red");
	$("").attr("width", "35");


	Application app;
	StopWatch sw;
	MainFrm form(1022, 670);

	/*auto layouts = UIManager::LoadControl(utf8("../html/layout.html"));
	form.SetLayout((Layout*)layouts.at(0));
	form._layout->Style.BackgroundColor = "rgb(255,100,255)";
	form.FindControl("title")->Style.ForeColor = "rgb(0,0,0,0.8)";*/

	form.Show();
	Debug::Log("窗口启动耗时 %d ms", sw.ElapsedMilliseconds());
	return	app.exec();
}


