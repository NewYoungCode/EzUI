//WIN32 desktop application UI framework

//email:718987717@qq.com
//qq群: 758485934

#include "EzUI.h"

#include "Application.h"

#include "Form.h"

#include "Button.h"

#include "TileLayout.h"

#include "VList.h"

using namespace EzUi;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
	
{

	Application app;//消息循环对象
	BorderlessWindow frm(800, 600);//无边框窗口
	
	VLayout mainLayout;//主要布局
	mainLayout.Style.BackgroundColor = Color::White;//主布局背景颜色

	HLayout title;//标题
	title.SetFixedHeight(45);//设置固定高度
	title.Style.BackgroundColor = Color::Pink;//控件背景颜色
	title.Action = ControlAction::MoveWindow;//可移动窗口的行为

	Label text;//标题文字
	text.Action = ControlAction::MoveWindow;//可移动窗口的行为
	text.SetText(L"hello world ");
	title.AddControl(&text);

	Label closeBtn;//关闭按钮
	closeBtn.Action = ControlAction::Close;
	closeBtn.Style.FontFamily = L"Marlett";
	closeBtn.Style.FontSize = 13;
	closeBtn.HoverStyle.ForeColor = Color::White;//伪样式 鼠标悬浮字体颜色变成白色
	closeBtn.SetFixedWidth(45);
	closeBtn.SetText(L"r");
	title.AddControl(&closeBtn);

	Label labelBottom;
	labelBottom.SetText(L"这是一个简单的窗口示例 !");

	mainLayout.AddControl(&title);//添加标题
	mainLayout.AddControl(&labelBottom);//

	frm.SetLayout(&mainLayout);//给窗口设置布局
	frm.Show();//显示窗口

	return app.exec();//进行消息循环
}
