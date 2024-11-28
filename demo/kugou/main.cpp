#include "global.h"
#include "mainFrm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Curl_Global_Init();//初始化curl
	EzUI::Application app;
	//EzUI::Application app("res.bin");
	app.EnableHighDpi();//启用高dpi适配

	MainFrm frm;
	frm.Show();

	return app.Exec();
}