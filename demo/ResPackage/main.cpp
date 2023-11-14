#include "EzUI/Application.h"
#include "EzUI/VLayout.h"
#include "EzUI/TextBox.h"
#include "EzUI/Button.h"
#include "EzUI/Window.h"
using namespace EzUI;

class MainFrm :public Window {
	VLayout layout;
	TextBox edit;
	Label tips;
	Button btn;
public:
	MainFrm() :Window(500, 300) {
		this->SetText(L"EzUI资源打包器");
		this->SetLayout(&layout);

		tips.SetText(L"请设置输出文件名");
		tips.SetFixedHeight(35);
		tips.Style.ForeColor = Color::Gray;
		tips.SetParent(&layout);

		edit.Style.Border = 1;
		edit.Style.Border.Color = Color::Gray;
		edit.SetFixedHeight(35);
		edit.SetParent(&layout);

		layout.Add(new Spacer);

		btn.Style.Border.Radius = 4;
		btn.Style.Border = 1;
		btn.Style.Border.Color = Color::Gray;
		btn.HoverStyle.BackColor = Color(100, 100, 100, 100);
		btn.ActiveStyle.BackColor = Color::White;

		btn.SetFixedSize({ 100,30 });
		btn.SetText(L"确认");
		btn.SetParent(&layout);

		layout.Add(new Spacer);
	}

	void OnClose(bool& close)  override {
		Application::Exit(0);
	}

	bool OnNotify(Control* sd, EventArgs& args)override {
		return __super::OnNotify(sd, args);
	}
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Application app;
	app.EnableHighDpi();

	MainFrm frm;
	frm.Show();

	return app.Exec();

};