//WIN32 desktop application UI framework

//email:718987717@qq.com //qqȺ: 758485934

#include "EzUI.h"

#include "Application.h"

#include "Form.h"

#include "Button.h"

#include "TileList.h"

#include "VList.h"
#include "HLayout.h"

#include <Windows.h>
using namespace EzUI;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	Application app;

	EzUI::Window frm(800, 600);
	HLayout layout;
	layout.Style.BackgroundColor = Color::White;

	Control c;
	c.SetRect({ 100,-100,400,200 });
	c.Style.BackgroundColor = Color(100, 200, 0, 255);

	Label c1;
	c1.Style.Radius = 200;

	c1.SetText("hello世界!撒大苏打撒旦sadasdasdas撒大苏打呱呱呱呱呱呱韩国飒飒撒旦哈哈哈哈哈哈66");
	c1.SetRect({ 100,200,400,200 });
	c1.Style.BackgroundColor = Color(255, 200, 250, 0);

	c1.AddControl(&c);

	layout.AddControl(&c1);


	//TileList tile;
	//tile.Dock = DockStyle::Fill;
	//tile.MarginRight = 1;
	//tile.MarginTop = 1;

	//for (size_t i = 0; i < 999; i++)
	//{
	//	Button* btn = new Button;
	//	btn->ShadowWidth = 5;
	//	btn->SetSize({ 100,20 });
	//	btn->SetText("btn98789789789" + std::to_string(i));
	//	btn->SetLocation({ 100, 30 });
	//	tile.AddControl(btn);
	//}

	////tile.ResumeLayout();
	//layout.AddControl(&tile);
	frm.SetLayout(&layout);
	frm.Show();

	return app.exec();

}