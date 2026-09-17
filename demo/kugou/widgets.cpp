#include "widgets.h"

#include <algorithm>

namespace {

float Clamp01(float value)
{
	if (value < 0.0f) {
		return 0.0f;
	}
	if (value > 1.0f) {
		return 1.0f;
	}
	return value;
}

bool HasVisualState(VisualState current, VisualState target)
{
	return (static_cast<int16_t>(current) & static_cast<int16_t>(target)) != 0;
}

float Positive(float value)
{
	return (std::max)(0.0f, value);
}

float PlayerThickness(float controlWidth, float controlHeight)
{
	return (std::min)(Positive(controlWidth), Positive(controlHeight));
}

float PlayerTrackHeight(float controlWidth, float controlHeight)
{
	return PlayerThickness(controlWidth, controlHeight) * 0.28f;
}

float PlayerMaxThumbDiameter(float controlWidth, float controlHeight)
{
	return PlayerThickness(controlWidth, controlHeight) * 0.72f;
}

float PlayerThumbDiameter(float controlWidth, float controlHeight, bool hover, bool active)
{
	const float thickness = PlayerThickness(controlWidth, controlHeight);
	float ratio = 0.56f;
	if (active) {
		ratio = 0.72f;
	}
	else if (hover) {
		ratio = 0.64f;
	}
	return thickness * ratio;
}

float PlayerThumbGlowRadius(float controlWidth, float controlHeight, float thumbDiameter, bool active)
{
	const float thickness = PlayerThickness(controlWidth, controlHeight);
	const float glow = thickness * (active ? 0.14f : 0.11f);
	const float maxGlow = Positive((controlHeight - thumbDiameter) * 0.5f);
	return (std::min)(glow, maxGlow);
}

} // namespace

LocalItem::LocalItem(const UIString& _songName, const UIString& _songTime) {

	del.Style->BackImage = Image::Make(L"res/imgs/del.png");
	del.SetFixedSize({ 20,20 });
	del.Style->Cursor = LoadCursor(Cursor::Pointer);
	del.SetName("dellocal");
	del.SetHitTestVisible(false);

	this->SetStyle("cursor:pointer;", VisualState::Normal);
	this->SetStyle("cursor:help;", VisualState::Active);

	songName.SetName("autosz");// = "autosz";
	songName.SetText(_songName);
	songName.SetElidedText("...");//文本超出容器之外采用省略号
	songName.SetTextAlign(TextAlign::MiddleLeft);
	songName.SetToolTip("child_" + _songName);
	//穿透事件
	songName.SetHitTestVisible(false);
	songName.SetStyle("font-size:15px;", VisualState::Hover);

	time.HoverStyle->Angle = 180;
	time.SetFixedWidth(50);
	time.SetText(_songTime);
	time.SetTextAlign(TextAlign::MiddleRight);

	this->SetFixedHeight(33);
	SetStyle("background-color:rgba(230,230,230,39%);", VisualState::Hover);
	SetStyle("background-color:rgba(255,230,230,39%);font-size:10px;", VisualState::Active);

	AddChild(new HSpacer(15));
	AddChild(&songName);
	AddChild(&time);
	AddChild(new HSpacer(15));
	AddChild(&del);
	AddChild(new HSpacer(10));

	//绑定事件
	this->AddEventHandler(Event::MouseMove | Event::MouseDown, [this](Control* sd, EventArgs* args) {
		if (args->EventType() == Event::MouseMove) {
			MouseEventArgs* arg = (MouseEventArgs*)args;
			if (del.IsVisible() && del.GetRect().Contains(arg->Position())) {
				auto hCursor = del.GetCursor();
				if (hCursor) {
					::SetCursor(hCursor);
				}
			}
		}
		if (args->EventType() == Event::MouseDown && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
			MouseEventArgs* arg = (MouseEventArgs*)args;
			if (del.IsVisible() && del.GetRect().Contains(arg->Position())) {
				del.SendEvent(args);
			}
		}
		});

}
LocalItem::~LocalItem() {
	if (del.Style->BackImage) {
		delete del.Style->BackImage;
	}
}

SearchStatusItem::SearchStatusItem(const UIString& text, bool highlight)
	: message(text), highlighted(highlight)
{
	SetText(message);
	SetStyle(highlighted ? "height:35px;background-color:#FEF9E5;" : "height:35px;", VisualState::Normal);
	SetHitTestVisible(false);
}

