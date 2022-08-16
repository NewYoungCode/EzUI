#include "ezui.h"
#include "VLayout.h"
#include "Form.h" 
#include "Button.h"
#include "UIManager.h"
#include "TileLayout.h"
class GameItem :public VBox {
public:
	Label ctlImg, ctlName;
	GameItem(Image* img, const EString& game) {
		img->Box = Rect(13, 8, 40, 40);
		this->SetFixedHeight(67);
		this->SetFixedWidth(67);
		ctlName.SetFixedHeight(20);
		ctlName.SetText(utf8("сно╥1"));
		ctlImg.Style.ForeImage = img;
		AddControl(&ctlImg);
		AddControl(&ctlName);
		ctlImg.MousePassThrough = ctlName.MousePassThrough= Event::OnMouseEnter | Event::OnMouseLeave;
		HoverStyle.SetBorder(Color::HotPink, 1);
	}
};
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Application app;
	LayeredWindow form(1022, 670);
	VBox box;
	form.SetLayout(&box);

	TileLayout list;
	box.AddControl(&list);
	box.Style.BackgroundColor = Color::Pink;
	form.Zoom = true;
	Image img = EString(L"Images/icon1.png");
	for (size_t i = 0; i < 50;i++)
	{
		GameItem* it = new GameItem(&img, "");
		list.AddControl(it);
	}
	form.Show();
	return	app.exec();
}


