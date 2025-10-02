#include "widgets.h"

LocalItem::LocalItem(const UIString& _songName, const UIString& _songTime) {

	del.Style.ForeImage = Image::Make(L"res/imgs/del.png");
	del.SetFixedSize({ 20,20 });
	del.Style.Cursor = LoadCursor(Cursor::HAND);
	del.Name = "dellocal";
	del.SetHitTestVisible(false);

	this->Name = "songItem";

	this->Style.Cursor = LoadCursor(Cursor::HAND);
	this->ActiveStyle.Cursor = LoadCursor(Cursor::HELP);

	songName.Name = "autosz";
	songName.SetText(_songName);
	songName.SetElidedText("...");//文本超出容器之外采用省略号
	songName.TextAlign = TextAlign::MiddleLeft;
	songName.SetTips("child_" + _songName);
	//穿透事件
	songName.SetHitTestVisible(false);
	songName.HoverStyle.FontSize = 15;

	time.HoverStyle.Angle = 180;
	time.SetFixedWidth(50);
	time.SetText(_songTime);
	time.TextAlign = TextAlign::MiddleRight;

	this->SetFixedHeight(33);
	HoverStyle.BackColor = Color(230, 230, 230, 100);
	ActiveStyle.BackColor = Color(255, 230, 230, 100);
	ActiveStyle.FontSize = 10;

	AddChild(new HSpacer(15));
	AddChild(&songName);
	AddChild(&time);
	AddChild(new HSpacer(15));
	AddChild(&del);
	AddChild(new HSpacer(10));

	this->EventHandler = [this](Control* sd, EventArgs& args) {
		if (args.EventType == Event::OnMouseMove) {
			MouseEventArgs arg = (MouseEventArgs&)args;
			if (del.IsVisible() && del.GetRect().Contains(arg.Location)) {
				auto hCursor = del.GetCursor();
				if (hCursor) {
					::SetCursor(hCursor);
				}
			}
		}
		if (args.EventType == Event::OnMouseDown) {
			MouseEventArgs arg = (MouseEventArgs&)args;
			if (del.IsVisible() && del.GetRect().Contains(arg.Location)) {
				del.SendEvent(args);
			}
		}
		};

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
	this->SetUserData(&song);
	SetAttribute("FileHash", s.hash);
	SetAttribute("SingerName", s.SingerName);

	SetTips(s.SongName);
	SetFixedHeight(35);
	Style.Border.Bottom = 1;
	Style.Border.Color = Color(245, 245, 245);
	HoverStyle.BackColor = Color(245, 245, 245);

	songName.SetElidedText("...");
	songName.SetText(s.SongName);
	songName.TextAlign = TextAlign::MiddleLeft;
	songName.SetHitTestVisible(false);
	songName.HoverStyle.ForeColor = Color(200, 100, 1);
	AlbumName.SetFixedWidth(180);
	AlbumName.SetText(s.AlbumName);
	AlbumName.TextAlign = TextAlign::MiddleLeft;
	AlbumName.Style.Cursor = LoadCursor(Cursor::HAND);
	AlbumName.Style.ForeColor = Color(150, 150, 150);
	AlbumName.SetHitTestVisible(false);

	mv.SetFixedWidth(35);
	mv.SetHitTestVisible(false);
	if (!s.MvHash.empty()) {
		mv.SetAttribute("mvhash", s.MvHash);
		mv.Style.ForeImage = Image::Make(L"res/imgs/mvicon.png");;
		mv.SetMargin(8);
		mv.Style.Cursor = LoadCursor(Cursor::HAND);
	}
	else {
		mv.SetHitTestVisible(false);
	}

	time.SetFixedWidth(60);
	time.SetText(global::toTimeStr(s.Duration));
	time.TextAlign = TextAlign::MiddleLeft;
	time.SetHitTestVisible(false);
	time.Style.ForeColor = Color(150, 150, 150);

	del.SetFixedWidth(33);

	UIString fileName = "res/imgs/" + std::to_string(s.QualityLevel) + ".png";
	Image* img = Image::Make(fileName.unicode());
	del.Style.BackImage = img;
	del.SetMargin(8);

	AddChild(new HSpacer(15));
	AddChild(&songName);
	AddChild(&AlbumName);
	AddChild(&mv);
	AddChild(&del);
	AddChild(new HSpacer(5));
	AddChild(&time);
	AddChild(new HSpacer(5));

	this->EventHandler = [this](Control* sd, EventArgs& args) {
		if (args.EventType == Event::OnMouseMove) {
			MouseEventArgs arg = (MouseEventArgs&)args;
			if (mv.IsVisible() && mv.GetRect().Contains(arg.Location)) {
				auto hCursor = mv.GetCursor();
				if (hCursor) {
					::SetCursor(hCursor);
				}
			}
		}
		if (args.EventType == Event::OnMouseDown) {
			MouseEventArgs arg = (MouseEventArgs&)args;
			if (mv.IsVisible() && mv.GetRect().Contains(arg.Location)) {
				mv.SendEvent(args);
			}
		}
		};
	this->SetRateWidth(1.0);
}

void LoginFrm::OnNotify(Control* sender, EventArgs& args)
{
	do
	{
		if (args.EventType == Event::OnMouseDown) {
			if (sender->Name == "btnLogin") {
				TextBox* editUser = (TextBox*)this->FindControl("username");
				TextBox* editPwd = (TextBox*)this->FindControl("password");
				if (editUser->GetText() == "admin" && editPwd->GetText() == "123456") {
					this->m_userName = editUser->GetText();
					::MessageBoxW(Hwnd(), L"登录成功!", L"成功", MB_OK);
					this->Close(1);
				}
				else
				{
					::MessageBoxW(Hwnd(), L"用户名或密码错误!", L"失败", MB_OK);
				}
				break;
			}
		}
	} while (false);
	ezui::DefaultNotify(sender, args);
}

LoginFrm::LoginFrm(HWND owner) :LayeredWindow(300, 200, owner)
{
	umg.LoadXml("res/xml/login.htm");
	umg.SetupUI(this);
}
