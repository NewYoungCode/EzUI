#include "IType.h"
namespace EzUI {

	size_t __count_onsize = 0;

	EBitmap::EBitmap(WORD width, WORD height, PixelFormat piexlFormat) {//默认24位不透明位图
		biteCount = (byte)piexlFormat;
		this->Width = width;
		this->Height = height;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		BITMAPINFOHEADER& bmih = bmi.bmiHeader;
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biBitCount = biteCount;
		bmih.biCompression = BI_RGB;
		bmih.biPlanes = 1;
		bmih.biWidth = width;
		bmih.biHeight = -height;
		bmih.biSizeImage = width * height * biteCount;
		_bitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &point, NULL, 0);
		this->GetDC();
	}

	void EBitmap::SetPixel(int x, int y, const Color& color) {
		DWORD* point = (DWORD*)this->point + (x + y * this->Width);//起始地址+坐标偏移	
		if (biteCount == 32) { //argb
			((BYTE*)point)[3] = color.GetA();//修改A通道数值
		}
		((BYTE*)point)[2] = color.GetR();//修改R通道数值
		((BYTE*)point)[1] = color.GetG();//修改G通道数值
		((BYTE*)point)[0] = color.GetB();//修改B通道数值
	}
	Color EBitmap::GetPixel(int x, int y) {
		DWORD* point = (DWORD*)this->point + (x + y * this->Width);//起始地址+坐标偏移
		BYTE a = 255, r, g, b;
		if (biteCount == 32) { //argb
			a = ((BYTE*)point)[3];
		}
		r = ((BYTE*)point)[2];//修改R通道数值
		g = ((BYTE*)point)[1];//修改G通道数值
		b = ((BYTE*)point)[0];//修改B通道数值
		return Color(a, r, g, b);
	}

	void EBitmap::Earse(const Rect& _rect) {
		Rect rect = _rect;
		if (rect.X < 0) {
			rect.X = 0;
			rect.Width += rect.X;
		}
		if (rect.Y < 0) {
			rect.Y = 0;
			rect.Height += rect.Y;
		}
		if (rect.GetBottom() > Height) {
			rect.Height = this->Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = this->Width - rect.X;
		}
		for (size_t y = rect.Y; y < rect.GetBottom(); y++)
		{
			DWORD* point = (DWORD*)this->point + (rect.X + y * this->Width);//起始地址+坐标偏移
			::memset(point, 0, rect.Width * 4);//抹除
		}
	}
	void EBitmap::FillRect(const Rect& _rect, const Color& color) {
		Rect rect = _rect;
		if (rect.X < 0) {
			rect.X = 0;
			rect.Width += rect.X;
		}
		if (rect.Y < 0) {
			rect.Y = 0;
			rect.Height += rect.Y;
		}
		if (rect.GetBottom() > Height) {
			rect.Height = this->Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = this->Width - rect.X;
		}

		BYTE a = color.GetA();
		BYTE r = color.GetR();
		BYTE g = color.GetG();
		BYTE b = color.GetB();
		DWORD c1 = 0;
		BYTE* c = (BYTE*)&c1;
		c[3] = a;
		c[2] = r;
		c[1] = g;
		c[0] = b;

		for (size_t y = rect.Y; y < rect.GetBottom(); y++)
		{
			DWORD* point = (DWORD*)this->point + (rect.X + y * this->Width);//起始地址+坐标偏移
			::memset(point, 255, rect.Width * 4);//抹除
		}
	}

	HDC& EBitmap::GetDC() {
		if (!_hdc) {
			_hdc = ::CreateCompatibleDC(NULL);
			::SelectObject(_hdc, _bitmap);
		}
		return _hdc;
	}
	EBitmap::~EBitmap() {
		if (_hdc) {
			::DeleteDC(_hdc);
			::DeleteBitmap(_bitmap);
		}
	}

	void Rect::StringToRect(const EString& str) {
		auto rectStr = str.Split(",");
		if (str.empty()) {
			X = Y = Width = Height = 0;
			return;//如果没写矩形区域
		}
		X = std::stoi(rectStr.at(0));
		Y = std::stoi(rectStr.at(1));
		Width = std::stoi(rectStr.at(2));
		Height = std::stoi(rectStr.at(3));
	}
	Rect::Rect(const EString& rect) {
		StringToRect(rect);
	}
	Rect& Rect::operator=(const EString& rect) {
		StringToRect(rect);
		return *this;
	}
	Rect::Rect(const RECT& winRect) {
		X = winRect.left;
		Y = winRect.top;
		Width = winRect.right - winRect.left;
		Height = winRect.bottom - winRect.top;
	}


	Color::Color()
	{
		Argb = (ARGB)Color::Black;
		valid = false;
	}
	Color::Color(const EString& colorStr) {
		_MakeARGB(colorStr);
	}
	Color::Color(IN BYTE r, IN BYTE g, IN BYTE b) {
		Argb = MakeARGB(255, r, g, b);
		valid = true;
	}
	Color::Color(IN BYTE a, IN BYTE r, IN BYTE g, IN BYTE b)
	{
		Argb = MakeARGB(a, r, g, b);
		valid = true;
	}
	Color::Color(IN ARGB argb)
	{
		Argb = argb;
		valid = true;
	}
	Color& Color::operator=(const Color& Align_Right_Color) {
		Argb = Align_Right_Color.GetValue();
		valid = true;
		return *this;
	}
	Color& Color::operator=(const EString& colorStr) {
		_MakeARGB(colorStr);
		return *this;
	}
	void Color::_MakeARGB(const EString& colorStr) {
		if (colorStr.find("#") == 0) { //"#4e6ef2"
			auto rStr = colorStr.substr(1, 2);
			auto gStr = colorStr.substr(3, 2);
			auto bStr = colorStr.substr(5, 2);
			unsigned int r, g, b;
			sscanf_s(rStr.c_str(), "%x", &r);
			sscanf_s(gStr.c_str(), "%x", &g);
			sscanf_s(bStr.c_str(), "%x", &b);
			Argb = MakeARGB(255, r, g, b);
			valid = true;
			return;
		}
		if (colorStr.find("rgb") == 0) { //"rgb(255,100,2,3)"
			int pos1 = colorStr.find("(");
			int pos2 = colorStr.rfind(")");
			EString rgbStr = colorStr.substr(pos1 + 1, pos2 - pos1 - 1);
			auto rgbList = rgbStr.Split(",");
			unsigned char r, g, b;
			float a = rgbList.size() == 3 ? 1 : std::stof(rgbList.at(3));//透明百分比 0~1
			r = std::stoi(rgbList.at(0));
			g = std::stoi(rgbList.at(1));
			b = std::stoi(rgbList.at(2));
			Argb = MakeARGB((byte)(255 * (a > 1 ? 1 : a)), r, g, b);
			valid = true;
			return;
		}

	}
	COLORREF Color::COLORRE() const {
		byte r = GetR(), g = GetG(), b = GetB();
		return RGB(r, g, b);
	}


	void ControlStyle::SetBorder(const Color& color, int width) { //对所有border有效
		BorderColor = color;
		BorderLeft = width;//左边边框
		BorderTop = width;//顶部边框
		BorderRight = width;//右边边框
		BorderBottom = width;//底部边框
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
				value = value.Erase('"');//删除双引号;
				style->BackgroundImage = new Image(value.utf16());
				break;
			}
			if (key == "fore-image") {
				value = value.Erase('"');//删除双引号;
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
			if (key == "radius" || key == "border-radius") {
				style->Radius = std::stoi(value);
				break;
			}
			if (key == "font-size") {
				style->FontSize = std::stoi(value);
				break;
			}
			if (key == "font-family") {
				value = value.Erase('"');//删除双引号;
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
	IControl::~IControl() {

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
	EString IControl::GetAttribute(const EString& attrName) {
		AttributeIterator itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			return (*itor).second;
		}
		return "";
	}

};