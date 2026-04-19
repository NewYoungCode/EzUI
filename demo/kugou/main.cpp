#include "global.h"
#include "mainForm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Application app;//app类
	app.EnableHighDpi();//启用高dpi适配
	app.SetResource("my_res");//设定vs中的自定义资源名称 资源名称位于kugou.rc文件中

	ezui::RegisterControl<VlcPlayer>("video");//注册vlcPlayer控件 视频播放控件 xml中使用<video></video>标签即可使用这个控件
	ezui::RegisterControl<LrcPanel>("lrcPanel");//注册歌词面板控件 xml中使用<lrcPanel></lrcPanel>标签即可使用这个控件

	MainFrm frm;//主窗口
	frm.CenterToScreen();//屏幕居中
	frm.Show();//显示窗口
	::SetForegroundWindow(frm.GetWindowHandle());//将窗口置于前台

	//设置窗口阴影颜色
	//frm.GetShadowHost()->SetShadowColor(Color::Red);

	return app.Exec();//进行消息循环
}