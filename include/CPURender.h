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
		EBitmap(WORD width, WORD height, BYTE _bitCount = 24) {//Ĭ��24λ��͸��λͼ
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
	//using Bitmap = Gdiplus::Bitmap;
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
				return;//���ûд��������
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


	enum class ImageSizeMode {
		//
		// ժҪ:
		//     Owner�ؼ� �е�ͼ����������������ʺ� Owner�ؼ�
		//     �Ĵ�С��
		StretchImage = 1,
		// ժҪ:
		//     ��� Owner�ؼ� ��ͼ�����ͼ�񽫾�����ʾ�����ͼ��� Owner�ؼ�
		//     ����ͼƬ������ Owner�ؼ� ���ģ������Ե�������õ���
		CenterImage = 3,
		//
		// ժҪ:
		//     ͼ���С����ԭ�еĴ�С���������ӻ��С��
		Zoom = 4
	};

	class UI_EXPORT UIImageBase {
	public:
		EzUI::Margin Margin;// �ؼ���ͼƬ�ľ��� ����ֵԽ�� ͼƬ��ԽС
		ImageSizeMode SizeMode = ImageSizeMode::Zoom;// ͼ����ʾģʽ
	};
	typedef class UI_EXPORT _Bitmap :public Gdiplus::Bitmap, public UIImageBase {
	public:
		virtual ~_Bitmap() {}
		_Bitmap(const EString& fileName) :Gdiplus::Bitmap(fileName.utf16().c_str()) {}
		_Bitmap(const EBitmap* eBitmap) :Gdiplus::Bitmap(&eBitmap->bmi, eBitmap->point) {}
		_Bitmap(INT width, INT height, Gdiplus::PixelFormat pixelFormat = PixelFormat32bppARGB) :Gdiplus::Bitmap(width, height, pixelFormat) {}
	} Image;
#define HImage Tuple<Image*>

	extern void HighQualityMode(Gdiplus::Graphics* graphics);
	extern void CreateRectangle(GraphicsPath& path, const Rect& rect, int radius);//����

#if 0 //GDI+
#define Align_Top  1
#define Align_Bottom  2
#define Align_Left  4
#define Align_Right  8
#define Align_Mid  16
#define Align_Center  32
#else //GDI
#define Align_Top  DT_TOP
#define Align_Bottom  DT_BOTTOM
#define Align_Left  DT_LEFT
#define Align_Right  DT_RIGHT
#define Align_Mid   DT_VCENTER
#define Align_Center   DT_CENTER
#endif
	enum class Align :int
	{
		Top = Align_Top,
		Bottom = Align_Bottom,
		Left = Align_Left,
		Right = Align_Right,
		Mid = Align_Mid,
		Center = Align_Center
	};
	enum  class TextAlign :int {
		// ժҪ: 
		   //     �����ڴ�ֱ�����϶������룬��ˮƽ��������߶��롣
		TopLeft = Align_Top | Align_Left,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����϶������룬��ˮƽ�����Ͼ��ж��롣
		TopCenter = Align_Top | Align_Center,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����϶������룬��ˮƽ�������ұ߶��롣
		TopRight = Align_Top | Align_Right,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�������м���룬��ˮƽ��������߶��롣
		MiddleLeft = Align_Mid | Align_Left,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�������м���룬��ˮƽ�����Ͼ��ж��롣
		MiddleCenter = Align_Mid | Align_Center,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�������м���룬��ˮƽ�������ұ߶��롣
		MiddleRight = Align_Mid | Align_Right,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����ϵױ߶��룬��ˮƽ��������߶��롣
		BottomLeft = Align_Bottom | Align_Left,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����ϵױ߶��룬��ˮƽ�����Ͼ��ж��롣
		BottomCenter = Align_Bottom | Align_Center,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����ϵױ߶��룬��ˮƽ�������ұ߶��롣
		BottomRight = Align_Bottom | Align_Right
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
				float a = rgbList.size() == 3 ? 1 : std::stof(rgbList.at(3));//͸���ٷֱ� 0~1
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
		Valid = Gdiplus::CombineMode::CombineModeReplace,//������Ч����
		Invalid = Gdiplus::CombineMode::CombineModeExclude,//������Ч����
	};

	class UI_EXPORT CPURender
	{
	protected:
		//�������ڲ�ʹ�� û������ƫ��������ֱ�ӷ��� 
		void DrawString(const std::wstring& text, const Gdiplus::Font* font, const Color& color, const RectF& rect, TextAlign textAlign, bool underLine = false);
		void CreateFormat(TextAlign textAlign, Gdiplus::StringFormat& outStrFormat);
	public:
		static Region* IntersectRound(const Rect& clientRect, int r, const Rect& _ClipRect) {
			GraphicsPath gp;//�ؼ�����Ĺ�դ��·��
			gp.AddArc(clientRect.X, clientRect.Y, r, r, 180, 90);//���Ͻ�
			gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y, r, r, 270, 90);//���Ͻ�
			gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y + clientRect.Height - r, r, r, 0, 90);//���½�
			gp.AddArc(clientRect.X, clientRect.Y + clientRect.Height - r, r, r, 90, 90);//���½�
			gp.CloseFigure();
			Region* region1 = new Region(&gp);
			region1->Intersect(_ClipRect);//�ؼ��������� �ͽ��� ������ �������� 
			return region1;
		}
		Gdiplus::Graphics* base = NULL;
		std::list<const Rect*> Layer;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
		CPURender(HDC hdc);
		CPURender(HWND hWnd);
		virtual ~CPURender();
		void DrawRectangle(const Rect& rect, const Color& color, int width = 1, int radius = 0);
		void FillRectangle(const Rect& rect, const Color& color, int radius = 0);
		void DrawString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const Rect& rect, TextAlign textAlign, bool underLine = false);
		void MeasureString(const EString& _text, const EString& fontf, int fontSize, RectF& outBox);
		//δ����ƫ�� Ԥ���ݲ�ʹ��
		//void MeasureString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const RectF& _rect, TextAlign textAlign, RectF& outBox);
		void CreateLayer(const Rect& rect, ClipMode clipMode = ClipMode::Valid, int radius = 0);
		void PopLayer();
		void DrawLine(const Color& color, const Point& A, const Point& B, int width = 1);
		void DrawImage(Gdiplus::Image* image, const Rect& rect, const ImageSizeMode& imageSizeMode = ImageSizeMode::Zoom, const Margin& margin = 0);
		void SaveImage(const WCHAR* format, const WCHAR* fileName, const Size& size);
	};
	void RenderInitialize();
	void RenderUnInitialize();

	extern int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	extern BOOL SaveHDCToFile(HDC hDC, LPRECT lpRect, const WCHAR* format, const WCHAR* filename);
};
