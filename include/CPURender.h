#pragma once
#include "EzUI.h"
#include <comdef.h>
#include <GdiPlus.h>
#include <gdiplusimagecodec.h>
#include <gdiplusimaging.h>
#include <gdiplusimageattributes.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")
namespace EzUI {

	class Margin {
	public:
		size_t Left, Top, Right, Bottom;
		Margin() {
			Left = Top = Right = Bottom = 0;
		}
		Margin(const size_t& marginAll) {
			Left = Top = Right = Bottom = marginAll;
		}
		Margin& operator=(const size_t& marginAll) {
			Left = Top = Right = Bottom = marginAll;
			return *this;
		}
	};

	struct EBitmap {
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
		EBitmap(WORD width, WORD height, BYTE _bitCount = 24) {//默认24位不透明位图
			biteCount = _bitCount;
			this->Width = width;
			this->Height = height;
			memset(&bmi, 0, sizeof(BITMAPINFO));
			BITMAPINFOHEADER& bmih = bmi.bmiHeader;
			bmih.biSize = sizeof(BITMAPINFOHEADER);
			bmih.biBitCount = _bitCount;
			bmih.biCompression = BI_RGB;
			bmih.biPlanes = 1;
			bmih.biWidth = width;
			bmih.biHeight = -height;
			bmih.biSizeImage = width * height * _bitCount;
			_bitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &point, NULL, 0);
		}
		HDC& GetHDC() {
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
	using Bitmap = Gdiplus::Bitmap;
	//using Rect = Gdiplus::Rect;
	using RectF = Gdiplus::RectF;
	using Size = Gdiplus::Size;
	using SizeF = Gdiplus::SizeF;
	using Point = Gdiplus::Point;
	using PointF = Gdiplus::PointF;
	using SolidBrush = Gdiplus::SolidBrush;
	using GraphicsPath = Gdiplus::GraphicsPath;
	using Region = Gdiplus::Region;
	using Pen = Gdiplus::Pen;
	using ARGB = Gdiplus::ARGB;

	typedef struct _Rect :Gdiplus::Rect {
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
		_Rect() :Gdiplus::Rect() {};
		virtual ~_Rect() {};
		_Rect(IN INT x, IN INT y, IN INT width, IN INT height) :Gdiplus::Rect(x, y, width, height) {};
		_Rect(IN const Point& location, IN const Size& size) :Gdiplus::Rect(location, size) {};
		_Rect(const EString& rect) {
			StringToRect(rect);
		}
		_Rect& operator=(const EString& rect) {
			StringToRect(rect);
			return *this;
		}
	}Rect;

	class UI_EXPORT _Bitamp :public  Gdiplus::Bitmap {
	public:
		_Bitamp(const EBitmap& eBitmap) :Gdiplus::Bitmap(&eBitmap.bmi, eBitmap.point) {}
	};

	class UI_EXPORT  Image :public  Gdiplus::Image {
	public:
		//边距
		EzUI::Margin Margin;
		enum class SizeMode {
			//
			// 摘要:
			//     Owner控件 中的图像被拉伸或收缩，以适合 Owner控件
			//     的大小。
			StretchImage = 1,
			// 摘要:
			//     如果 Owner控件 比图像大，则图像将居中显示。如果图像比 Owner控件
			//     大，则图片将居于 Owner控件 中心，而外边缘将被剪裁掉。
			CenterImage = 3,
			//
			// 摘要:
			//     图像大小按其原有的大小比例被增加或减小。
			Zoom = 4
		};
		//图像显示模式
		Image::SizeMode SizeMode = Image::SizeMode::Zoom;
		Image(const EString& filename);
		virtual ~Image();
		Image(Gdiplus::GpImage* nativeImage, Gdiplus::Status status) :Gdiplus::Image(nativeImage, status) {}
	};
	//class HImage {
	//private:
	//	void Clone(const Image* img);
	//public:
	//	int ref = 0;
	//	bool valid = false;
	//	Image* value = NULL;
	//	HImage(const HImage& img);
	//	HImage& operator=(const HImage& img);
	//	HImage();
	//	void operator=(const Image* img);
	//	operator Image* ();
	//	Image* operator->();
	//	~HImage();
	//};

#define HImage Tuple<Image*>
	void HighQualityMode(Gdiplus::Graphics* graphics);
	void CreateRectangle(GraphicsPath& path, const Rect& rect, int radius);//申明
#define Align_Top  1
#define Align_Bottom  2
#define Align_Left  4
#define Align_Right  8
#define Align_Mid  16
#define Align_Center  32

	enum class  Align :int
	{
		Top = Align_Top,
		Bottom = Align_Bottom,
		Left = Align_Left,
		Right = Align_Right,
		Mid = Align_Mid,
		Center = Align_Center
	};
	enum class TextAlign :int {
		// 摘要: 
		   //     内容在垂直方向上顶部对齐，在水平方向上左边对齐。
		TopLeft = Align_Top | Align_Left,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上居中对齐。
		TopCenter = Align_Top | Align_Center,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上右边对齐。
		TopRight = Align_Top | Align_Right,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上左边对齐。
		MiddleLeft = Align_Mid | Align_Left,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上居中对齐。
		MiddleCenter = Align_Mid | Align_Center,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上右边对齐。
		MiddleRight = Align_Mid | Align_Right,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上左边对齐。
		BottomLeft = Align_Bottom | Align_Left,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上居中对齐。
		BottomCenter = Align_Bottom | Align_Center,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上右边对齐。
		BottomRight = Align_Bottom | Align_Right
	};
	enum class ImageSizeMode {
		Normal,//
		StretchImage,//拉伸
		Zoom,//自动根据比例居中
		Clip//根据宽高自动裁剪
	};


	typedef struct _Color :Gdiplus::Color {
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
	}Color;

	enum ClipMode {
		Valid = Gdiplus::CombineMode::CombineModeReplace,//设置有效区域
		Invalid = Gdiplus::CombineMode::CombineModeExclude,//设置无效区域
	};

	class UI_EXPORT CPURender
	{
	protected:
		//函数供内部使用 没有设置偏移所不可直接访问 
		void DrawString(const std::wstring& text, const Gdiplus::Font* font, const Color& color, const RectF& rect, TextAlign textAlign, bool underLine = false);
		void CreateFormat(TextAlign textAlign, Gdiplus::StringFormat& outStrFormat);
	public:
		static Region* IntersectRound(const Rect& clientRect, int r, const Rect& _ClipRect) {
			GraphicsPath gp;//控件本身的光栅化路径
			gp.AddArc(clientRect.X, clientRect.Y, r, r, 180, 90);//左上角
			gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y, r, r, 270, 90);//右上角
			gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y + clientRect.Height - r, r, r, 0, 90);//右下角
			gp.AddArc(clientRect.X, clientRect.Y + clientRect.Height - r, r, r, 90, 90);//左下角
			gp.CloseFigure();
			Region* region1 = new Region(&gp);
			region1->Intersect(_ClipRect);//控件本身区域 和交集 区域做 交集处理 
			return region1;
		}
		Gdiplus::Graphics* base = NULL;
		std::list<const Rect*> Layer;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
		CPURender(Bitmap* image);
		CPURender(HDC hdc);
		CPURender(HWND hWnd);
		virtual ~CPURender();
		void DrawRectangle(const Color& color, const Rect& rect, int width = 1, int radius = 0);
		void FillRectangle(const Color& color, const Rect& rect, int radius = 0);
		void DrawString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const RectF& rect, TextAlign textAlign, bool underLine = false);
		void MeasureString(const EString& _text, const EString& fontf, int fontSize, RectF& outBox);
		//未计算偏移 预留暂不使用
		//void MeasureString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const RectF& _rect, TextAlign textAlign, RectF& outBox);
		void CreateLayer(const Rect& rect, ClipMode clipMode = ClipMode::Valid, int radius = 0);
		void PopLayer();
		void DrawLine(const Color& color, const Point& A, const Point& B, int width = 1);
		void DrawImage(Bitmap* image, const Rect& _rect);
		void DrawImage(Image* image, const Rect& rect, int radius = 0);
		void SaveImage(const WCHAR* format, const WCHAR* fileName, const Size& size);
	};
	void RenderInitialize();
	void RenderUnInitialize();

	extern int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	extern BOOL SaveHDCToFile(HDC hDC, LPRECT lpRect, const WCHAR* format, const WCHAR* filename);
};
