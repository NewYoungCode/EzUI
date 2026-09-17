#include <Windows.h>

#include "EzUI/EzUI.h"

using namespace ezui;

class HelloWindow : public Window {
public:
	HelloWindow()
	{
		SetTitle(L"Hello World");
		LoadXml("res/mainForm.htm");
		m_title = FindControl<Button>("title");
	}
	virtual void OnClose(bool& allowClose) override
	{
		auto ret = ::MessageBoxW(GetWindowHandle(), L"真的要退出吗？", L"提示", MB_OKCANCEL);
		if (ret == IDOK) {
			allowClose = true;
			Application::Exit(0);//退出整个程序
		}
		else {
			allowClose = false;
		}
		__super::OnClose(allowClose);
	}
	virtual void OnNotify(Control* sender, EventArgs* args)override {
		__super::OnNotify(sender, args);
		if (args->EventType() == Event::MouseDown && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
			if (sender == m_title) {
				m_title->SetText(L"hello 欢迎使用ezui!");
				m_title->SetStyle("color:#FF0000;font-size:30px;", VisualState::Normal);
				m_title->Invalidate();
			}
		}
	}
private:
	Button* m_title = nullptr;
};

#include "EzUI/window/ColorDialog.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	Application app(hInstance);
	app.EnableHighDpi();
	app.SetResource("my_res");

	//Color color = Color::Red;
	//std::vector<Color> custom{ Color::Red,Color::Gray,Color::Blue };
	//ColorDialog colorDialog(NULL, color, &custom);
	//colorDialog.ShowModal();

	HelloWindow window;
	window.Show();
	window.CenterToScreen();
	::SetForegroundWindow(window.GetWindowHandle());

	return app.Exec();
}
