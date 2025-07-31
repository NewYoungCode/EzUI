#include "global.h"
#include "mainFrm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Application app;//app类
	app.EnableHighDpi();//启用高dpi适配
	app.SetResource("my_res");//设定资源名称

	MainFrm frm;//主窗口

	//给窗口添加淡入效果
	Animation* ant = new Animation(&frm);//绑定父对象为frm,则ant无需手动释放
	ant->SetStartValue(0.1);
	ant->SetEndValue(1.0);
	ant->ValueChanged = [&](double value) {
		Invoke([&] {
			frm.Opacity = value;//修改透明度
			frm.Invalidate();//刷新
			});
		};

	frm.Opacity = 0.1;
	ant->Start(200);//开始动画

	frm.Show();

	return app.Exec();
}