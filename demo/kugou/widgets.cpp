#include "widgets.h"

LocalItem::LocalItem(const UIString& _songName, const UIString& _songTime) {

	del.Style.ForeImage = Image::Make(L"res/imgs/del.png");
	del.SetFixedSize({ 20,20 });
	del.Style.Cursor = LoadCursor(Cursor::HAND);
	del.Name = "dellocal";
	del.EventPassThrough = Event::OnHover;

	this->Name = "songItem";

	songName.Name = "autosz";
	songName.SetText(_songName);
	songName.Ellipsis = "...";//文本超出容器之外采用省略号
	songName.TextAlign = TextAlign::MiddleLeft;
	songName.SetTips("child_" + _songName);
	//穿透事件
	songName.EventPassThrough = time.EventPassThrough = Event::OnHover | Event::OnActive | Event::OnMouseDoubleClick;
	songName.HoverStyle.FontSize = 15;

	time.HoverStyle.Angle = 180;
	time.SetFixedWidth(50);
	time.SetText(_songTime);
	time.TextAlign = TextAlign::MiddleRight;

	this->SetFixedHeight(33);
	HoverStyle.BackColor = Color(230, 230, 230, 100);
	ActiveStyle.BackColor = Color(255, 230, 230, 100);
	ActiveStyle.FontSize = 10;

	Add(new HSpacer(15));
	Add(&songName);
	Add(&time);
	Add(new HSpacer(15));
	Add(&del);
	Add(new HSpacer(10));
}
LocalItem::~LocalItem() {
	if (del.Style.ForeImage) {
		delete del.Style.ForeImage;
	}
}

SearchItem::~SearchItem() {
	if (mv.Style.ForeImage) {
		delete mv.Style.ForeImage;
	}
	if (del.Style.BackImage) {
		delete del.Style.BackImage;
	}
}
SearchItem::SearchItem(const Song& s) {
	song = s;
	this->Tag = (int_t)&song;
	SetAttribute("FileHash", s.hash);
	SetAttribute("SingerName", s.SingerName);

	SetTips(s.SongName);
	SetFixedHeight(35);
	Style.Border.Bottom = 1;
	Style.Border.Color = Color(245, 245, 245);
	HoverStyle.BackColor = Color(245, 245, 245);

	songName.Ellipsis = L"...";
	songName.SetText(s.SongName);
	songName.TextAlign = TextAlign::MiddleLeft;
	songName.EventPassThrough = Event::OnHover | Event::OnMouseDoubleClick;
	songName.HoverStyle.ForeColor = Color(200, 100, 1);
	AlbumName.SetFixedWidth(180);
	AlbumName.SetText(s.AlbumName);
	AlbumName.TextAlign = TextAlign::MiddleLeft;
	AlbumName.Style.Cursor = LoadCursor(Cursor::HAND);
	AlbumName.Style.ForeColor = Color(150, 150, 150);
	AlbumName.EventPassThrough = Event::OnHover;

	mv.SetFixedWidth(35);
	mv.EventPassThrough = Event::OnHover;
	if (!s.MvHash.empty()) {
		mv.SetAttribute("mvhash", s.MvHash);
		mv.Style.ForeImage = Image::Make(L"res/imgs/mvicon.png");;
		mv.Margin = 8;
		mv.Style.Cursor = LoadCursor(Cursor::HAND);
	}
	else {
		mv.EventPassThrough = Event::OnHover | Event::OnMouseDoubleClick;
	}

	time.SetFixedWidth(60);
	time.SetText(global::toTimeStr(s.Duration));
	time.TextAlign = TextAlign::MiddleLeft;
	time.EventPassThrough = Event::OnHover | Event::OnMouseDoubleClick;
	time.Style.ForeColor = Color(150, 150, 150);

	del.SetFixedWidth(33);

	UIString fileName = "res/imgs/" + std::to_string(s.QualityLevel) + ".png";
	Image* img = Image::Make(fileName.unicode());
	del.Style.BackImage = img;
	del.Margin = 8;

	Add(new HSpacer(15));
	Add(&songName);
	Add(&AlbumName);
	Add(&mv);
	Add(&del);
	Add(new HSpacer(5));
	Add(&time);
	Add(new HSpacer(5));
}

LoginFrm::LoginFrm(HWND owner) :LayeredWindow(300, 200,  owner)
{
	umg.LoadXmlFile("res/xml/login.htm");
	umg.SetupUI(this);
	this->Border.Radius = 10;
	this->Border = 1;
	this->Border.Color = Color::Gray;
}
