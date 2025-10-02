#pragma once
#include "global.h"


//登录窗口
class LoginForm :public Form
{
private:
	//ui管理类
	UILoader umg;
protected:
	virtual void OnNotify(Control* sender, EventArgs& args)override;//重载事件通知
	virtual void OnClose(bool& close)override;//当窗口关闭的时候
public:
	LoginForm();
	virtual ~LoginForm();
};

