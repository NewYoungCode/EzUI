//WIN32 desktop application UI framework (2d graphical library:direct2d,���ڿ��ܻ����������ƽ̨��2dͼ�ο������UI��ܽ��п�ƽ̨)

//email:718987717@qq.com //qqȺ: 758485934

#include <Windows.h>

#include "EzUI.h"
#include "BorderlessWindow.h"
#include "Button.h"
#include "TileList.h"
#include "VList.h"
#include "HList.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Application.h"
#include  "TextBox.h"

using namespace EzUI;

class MainFrm :public Window {
public:
	MainFrm(int cx, int cy) :Window(cx, cy) {}
	void OnClose(bool& b) {
		if (this->GetText()=="modal") {
			//Application::Exit(0);

		}
		else {
			Application::Exit(0);
		}
	}
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	Application app;//��Ϣѭ������

	//{
	//	MainFrm frm(800, 600);//�ޱ߿򴰿�

	//	HLayout mainLayout;
	//	frm.SetLayout(&mainLayout);
	//	mainLayout.Style.BackgroundColor = Color::White;

	//	Label lb1(&mainLayout);
	//	Label lb2(&mainLayout);

	//	Label lb3(&mainLayout);


	//	lb1.SetMiniWidth(200);
	//	lb1.SetMaxWidth(500);

	//	lb3.SetMaxWidth(400);

	//	lb1.SetText(L"����label1");
	//	lb2.SetText(L"����label2");
	//	lb3.SetText(L"����label333");


	//	frm.Show();

	//	return app.exec();
	//}

	//keybd_event(VK_LWIN, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	MainFrm frm(800, 600);//�ޱ߿򴰿�
	VLayout mainLayout;//�����е�main����

	mainLayout.Style.BackColor = Color::White;//�����ֱ�����ɫ
	//HLayout title(&mainLayout);//����
	//title.SetFixedHeight(45);//���ù̶��߶�
	//title.Style.BackgroundColor = Color::Pink;//�ؼ�������ɫ
	//title.Action = ControlAction::MoveWindow;//���ƶ����ڵ���Ϊ

	//Label text(&title);//��������
	//text.Action = ControlAction::MoveWindow;//���ƶ����ڵ���Ϊ
	//text.SetText(L"hello world ");//��label��������

	Label closeBtn;//�رհ�ť

	closeBtn.Action = ControlAction::Close;//�رմ��ڵ���Ϊ

	closeBtn.Style.FontFamily = L"Marlett";//���ó�icon����

	closeBtn.Style.FontSize = 13;//�����С

	closeBtn.HoverStyle.ForeColor = Color::White;//α��ʽ �������������ɫ��ɰ�ɫ

	closeBtn.SetFixedWidth(45);//���þ��Ը߶�

	closeBtn.SetText(L"r");//���ڵĹرհ�ťicon

	//Label labelBottom;
	//labelBottom.SetText(L"����һ���򵥵Ĵ���ʾ��!");
	HList list;
	list.SetParent(&mainLayout);
	//list.SetAutoHeight(true);
	for (size_t i = 0; i < 10; i++)
	{
		Label* lb = new Label;// (&list);
		//lb->Dock = DockStyle::Horizontal;
		lb->SetText(std::to_string(i));
		lb->Name = std::to_string(i);
		lb->SetFixedSize({ 100,30 });
		lb->Style.BackColor = Color::Green;
		lb->HoverStyle.BackColor = Color::Gray;
		lb->Margin = 1;
		lb->EventHandler = [&list](Control* sd, const EventArgs& arg)->void {
			if (arg.EventType == Event::OnMouseClick) {
				/*list.Remove(sd);
				list.Invalidate();*/
				MainFrm frm(500, 300);//�ޱ߿򴰿�
				frm.SetText("modal");
				VLayout mainLayout;//�����е�main����
				mainLayout.Style.BackColor = Color::White;//�����ֱ�����ɫ
				frm.SetLayout(&mainLayout);
				frm.ShowModal();
			}
			};
		list.Add(lb);
	}

	TextBox text;
	text.SetParent(&mainLayout);
	//text.SetFixedSize({ 100,50 });
	text.Style.Border.Color = Color::Gray;
	text.Style.Border = 1;
	text.SetMultiLine(true);
	text.Margin.Left = 20;
	text.Margin.Right = 20;
	//text.Style.Border.Radius = 20;

	text.SetText(L"��������໻�����俴���ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ����ӽ�\n�ڵķ�ʽ���Ǽ������Ǹ���ڹ���ʱ�享��");
	//text.HoverStyle.FontSize = 50;
	//text.ActiveStyle.FontSize = 50;
	text.GetScrollBar()->SetFixedWidth(20);

	Button btn;
	btn.SetParent(&mainLayout);
	btn.SetText(L"����/�����л�");
	btn.SetFixedSize({ 150,40 });
	btn.EventHandler = [&text](Control* sd, const EventArgs& arg)->void {
		if (arg.EventType == Event::OnMouseClick) {
			/*text.SetMultiLine(!text.IsMultiLine());
			text.Invalidate();*/
		}
		};


	/*for (size_t i = 0; i < 99999; i++)
	{
		Button* lb = new Button(&list);
		lb->SetText("button" + std::to_string(i));
		lb->SetFixedSize({ 50,30 });
	}*/

	//mainLayout.AddControl(&labelBottom);//��ӿؼ�

	frm.SetLayout(&mainLayout);//���������ò���

	frm.Show();//��ʾ����
	return app.Exec();//������Ϣѭ��

}