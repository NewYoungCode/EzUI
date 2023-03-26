#include <Windows.h>
#include "Application.h"
#include "MainFrm.h"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	EzUI::Application app;
	
	MainFrm frm;
	frm.Show();

	return app.exec();
}