SearchItem::~SearchItem() {
	if (mv.Style->BackImage) {
		delete mv.Style->BackImage;
	}
	if (del.Style->BackImage) {
		delete del.Style->BackImage;
	}
}
SearchItem::SearchItem(const Song& s) {
	song = s;
	this->SetUserData(&song);
	SetAttribute("FileHash", s.hash);
	SetAttribute("SingerName", s.SingerName);

	SetToolTip(s.SongName);
	SetFixedHeight(35);
	SetStyle("border-bottom:1px solid #F5F5F5;", VisualState::Normal);
	SetStyle("background-color:#F5F5F5;", VisualState::Hover);

	songName.SetElidedText("...");
	songName.SetText(s.SongName);
	songName.SetTextAlign(TextAlign::MiddleLeft);
	songName.SetHitTestVisible(false);
	songName.SetStyle("color:#C86401;", VisualState::Hover);
	AlbumName.SetFixedWidth(180);
	AlbumName.SetText(s.AlbumName);
	AlbumName.SetTextAlign(TextAlign::MiddleLeft);
	AlbumName.SetStyle("cursor:pointer;color:#969696;", VisualState::Normal);
	AlbumName.SetHitTestVisible(false);

	mv.SetFixedWidth(35);
	mv.SetHitTestVisible(false);
	if (!s.MvHash.empty()) {
		mv.SetAttribute("mvhash", s.MvHash);
		mv.Style->BackImage = Image::Make(L"res/imgs/mvicon.png");;
		mv.SetMargin(8);
		mv.SetStyle("cursor:pointer;", VisualState::Normal);
	}
	else {
		mv.SetHitTestVisible(false);
	}

	time.SetFixedWidth(60);
	time.SetText(global::toTimeStr(s.Duration));
	time.SetTextAlign(TextAlign::MiddleLeft);
	time.SetHitTestVisible(false);
	time.SetStyle("color:#969696;", VisualState::Normal);

	del.SetFixedWidth(33);

	UIString fileName = "res/imgs/" + std::to_string(s.QualityLevel) + ".png";
	Image* img = Image::Make(fileName.unicode());
	del.Style->BackImage = img;
	del.SetMargin(8);

	AddChild(new HSpacer(15));
	AddChild(&songName);
	AddChild(&AlbumName);
	AddChild(&mv);
	AddChild(&del);
	AddChild(new HSpacer(5));
	AddChild(&time);
	AddChild(new HSpacer(5));

	auto id = this->AddEventHandler(Event::MouseMove | Event::MouseDown, [this](Control* sd, EventArgs* args) {
		if (args->EventType() == Event::MouseMove) {
			MouseEventArgs* arg = (MouseEventArgs*)args;
			if (mv.IsVisible() && mv.GetRect().Contains(arg->Position())) {
				auto hCursor = mv.GetCursor();
				if (hCursor) {
					::SetCursor(hCursor);
				}
			}
		}
		if (args->EventType() == Event::MouseDown && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
			MouseEventArgs* arg = (MouseEventArgs*)args;
			if (mv.IsVisible() && mv.GetRect().Contains(arg->Position())) {
				mv.SendEvent(args);
			}
		}
		});
}

PlayerProgressBar::PlayerProgressBar(Object* ownerObj) : Slider(ownerObj)
{
	Style->BackColor = Color(255, 255, 255, 56);
	Style->ForeColor = Color(42, 176, 255);
	Style->Cursor = LoadCursor(Cursor::Pointer);
}

void PlayerProgressBar::OnTrackPaint(PaintEventArgs* args)
{
	RectF trackRect = GetTrackRect();
	const float radius = trackRect.Height * 0.5f;
	const float ratio = Clamp01(GetRatio());
	Graphics* graphics = args->Graphics();

	graphics->SetColor(Color(255, 255, 255, 54));
	graphics->FillRectangle(trackRect, radius);

	if (ratio <= 0.0f || trackRect.Width <= 0.0f) {
		return;
	}

	RectF activeRect = trackRect;
	activeRect.Width = (std::min)(trackRect.Width, (std::max)(trackRect.Height, trackRect.Width * ratio));

	RectF glowRect = activeRect;
	float trackGlow = PlayerThickness((float)Width(), (float)Height()) * 0.11f;
	trackGlow = (std::min)(trackGlow, trackRect.Y);
	trackGlow = (std::min)(trackGlow, (float)Height() - trackRect.GetBottom());
	trackGlow = (std::max)(0.0f, trackGlow);
	glowRect.Y -= trackGlow;
	glowRect.Height += trackGlow * 2.0f;
	graphics->SetColor(Color(36, 190, 255, 46));
	graphics->FillRectangle(glowRect, glowRect.Height * 0.5f);

	Gradient fillGradient = Gradient::Linear(Vec2(activeRect.X, activeRect.Y), Vec2(activeRect.GetRight(), activeRect.Y))
		.AddStop(0.0f, Color(111, 230, 255))
		.AddStop(0.52f, Color(34, 170, 255))
		.AddStop(1.0f, Color(0, 116, 238));
	graphics->FillRectangle(activeRect, fillGradient, radius);

	RectF highlightRect = activeRect;
	highlightRect.Y += trackRect.Height * 0.18f;
	highlightRect.Height = trackRect.Height * 0.24f;
	graphics->SetColor(Color(255, 255, 255, 72));
	graphics->FillRectangle(highlightRect, highlightRect.Height * 0.5f);
}

