#include "global.h"
#include "MainFrm.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{


	ui::Application app;

	MainFrm frm;
	frm.Show();

	app.exec();
}