//WIN32 desktop application UI framework (2d graphical library:direct2d,后期可能会采用其他跨平台的2d图形库对整个UI框架进行跨平台)

//email:718987717@qq.com //qq群: 758485934

#include <Windows.h>

#include "EzUI.h"
#include "Form.h"
#include "Button.h"
#include "TileList.h"
#include "VList.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Application.h"

using namespace EzUI;

class MainFrm :public Window {
public:
	MainFrm(int cx, int cy) :Window(cx, cy) {}
	void OnClose(bool& b) {
		Application::exit(0);
	}
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	Application app;//消息循环对象

	MainFrm frm(800, 600);//无边框窗口
	VLayout mainLayout;//窗口中的main布局

	mainLayout.Style.BackgroundColor = Color::White;//主布局背景颜色
	//HLayout title(&mainLayout);//标题
	//title.SetFixedHeight(45);//设置固定高度
	//title.Style.BackgroundColor = Color::Pink;//控件背景颜色
	//title.Action = ControlAction::MoveWindow;//可移动窗口的行为

	//Label text(&title);//标题文字
	//text.Action = ControlAction::MoveWindow;//可移动窗口的行为
	//text.SetText(L"hello world ");//给label设置文字

	Label closeBtn;//关闭按钮

	closeBtn.Action = ControlAction::Close;//关闭窗口的行为

	closeBtn.Style.FontFamily = L"Marlett";//设置成icon字体

	closeBtn.Style.FontSize = 13;//字体大小

	closeBtn.HoverStyle.ForeColor = Color::White;//伪样式 鼠标悬浮字体颜色变成白色

	closeBtn.SetFixedWidth(45);//设置绝对高度

	closeBtn.SetText(L"r");//窗口的关闭按钮icon


	//Label labelBottom;
	//labelBottom.SetText(L"这是一个简单的窗口示例!");

	TileList list(&mainLayout);

	for (size_t i = 0; i < 99999; i++)
	{
		Button* lb = new Button(&list);
		lb->SetText("button" + std::to_string(i));
		lb->SetFixedSize({ 50,30 });
	}

	//mainLayout.AddControl(&labelBottom);//添加控件

	frm.SetLayout(&mainLayout);//给窗口设置布局

	frm.Show();//显示窗口

	return app.exec();//进行消息循环

}