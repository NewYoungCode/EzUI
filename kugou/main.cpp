#include "global.h"
#include "MainFrm.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	//SetProcessDPIAware();
	//SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);


	Curl_Global_Init();

	//EzUI::Application app;//不指定资源
	EzUI::Application app("res.zip");//指定资源文件
	//EzUI::Application app(ID_FILE,"zip","");//指定vs中资源文件
	MainFrm frm;
	frm.Show();


	return app.exec();
}