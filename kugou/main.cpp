#include "global.h"
#include "MainFrm.h"

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

	printf("����day");

	Curl_Global_Init();
	EzUI::Application app("res.zip");//ָ����Դ�ļ�
	//EzUI::Application app;//��ָ����Դ
	//EzUI::Application app(ID_FILE,"zip","");//ָ��vs����Դ�ļ�
	app.EnableHighDpi();

	MainFrm frm;
	frm.Show();

	return app.Exec();
}