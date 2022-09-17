#include "IControl.h"
namespace EzUI {
	void ControlStyle::SetBorder(const Color& color, int width) { //¶ÔËùÓÐborderÓÐÐ§
		BorderColor = color;
		BorderLeft = width;//×ó±ß±ß¿ò
		BorderTop = width;//¶¥²¿±ß¿ò
		BorderRight = width;//ÓÒ±ß±ß¿ò
		BorderBottom = width;//µ×²¿±ß¿ò
	}
	bool ControlStyle::IsValid() {
		return  Radius.valid ||
			BorderLeft.valid || BorderTop.valid || BorderRight.valid || BorderBottom.valid || BorderColor.valid || BackgroundColor.valid ||
			BackgroundImage.valid || ForeImage.valid ||
			!FontFamily.empty() || FontSize.valid || ForeColor.valid;
	}
	void ControlStyle::SetStyleSheet(const EString& styleStr)
	{
		auto attrs = styleStr.Split(";");
		for (auto& it : attrs) {
			size_t pos = it.find(":");
			if (pos == -1)continue;
			EString key = it.substr(0, pos);
			EString value = it.substr(pos + 1);
			this->SetStyle(key, value);
		}
	}
	void ControlStyle::SetStyle(const EString& key, const EString& _value)
	{
		EString& value = (EString&)_value;
		ControlStyle* style = this;
		do
		{
			if (key == "background-color") {
				style->BackgroundColor = value;
				break;
			}
			if (key == "background-image") {
				value = value.Erase('"');//É¾³ýË«ÒýºÅ;
				style->BackgroundImage = new Image(value.utf16());
				break;
			}
			if (key == "fore-image") {
				value = value.Erase('"');//É¾³ýË«ÒýºÅ;
				style->ForeImage = new Image(value.utf16());
				break;
			}
			if (key == "border-color") {
				style->BorderColor = value;
				break;
			}
			if (key == "color" || key == "fore-color") {
				style->ForeColor = value;
				break;
			}
			if (key == "radius") {
				style->Radius = std::stoi(value);
				break;
			}
			if (key == "font-size") {
				style->FontSize = std::stoi(value);
				break;
			}
			if (key == "font-family") {
				value = value.Erase('"');//É¾³ýË«ÒýºÅ;
				style->FontFamily = value;
				break;
			}
			if (key == "border") {
				auto width = std::stoi(value);
				style->BorderLeft = width;
				style->BorderTop = width;
				style->BorderRight = width;
				style->BorderBottom = width;
				break;
			}
			if (key == "border-left") {
				style->BorderLeft = std::stoi(value);
				break;
			}
			if (key == "border-top") {
				style->BorderTop = std::stoi(value);
				break;
			}
			if (key == "border-right") {
				style->BorderRight = std::stoi(value);
				break;
			}
			if (key == "border-bottom") {
				style->BorderBottom = std::stoi(value);
				break;
			}
		} while (false);
	}


	const int& IRect::X()
	{
		return _rect.X;
	}
	const int& IRect::Y()
	{
		return _rect.Y;
	}
	const int& IRect::Width()
	{
		return _rect.Width;
	}
	const int& IRect::Height()
	{
		return _rect.Height;
	}
	void IRect::SetX(const int& X) {
		SetLocation({ X,Y() });
	}
	void IRect::SetY(const int& Y) {
		SetLocation({ X(),Y });
	}
	void IRect::SetLocation(const Point& pt) {
		SetRect(Rect(pt.X, pt.Y, Width(), Height()));
	}
	void IRect::SetWidth(const int& width) {
		SetSize({ width,Height() });
	}
	void IRect::SetHeight(const int& height) {
		SetSize({ Width(),height });
	}
	void IRect::SetSize(const Size& size)
	{
		SetRect({ X(),Y(),size.Width,size.Height });
	}
	const Rect& IRect::GetRect()
	{
		return _rect;
	}
	void IRect::OnLayout(const Size& pRect, bool instantly)
	{
		while (Dock != DockStyle::None)
		{
			if (Dock == DockStyle::Fill) {
				_rect = { 0,0,pRect.Width,pRect.Height };
				break;
			}
			if (Dock == DockStyle::Vertical) {
				_rect = { X(),0,Width(),pRect.Height };
				break;
			}
			if (Dock == DockStyle::Horizontal) {
				_rect = { 0,Y(),pRect.Width,Height() };
				break;
			}
			break;
		}
		if (instantly) {
			SetRect(_rect);
		}
	}
	void IRect::SetFixedWidth(const int& fixedWidth)
	{
		_rect.Width = fixedWidth;
		_fixedWidth = fixedWidth;
	}
	void IRect::SetFixedHeight(const int& fixedHeight)
	{
		_rect.Height = fixedHeight;
		_fixedHeight = fixedHeight;
	}
	const int& IRect::GetFixedWidth()
	{
		return _fixedWidth;
	}
	const int& IRect::GetFixedHeight()
	{
		return _fixedHeight;
	}
	bool IMouseKeyBoard::CheckEventPassThrough(Event eventType)
	{
		if ((MousePassThrough & eventType) == eventType) {
			return true;
		}
		return false;
	}


	IControl::IControl() {}
	IControl::~IControl() {
		if (_hasTimer) {
			KillTimer();
		}
	}
	void IControl::OnTimer() {}
	UINT_PTR IControl::SetTimer(size_t interval)
	{
		if (_hasTimer) {
			return (UINT_PTR)this;
		}
		_hasTimer = true;
		return ::SetTimer(_hWnd, (UINT_PTR)this, interval, NULL);
	}
	void IControl::KillTimer() {
		::KillTimer(_hWnd, (UINT_PTR)this);
		_hasTimer = false;
	}
	void IControl::SetStyleSheet(const EString& styleStr)
	{

	}
	void IControl::SetAttribute(const EString& attrName, const EString& attrValue) {
		AttributeIterator itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			(*itor).second = attrValue;
		}
		else {
			_attrs.insert(std::pair<EString, EString>(attrName, attrValue));
		}
	}
	const EString IControl::GetAttribute(const EString& attrName) {
		AttributeIterator itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			return (*itor).second;
		}
		return "";
	}

};