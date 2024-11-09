#include "global.h"
#include "MainFrm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	//#ifdef _DEBUG
	//	AllocConsole();
	//	FILE* fp = NULL;
	//	freopen_s(&fp, "CONOUT$", "w+t", stdout);
	//#endif
	//	printf("hello EzUI !");

	Curl_Global_Init();//初始化curl
	EzUI::Application app;
	//EzUI::Application app("res.bin");
	app.EnableHighDpi();//启用高dpi适配

	MainFrm frm;
	frm.Show();

	return app.Exec();
}