#include "global.h"
#include "mainFrm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Curl_Global_Init();//初始化curl
	Application app;
	app.SetResource("my_res");

	app.EnableHighDpi();//启用高dpi适配

	MainFrm frm;
	frm.Show();
	return app.Exec();
}