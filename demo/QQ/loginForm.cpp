#include "loginForm.h"

LoginForm::LoginForm() :Form(320, 448)
{
	umg.LoadXml("res/loginForm.htm");//加载xml里面的控件与样式
	umg.SetupUI(this);
}
void LoginForm::OnNotify(Control* sender, EventArgs& args)
{
	if (args.EventType == Event::OnMouseDown) {
		if (sender->Name == "btnLogin") {
			TextBox* editUser = (TextBox*)FindControl("user");
			TextBox* editpwd = (TextBox*)FindControl("pwd");
			CheckBox* ckbox = (CheckBox*)FindControl("ckbox");
			if (!ckbox->GetCheck()) {
				::MessageBox(Hwnd(), L"请阅读协议并勾选!", L"提示", MB_OK);
				return;
			}
			UIString user = editUser->GetText();
			UIString pwd = editpwd->GetText();
			if (user == "718987717" && pwd == "123456") {
				::MessageBox(Hwnd(), L"登录成功!", L"提示", MB_OK);
			}
			else {
				::MessageBox(Hwnd(), L"用户名或密码错误!", L"提示", MB_OK);
			}
		}
		if (sender->Name == "btnExit") {
			Application::Exit();
		}
		if (!sender->GetAttribute("url").empty()) {
			::ShellExecuteA(0, "open", sender->GetAttribute("url").c_str(), "", "", SW_SHOW);
		}
	}
	__super::OnNotify(sender, args);
}

void LoginForm::OnClose(bool& bClose)
{
	//bClose = false;
	Application::Exit();
}

LoginForm::~LoginForm()
{

}
