#pragma once
#include "EzUI.h"
#include "RenderType.h"
#pragma comment(lib, "Msimg32.lib")
namespace EzUI {
	struct EBitmap {
		enum class PixelFormat :int {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		HDC _hdc = NULL;
		HGDIOBJ _hgdiobj = NULL;
		EBitmap(const EBitmap& hBitmap) {}
		EBitmap operator=(const EBitmap& hBitmap) {}
	public:
		HBITMAP _bitmap = NULL;
		WORD Width;
		WORD Height;
		void* point = NULL;
		BITMAPINFO bmi;
		byte biteCount = 0;
		EBitmap(WORD width, WORD height, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB) {//默认24位不透明位图
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
		HDC& GetDC() {
			if (!_hdc) {
				_hdc = ::CreateCompatibleDC(NULL);
				_hgdiobj = ::SelectObject(_hdc, _bitmap);
			}
			return _hdc;
		}
		virtual ~EBitmap() {
			if (_hdc) {
				::SelectObject(_hdc, _hgdiobj);
				::DeleteDC(_hdc);
				::DeleteBitmap(_bitmap);
			}
		}
	};

	using RectF = RenderType::RectF;
	using Size = RenderType::Size;
	using SizeF = RenderType::SizeF;
	using Point = RenderType::Point;
	using PointF = RenderType::PointF;
	using ARGB = RenderType::ARGB;

	typedef struct _Rect :public RenderType::Rect {
	private:
		void StringToRect(const EString& str) {
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
	public:
		_Rect() :RenderType::Rect() {};
		virtual ~_Rect() {};
		_Rect(IN INT x, IN INT y, IN INT width, IN INT height) :RenderType::Rect(x, y, width, height) {};
		_Rect(IN const Point& location, IN const Size& size) :RenderType::Rect(location, size) {};
		_Rect(const EString& rect) {
			StringToRect(rect);
		}
		_Rect& operator=(const EString& rect) {
			StringToRect(rect);
			return *this;
		}
		_Rect(const RECT& winRect) {
			X = winRect.left;
			Y = winRect.top;
			Width = winRect.right - winRect.left;
			Height = winRect.bottom - winRect.top;
		}
		RECT WinRECT() const {
			return RECT{ X,Y,GetRight(),GetBottom() };
		}
	}Rect;


	typedef struct _Color :RenderType::Color {
	public:
		bool valid = false;
	private:
		void _MakeARGB(const EString& colorStr) {
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
	public:
		_Color()
		{
			Argb = (ARGB)Color::Black;
			valid = false;
		}
		virtual ~_Color() {}

		_Color& operator=(const EString& colorStr) {
			_MakeARGB(colorStr);
			return *this;
		}
		_Color(const EString& colorStr) {
			_MakeARGB(colorStr);
		}
		_Color(IN BYTE r,
			IN BYTE g,
			IN BYTE b)
		{
			Argb = MakeARGB(255, r, g, b);
			valid = true;
		}
		_Color(IN BYTE a,
			IN BYTE r,
			IN BYTE g,
			IN BYTE b)
		{
			Argb = MakeARGB(a, r, g, b);
			valid = true;
		}
		_Color(IN ARGB argb)
		{
			Argb = argb;
			valid = true;
		}
		_Color& operator=(const _Color& Align_Right_Color) {
			Argb = Align_Right_Color.GetValue();
			valid = true;
			return *this;
		}
		COLORREF COLORRE() const {
			byte r = GetR(), g = GetG(), b = GetB();
			return RGB(r, g, b);
		}
	}Color;
};
