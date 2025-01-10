#include "mainForm.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Application app;
	app.EnableHighDpi();

	MainFrm frm(lpCmdLine);
	frm.Show();

	return app.Exec();
};