//WIN32 desktop application UI framework

//email:718987717@qq.com //qqȺ: 758485934

#include "EzUI.h"
#include "Application.h"
#include "Form.h"
#include "Button.h"
#include "VLayout.h"
#include "HLayout.h"

using namespace EzUI;

int APIENTRY wWinMain2( HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine, int nCmdShow)
{

	Application app;//��Ϣѭ������
	BorderlessWindow frm(800, 600);//�ޱ߿򴰿�

	VLayout mainLayout;//��Ҫ����
	mainLayout.Style.BackgroundColor = Color::White;//�����ֱ�����ɫ

	HLayout title;//����
	title.SetFixedHeight(45);//���ù̶��߶�
	title.Style.BackgroundColor = Color::Pink;//�ؼ�������ɫ
	title.Action = ControlAction::MoveWindow;//���ƶ����ڵ���Ϊ

	Label text;//��������
	text.Action = ControlAction::MoveWindow;//���ƶ����ڵ���Ϊ
	text.SetText(L"hello world ");
	title.AddControl(&text);

	Label closeBtn;//�رհ�ť
	closeBtn.Action = ControlAction::Close;
	closeBtn.Style.FontFamily = L"Marlett";
	closeBtn.Style.FontSize = 13;
	closeBtn.HoverStyle.ForeColor = Color::White;//α��ʽ �������������ɫ��ɰ�ɫ
	closeBtn.SetFixedWidth(45);
	closeBtn.SetText(L"r");
	title.AddControl(&closeBtn);

	Label labelBottom;
	labelBottom.SetText(L"����һ���򵥵Ĵ���ʾ�� !");

	mainLayout.AddControl(&title);//��ӱ���
	mainLayout.AddControl(&labelBottom);//

	frm.SetLayout(&mainLayout);//���������ò���
	frm.Show();//��ʾ����

	return app.exec();//������Ϣѭ��
}