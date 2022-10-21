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
//}//��������Ļص�����
//
//HWND GetWorkerW() {
//	int result;
//	HWND windowHandle = FindWindow(L"Progman", nullptr);
//	SendMessageTimeout(windowHandle, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, (PDWORD_PTR)&result);
//	EnumWindows(EnumWindowsProc, (LPARAM)nullptr);
//	    ShowWindow(_workerw,SW_HIDE);
//	return windowHandle;
//}//��ȡ������ײ�ľ��
//

void test1() {
	for (size_t i = 0; i < 10; i++)
	{
		Windows::Timer* t = new Windows::Timer();

		std::thread::id this_id = std::this_thread::get_id();
		unsigned int tid = *(unsigned int*)&this_id;// threadid ת�� unsigned int

		t->Name = std::to_string(tid) + " number " + std::to_string(i);

		t->Interval = 100;
		t->Tick = [=](Windows::Timer* tm)->void {
			Debug::Log("%s", tm->Name.c_str());
		};
		t->Start();
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Curl_Global_Init();

	ui::Application app;
	//test1();
	/*std::thread* t1 = new std::thread(test1);
	std::thread* t2 = new std::thread(test1);*/

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

	MainFrm frm;
	//frm.Tag = (UINT_PTR)&player;
	frm.Show();
	return app.exec();
}