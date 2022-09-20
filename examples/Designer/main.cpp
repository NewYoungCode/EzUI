//WIN32 desktop application UI framework

//email:718987717@qq.com //qqȺ: 758485934

#include "EzUI.h"

#include "Application.h"

#include "Form.h"

#include "Button.h"

#include "TileList.h"

#include "VList.h"

#include <Windows.h>
using namespace EzUI;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{

	Application app;

	EzUI::Window frm(800, 600);
	Layout layout;
	layout.Style.BackgroundColor = Color::White;
	TileList tile;
	tile.Dock = DockStyle::Fill;
	tile.MarginRight = 1;
	tile.MarginTop = 1;

	for (size_t i = 0; i < 999; i++)
	{
		Button* btn = new Button;
		btn->ShadowWidth = 5;
		btn->SetSize({ 100,20 });
		btn->SetText("btn98789789789" + std::to_string(i));
		btn->SetLocation({ 100, 30 });
		tile.AddControl(btn);
	}

	//tile.ResumeLayout();
	layout.AddControl(&tile);
	frm.SetLayout(&layout);
	frm.Show();

	return app.exec();

}