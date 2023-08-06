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

	//EzUI::Application app;//��ָ����Դ
	EzUI::Application app("res.zip");//ָ����Դ�ļ�
	//EzUI::Application app(ID_FILE,"zip","");//ָ��vs����Դ�ļ�
	MainFrm frm;
	frm.Show();


	return app.exec();
}