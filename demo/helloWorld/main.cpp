#include <Windows.h>

#include "EzUI/EzUI.h"

using namespace ezui;

class HelloWindow : public Window {
public:
	HelloWindow()
	{
		SetText(L"Hello World");
		SetSize(480, 320);
		m_root.Style->BackColor = Color::White;
		m_title.SetText(L"Hello World");
		m_title.Style->FontSize = 24;
		m_title.SetMargin(24);
		m_root.AddChild(&m_title);
		SetLayout(&m_root);
	}
	virtual void OnClose(bool& bClose) override
	{
		auto ret = ::MessageBoxW(GetWindowHandle(), L"真的要退出吗？", L"提示", MB_OKCANCEL);
		if (ret == IDOK) {
			bClose = true;
			Application::Exit(0);//退出整个程序
		}
		else {
			bClose = false;
		}
		__super::OnClose(bClose);
	}
	virtual void OnNotify(Control* sender, EventArgs* args)override {
		__super::OnNotify(sender, args);
		if (args->EventType() == Event::MouseDown && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
			if (sender == &m_title) {
				m_title.SetText(L"hello 欢迎使用ezui!");
				m_title.Style->ForeColor = Color::Red;
				m_title.Style->FontSize = 30;
				m_title.Invalidate();
			}
		}
	}
private:
	VLayout m_root;
	Button m_title;
};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	Application app(hInstance);
	app.EnableHighDpi();

	HelloWindow window;
	window.Show();
	window.CenterToScreen();
	::SetForegroundWindow(window.GetWindowHandle());

	return app.Exec();
}
