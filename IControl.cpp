#include "IControl.h"

void ControlStyle::SetBorder(const Color& color, int width) { //¶ÔËùÓÐborderÓÐÐ§
	BorderColor = color;
	BorderLeft = width;//×ó±ß±ß¿ò
	BorderTop = width;//¶¥²¿±ß¿ò
	BorderRight = width;//ÓÒ±ß±ß¿ò
	BorderBottom = width;//µ×²¿±ß¿ò
}
bool ControlStyle::IsValid() {
	return Radius.valid ||
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
			style->BackgroundImage = new Image(value);
			break;
		}
		if (key == "fore-image") {
			value = value.Erase('"');//É¾³ýË«ÒýºÅ;
			style->ForeImage = new Image(value);
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

IControl::IControl() {}
IControl::~IControl() {}
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