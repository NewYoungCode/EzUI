#include "global.h"
#include "mainFrm.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Curl_Global_Init();//初始化curl
	Application app;
	app.SetResource("my_res");

	app.EnableHighDpi();//启用高dpi适配

	//while (true)
	//{
	//	VListView view;
	//	for (size_t i = 0; i < 999; i++)
	//	{
	//		//btn无需释放 生命周期跟随view
	//		Button* btn = new Button(&view);
	//		btn->SetFixedSize({ 100,30 });
	//		btn->SetText(L"按钮1");
	//		view.Add(btn);
	//	}
	//	int pause = 0;
	//}

	MainFrm frm;

	//给窗口添加淡入效果
	Animation* ant = new Animation(&frm);
	ant->SetStartValue(0.1);
	ant->SetEndValue(1.0);
	ant->ValueChanged = [&](double value) {
		Invoke([&] {
			frm.Opacity = value;
			frm.Invalidate();
			});
		};
	ant->Start(200);//开始动画

	frm.Show();
	return app.Exec();
}