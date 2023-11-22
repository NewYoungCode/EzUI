#include "global.h"
#include "MainFrm.h"
//#include "EzUI/FileSystem.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

#ifdef _DEBUG
	AllocConsole();
	FILE* fp = NULL;
	freopen_s(&fp, "CONOUT$", "w+t", stdout);
#endif
	printf("hello world");

	Curl_Global_Init();
	
	EzUI::Application app("res.bin");
	app.EnableHighDpi();

	MainFrm frm;
	frm.Show();

	return app.Exec();
}