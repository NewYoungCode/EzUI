//WIN32 desktop application UI framework

//email:718987717@qq.com //qqȺ: 758485934

#include "EzUI.h"

#include "Application.h"

#include "Form.h"

#include "Button.h"

#include "TileList.h"

#include "VList.h"
#include "HLayout.h"
#include "vLayout.h"

#include "HList.h"
#include "ComBox.h"
#include "PictureBox.h"

#include <Windows.h>
using namespace EzUI;

class GameItem :public VLayout {
public:
	Label img;
	Label name;
	GameItem(const EString& txt) {
		Name = txt;
		SetSize({ 100,130 });
		Style.Radius = 20;
		Style.BackgroundColor = Color(200, 0, 200);
		name.SetFixedHeight(30);
		name.SetText(txt);
		img.Style.BackgroundImage = new Image(L"images\\a.png");
		AddControl(&img);
		AddControl(&name);
	}
};
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//Application app;
	//EzUI::Window frm(800, 600);
	//frm.SetText("内矩形抗锯齿");
	//HLayout layout;
	///*Control c;
	//c.SetRect({ 100,-100,400,200 });
	//c.Style.BackgroundColor = Color(100, 200, 0, 255);
	//Edit c1;
	//c1.Style.FontSize = 15;
	//c1.Style.Radius = 200;
	//c1.Style.SetBorder(Color::Black, 2);
	//c1.SetText("hello世界!撒大苏打撒旦sadasdasdas撒大苏打呱呱呱呱呱呱韩国飒飒撒旦哈哈哈哈哈哈66");
	//c1.SetRect({ 100,200,400,200 });
	//c1.Style.BackgroundColor = Color(255, 200, 250, 0);
	//c1.AddControl(&c);
	//layout.AddControl(&c1);*/
	//TileList tile;
	//tile.Dock = DockStyle::Fill;
	//tile.Style.BackgroundColor = Color::White;// Color(25, 32, 47);
	//tile.MarginRight = 20;
	//tile.MarginTop = 20;
	//tile.Style.ForeColor = Color::White;
	//for (size_t i = 0; i < 999; i++)
	//{
	//	/*Button* btn = new Button;
	//	btn->ShadowWidth = 5;
	//	btn->SetSize({ 100,20 });
	//	btn->SetText("btn98789789789" + std::to_string(i));
	//	btn->SetLocation({ 100, 30 });
	//	tile.AddControl(btn);*/
	//	GameItem* it = new GameItem;
	//	it->img.Style.BackgroundImage = new Image(L"images\\a.png");
	//	tile.AddControl(it);
	//}
	////tile.ResumeLayout();
	//layout.AddControl(&tile);
	//frm.SetLayout(&layout);
	//frm.Show();
	//return app.exec();

	Application app;

	//EzUI::BorderlessWindow frm(800, 600);

	//ui::TileList layout;
	//layout.Style.BackgroundColor = Color::White;

	//for (size_t i = 0; i < 1000; i++)
	//{
	//	GameItem* btn1 = new GameItem("Steam PUBG  " + std::to_string(i));

	//	btn1->Margin.Left = 10;
	//	btn1->Margin.Top = 10;

	//	//btn1->SetText(std::to_string(i) + "button");
	//	btn1->Style.BackgroundColor = Color::Gray;
	//	//btn1->SetFixedWidth(35);
	//	//btn1->Dock = DockStyle::Horizontal;
	//	layout.AddControl(btn1);

	//	btn1->MouseClick = [&](Control* ct, MouseButton, const Point&) {
	//		layout.RemoveControl(ct);
	//		layout.Invalidate();
	//	};
	//}


	//frm.SetLayout(&layout);
	//frm.Show();


	Window wd(800, 600);

	PictureBox l;
	l.Style.BackgroundColor = Color::Pink;

	Image testGif(L"D:\\test.gif");

	//l.HoverStyle.ForeImage = &testGif;

	l.SetImage(&testGif);


	ComBox cbox;
	
	for (size_t i = 0; i < 6; i++)
	{
		cbox.AddItem("选中"+std::to_string(i) );
	}

	cbox.SetRect({100,100,180,30});
	l.AddControl(&cbox);

	wd.SetLayout(&l);

	wd.Show();

	return app.exec();

}