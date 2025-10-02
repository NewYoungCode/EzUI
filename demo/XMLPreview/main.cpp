#include "mainForm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	
	Application app;//app类
	app.EnableHighDpi();//启用高dpi适配

	MainFrm frm(1200, 900);//主窗口

	frm.CenterToScreen();//屏幕居中
	frm.ShowMaximized();
	//frm.SetTopMost(true);

	return app.Exec();
}