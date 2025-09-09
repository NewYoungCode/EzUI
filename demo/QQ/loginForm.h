#pragma once
#include "ezui/Application.h" //app类
#include "EzUI/Window.h" //基础窗口类
#include "EzUI/Button.h" //按钮
#include "EzUI/TextBox.h" //文本框
#include "EzUI/CheckBox.h" //复选框
#include "EzUI/PictureBox.h" //图片控件
#include "EzUI/TabLayout.h" //选项卡控件
#include "EzUI/VLayout.h" //垂直布局
#include "EzUI/HLayout.h"//水平布局
#include "EzUI/VListView.h"//垂直带滚动条列表
#include "EzUI/HListView.h"//水平带滚动条列表
#include "EzUI/TileListView.h"//瓦片列表
#include "EzUI/LayeredWindow.h"//分层窗口类
#include "ezui/UIManager.h"//ui管理类(使用xml生成控件)

#pragma comment(lib,"ezui.lib")
using namespace ezui;

using Form = LayeredWindow; //支持异形透明窗口(带阴影)
//using Form = BorderlessWindow; //常规无边框窗口(带阴影)
//using Form = Window; //标准窗口(带系统标题栏)

//登录窗口
class LoginForm :public Form
{
private:
	//ui管理类
	UIManager umg;
protected:
	virtual void OnNotify(Control* sender, EventArgs& args)override;//重载事件通知
	virtual void OnClose(bool& close)override;//当窗口关闭的时候
public:
	LoginForm();
	virtual ~LoginForm();
};

