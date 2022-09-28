#include "global.h"
#include "MainFrm.h"
#include "timer.h"
#include "Button.h"

class DesktopWallper :public BorderlessWindow
{
public:
	DesktopWallper(int w, int h, HWND ownerWnd) : BorderlessWindow(w, h, ownerWnd) {};
};
HWND _workerw = nullptr;
//
//inline BOOL CALLBACK EnumWindowsProc(_In_ HWND tophandle, _In_ LPARAM topparamhandle)
//{ 
//	HWND defview = FindWindowEx(tophandle, 0, L"SHELLDLL_DefView", nullptr);
//	if (defview != nullptr)
//	{
//		_workerw = FindWindowEx(0, tophandle, L"WorkerW", 0);
//	}
//	return true;
//}//遍历句柄的回调函数
//
//HWND GetWorkerW() {
//	int result;
//	HWND windowHandle = FindWindow(L"Progman", nullptr);
//	SendMessageTimeout(windowHandle, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, (PDWORD_PTR)&result);
//	EnumWindows(EnumWindowsProc, (LPARAM)nullptr);
//	    ShowWindow(_workerw,SW_HIDE);
//	return windowHandle;
//}//获取桌面最底层的句柄
//



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Curl_Global_Init();
	ui::Application app;

	//HWND workWnd = GetWorkerW();
	//::ShowWindow(workWnd, SW_HIDE);
	/*HWND wnd = GetWorkerW();

	DesktopWallper dw(1920, 1080, wnd);
	dw.SetText(L"EzUI Wallper");
	dw.CloseShadow();
	::SetParent(dw.Hwnd(), wnd);

	Layout l;
	VlcPlayer  player;
	player.Dock = DockStyle::Fill;
	l.AddControl(&player);
	dw.SetLayout(&l);

	::SetWindowPos(wnd, HWND_TOP, 0, 0, 1920, 1080, SWP_SHOWWINDOW);
	dw.Show();*/

	/*player.OpenPath("E:\\CloudMusic\\MV\\x.mp4");
	player.Play();*/

	//Window mainFrm(500, 300);
	//Layout ll;
	//ll.Style.BackgroundColor = Color::Pink;
	//Button btn;
	//btn.SetSize({ 100,35 });
	//btn.SetText("Play");
	//ll.AddControl(&btn);
	//mainFrm.SetLayout(&ll);
	//mainFrm.Show();
	//btn.MouseClick = [&](Control*, MouseButton, const Point&)->void {
	//	player.OpenPath("E:\\CloudMusic\\MV\\x.mp4");
	//	player.Play();
	//};

	mvicon = new Image(L"imgs/mvicon.png");
	MainFrm frm;
	//frm.Tag = (UINT_PTR)&player;
	frm.Show();

	app.exec();
}