void PlayerProgressBar::OnThumbPaint(PaintEventArgs* args)
{
	const float thickness = PlayerThickness((float)Width(), (float)Height());
	const bool active = PressedButton() == MouseButton::Left;
	const bool hover = HasVisualState(GetVisualState(), VisualState::Hover);
	RectF thumbRect = GetThumbRect();
	Graphics* graphics = args->Graphics();

	if (hover || active) {
		RectF glowRect = thumbRect;
		const float glow = PlayerThumbGlowRadius((float)Width(), (float)Height(), thumbRect.Height, active);
		glowRect.X -= glow;
		glowRect.Y -= glow;
		glowRect.Width += glow * 2.0f;
		glowRect.Height += glow * 2.0f;
		graphics->SetColor(Color(62, 201, 255, active ? 82 : 58));
		graphics->FillEllipse(glowRect);
	}

	RectF shadowRect = thumbRect;
	float shadowOffset = thickness * 0.07f;
	shadowOffset = (std::min)(shadowOffset, (float)Height() - thumbRect.GetBottom());
	shadowOffset = (std::max)(0.0f, shadowOffset);
	shadowRect.Y += shadowOffset;
	graphics->SetColor(Color(0, 28, 64, active ? 92 : 62));
	graphics->FillEllipse(shadowRect);

	graphics->SetColor(Color(255, 255, 255));
	graphics->FillEllipse(thumbRect);
	graphics->SetColor(Color(138, 221, 255, hover || active ? 220 : 150));
	graphics->DrawEllipse(thumbRect, thickness * 0.06f);

	RectF dotRect = thumbRect;
	const float targetInset = thumbRect.Width * (active ? 0.33f : 0.31f);
	const float maxInset = (std::min)(thumbRect.Width, thumbRect.Height) * 0.38f;
	const float inset = (std::min)(targetInset, maxInset);
	dotRect.X += inset;
	dotRect.Y += inset;
	dotRect.Width -= inset * 2.0f;
	dotRect.Height -= inset * 2.0f;
	graphics->SetColor(Color(16, 153, 255));
	graphics->FillEllipse(dotRect);
}

SizeF PlayerProgressBar::GetThumbSize()
{
	const bool active = PressedButton() == MouseButton::Left;
	const bool hover = HasVisualState(GetVisualState(), VisualState::Hover);
	const float diameter = PlayerThumbDiameter((float)Width(), (float)Height(), hover, active);
	return SizeF(diameter, diameter);
}

RectF PlayerProgressBar::GetTrackRect()
{
	const float maxThumb = PlayerMaxThumbDiameter((float)Width(), (float)Height());
	const float maxGlow = PlayerThumbGlowRadius((float)Width(), (float)Height(), maxThumb, true);
	const float padding = maxThumb * 0.5f + maxGlow;
	const float trackHeight = PlayerTrackHeight((float)Width(), (float)Height());
	const float trackWidth = (std::max)(0.0f, Width() - padding * 2.0f);
	return RectF(padding, (Height() - trackHeight) * 0.5f, trackWidth, trackHeight);
}

RectF PlayerProgressBar::GetThumbRect()
{
	SizeF thumbSize = GetThumbSize();
	RectF trackRect = GetTrackRect();
	const float ratio = Clamp01(GetRatio());
	const float centerX = trackRect.X + trackRect.Width * ratio;
	return RectF(centerX - thumbSize.Width * 0.5f, (Height() - thumbSize.Height) * 0.5f, thumbSize.Width, thumbSize.Height);
}

void LoginFrm::OnNotify(Control* sender, EventArgs* args)
{
	do
	{
		if (args->EventType() == Event::MouseDown) {
			if (sender->GetName() == "btnLogin") {
				TextBox* editUser = (TextBox*)this->FindControl("username");
				TextBox* editPwd = (TextBox*)this->FindControl("password");
				if (editUser->GetText() == "admin" && editPwd->GetText() == "123456") {
					this->m_userName = editUser->GetText();
					::MessageBoxW(GetWindowHandle(), L"登录成功!", L"成功", MB_OK);
					this->Close(1);
				}
				else
				{
					::MessageBoxW(GetWindowHandle(), L"用户名或密码错误!", L"失败", MB_OK);
				}
				break;
			}
		}
	} while (false);
	__super::OnNotify(sender, args);
}

LoginFrm::LoginFrm(HWND owner) :BorderlessWindow(owner)
{
	this->LoadXml("res/xml/login.htm");
	this->EnableAlphaBlending();
}
