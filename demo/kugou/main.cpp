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
	printf("福来day");

	Curl_Global_Init();

	//if(0){
	//	EzUI::Application app;//指定资源文件
	//	//打包资源文件
	//	Resource::Package(Path::StartPath()+"/res","d:/res.bin");
	//}

	EzUI::Application app("res.bin");//指定资源文件
	app.EnableHighDpi();

	MainFrm frm;
	frm.Show();
	

	return app.Exec();
}