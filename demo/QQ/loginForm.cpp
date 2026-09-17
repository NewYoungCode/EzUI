#include "loginForm.h"

LoginForm::LoginForm() :Form()
{
	this->LoadXml("res/loginForm.htm");//加载xml里面的控件与样式
	this->SetSize(Size(320, 448));
}
void LoginForm::OnNotify(Control* sender, EventArgs* args)
{
	if (args->EventType() == Event::MouseDown) {
		if (sender->GetName() == "btnLogin") {
			TextBox* editUser = (TextBox*)FindControl("user");
			TextBox* editpwd = (TextBox*)FindControl("pwd");
			CheckBox* ckbox = (CheckBox*)FindControl("ckbox");
			if (!ckbox->GetCheck()) {
				::MessageBox(GetWindowHandle(), L"请阅读协议并勾选!", L"提示", MB_OK);
				return;
			}
			UIString user = editUser->GetText();
			UIString pwd = editpwd->GetText();
			if (true || (user == "718987717" && pwd == "123456")) {
				::MessageBox(GetWindowHandle(), L"登录成功!", L"提示", MB_OK);
				this->Close(1);
			}
			else {
				::MessageBox(GetWindowHandle(), L"用户名或密码错误!\n账号:718987717\n密码:123456", L"提示", MB_OK);
			}
		}
		if (sender->GetName() == "btnExit") {
			Application::Exit();
		}
		if (!sender->GetAttribute("url").empty()) {
			::ShellExecuteA(0, "open", sender->GetAttribute("url").c_str(), "", "", SW_SHOW);
		}
	}
	__super::OnNotify(sender, args);
}

void LoginForm::OnClose(bool& allowClose)
{
	//allowClose = false;
}

LoginForm::~LoginForm()
{

}
