//WIN32 desktop application UI framework

//email:718987717@qq.com

#include "EzUI.h"

#include "Application.h"

#include "Form.h"

#include "Button.h"

#include "TileLayout.h"

#include "VList.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
	
{

	Application app;

	FrameWindow frm(800, 600);
	Layout layout;
	layout.Style.BackgroundColor = Color::White;
	TileLayout tile;
	tile.Dock = DockStyle::Fill;
	tile.MarginRight = 2;
	tile.MarginTop = 2;

	for (size_t i = 0; i < 1000000; i++)
	{
		Button* btn = new Button;
		btn->HoverStyle.BackgroundColor = Color::Red;
		btn->SetText("btn" + std::to_string(i));
		btn->ReSize({ 100, 30 });
		tile.AddControl(btn);
	}

	layout.AddControl(&tile);
	frm.SetLayout(&layout);
	frm.Show();

	return app.exec();
}
