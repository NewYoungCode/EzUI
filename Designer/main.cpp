#include "stdafx.h"
#include "MainFrm.h"
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Application app;

	MainFrm mainFrm(1100, 800);
	mainFrm.Show();

	return app.exec();
}