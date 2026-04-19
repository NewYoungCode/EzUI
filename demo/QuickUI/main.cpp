#include "mainForm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//app类
	Application app(hInstance);
	app.EnableHighDpi();//启用高DPI
	app.SetResource("my_res");//设定资源名称

	//窗口实例
	MainForm mainFrm;
	mainFrm.Show();
	mainFrm.CenterToScreen();
	::SetForegroundWindow(mainFrm.GetWindowHandle());//设置为前景窗口

	//开始消息循环
	int code = app.Exec();
	return code;
}
