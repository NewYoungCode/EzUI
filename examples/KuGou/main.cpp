#include "global.h"
#include "MainFrm.h"
#include "timer.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Curl_Global_Init();
	
	ui::Application app;
	mvicon = new Image(L"imgs/mvicon.png");
	MainFrm frm;
	frm.Show();

	app.exec();
}