//WIN32 desktop application UI framework (2d graphical library:direct2d,后期可能会采用其他跨平台的2d图形库对整个UI框架进行跨平台)

//email:718987717@qq.com //qq群: 758485934

#include <Windows.h>

#include "EzUI/EzUI.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/Button.h"
#include "EzUI/TileListView.h"
#include "EzUI/VListView.h"
#include "EzUI/HListView.h"
#include "EzUI/HLayout.h"
#include "EzUI/VLayout.h"
#include "EzUI/Application.h"
#include "EzUI/TextBox.h"
#include "EzUI/ComBoBox.h"

using namespace ezui;

class MainFrm :public Window {
public:
	VLayout* main;
	MainFrm(int cx, int cy) :Window(cx, cy) {

		//以下代码专用与测试释放托管问题

		main = new VLayout;
		auto lb = new Label(this);

		auto lb2 = new Label(lb);
		lb2->SetText(L"999999");
		lb->Add(lb2);

		lb->SetText(L"666");
		main->Add(lb);

		SetLayout(main);
	}

	virtual ~MainFrm() {
		delete main;
	}
	void OnClose(bool& b) {
		if (this->GetText() == "modal") {
			//Application::Exit(0);

		}
		else {
			Application::Exit(0);
		}
	}
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Application app;//消息循环对象

	//{
	//	MainFrm frm(800, 600);//无边框窗口
	//	frm.Show();
	//	return app.Exec();
	//}

	MainFrm frm(800, 600);//无边框窗口
	VLayout mainLayout;//窗口中的main布局

	mainLayout.Style.BackColor = Color::White;//主布局背景颜色
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
	HListView list;
	list.SetParent(&mainLayout);
	//list.SetAutoHeight(true);
	for (size_t i = 0; i < 10; i++)
	{
		Label* lb = new Label;// (&list);
		//lb->Dock = DockStyle::Horizontal;
		lb->SetText(std::to_string(i));
		lb->Name = std::to_string(i);
		lb->SetFixedSize({ 100,30 });
		lb->Style.BackColor = Color::LightGray;
		lb->HoverStyle.BackColor = Color::Red;
		lb->Margin = 1;
		lb->EventHandler = [&list](Control* sd, const EventArgs& arg)->void {
			if (arg.EventType == Event::OnMouseDown) {
				/*list.Remove(sd);
				list.Invalidate();*/
				MainFrm frm(500, 300);//无边框窗口
				frm.SetText("modal");
				VLayout mainLayout;//窗口中的main布局
				mainLayout.Style.BackColor = Color::White;//主布局背景颜色
				frm.SetLayout(&mainLayout);
				frm.ShowModal();
			}
			};
		list.Add(lb);
	}

	ComboBox cbox;
	cbox.Style.Border = 1;
	cbox.Style.Border.Color = Color::Red;

	cbox.SetFixedSize({ 100,30 });
	cbox.SetParent(&mainLayout);
	cbox.AddItem(L"-请选择-");
	cbox.AddItem(L"青菜");
	cbox.AddItem(L"牛肉");
	cbox.AddItem(L"猪肉");

	TextBox text;
	text.SetParent(&mainLayout);
	//text.SetFixedSize({ 100,50 });
	text.Style.Border.Color = Color::Gray;
	text.Style.Border = 1;
	text.SetMultiLine(true);
	text.Margin.Left = 20;
	text.Margin.Right = 20;
	//text.Style.Border.Radius = 20;

	text.SetText(L"啊撒旦艰苦换个房间看电视进\n口的方电视进\n口的方电视进\n口的方电视进\n口的方电视进\n口的方电视进\n口的方电视进\n口的方电视进\n口的方电视进\n口的方式但是几乎都是复活节过段时间韩国");
	//text.HoverStyle.FontSize = 50;
	//text.ActiveStyle.FontSize = 50;
	text.GetScrollBar()->SetFixedWidth(20);

	Button btn;
	btn.SetParent(&mainLayout);
	btn.SetText(L"单行/多行切换");
	btn.SetFixedSize({ 150,40 });
	btn.EventHandler = [&text](Control* sd, const EventArgs& arg)->void {
		if (arg.EventType == Event::OnMouseDown) {
			text.SetMultiLine(!text.IsMultiLine());
			text.Invalidate();
		}
		};


	/*for (size_t i = 0; i < 99999; i++)
	{
		Button* lb = new Button(&list);
		lb->SetText("button" + std::to_string(i));
		lb->SetFixedSize({ 50,30 });
	}*/

	//mainLayout.AddControl(&labelBottom);//添加控件

	frm.SetLayout(&mainLayout);//给窗口设置布局

	frm.Show();//显示窗口
	return app.Exec();//进行消息循环

}