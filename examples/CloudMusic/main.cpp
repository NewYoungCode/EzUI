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

	/*WCHAR buf[]{ L"一百万个可能" };
	HWND wnd = (HWND)0x00080154;
	::SetFocus(wnd);
	for (size_t i = 0; i < sizeof(buf) / 2; i++)
	{
		LRESULT r = ::SendMessageW(wnd, WM_CHAR, (WPARAM)buf[i], NULL);
		int pause = 0;
	}
	::Sleep(500);
	::SendMessageW(wnd, WM_CHAR, (WPARAM)VK_RETURN, NULL);
	::SendMessageW(wnd, WM_KEYDOWN, (WPARAM)VK_RETURN, NULL);

	return 0;*/

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


