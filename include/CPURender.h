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
	}Rect;

	class UI_EXPORT  Image :public  Gdiplus::Image {
	public:
		Bitmap* BufBitmap = NULL;//Ԥ����
		Rect Box;//ָ��ͼƬ������ʲôλ�� //��ָ�����Զ����쵽��ǰ�ؼ���
		Image(const EString& filename, int radius = 0);
		virtual ~Image();
		Image(Gdiplus::GpImage* nativeImage, Gdiplus::Status status) :Gdiplus::Image(nativeImage, status) {}
		Image* Clone();
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
	void CreateRectangle(GraphicsPath& path, const Rect& rect, int radius);//����
	void ClipImage(Image* img, const Size& sz, int _radius, Bitmap** outBitmap);//

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
	enum class ImageSizeMode {
		Normal,//
		StretchImage,//����
		Zoom,//�Զ����ݱ�������
		Clip//���ݿ���Զ��ü�
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
				Argb = MakeARGB(255 * (a > 1 ? 1 : (byte)a), r, g, b);
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
		CPURender(Bitmap* image);
		CPURender(HDC hdc);
		CPURender(HWND hWnd);
		virtual ~CPURender();
		void DrawRectangle(const Color& color, const Rect& rect, int width = 1, int radius = 0);
		void FillRectangle(const Color& color, const Rect& rect, int radius = 0);
		void DrawString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const RectF& rect, TextAlign textAlign, bool underLine = false);
		void MeasureString(const EString& _text, const EString& fontf, int fontSize, RectF& outBox);
		//δ����ƫ�� Ԥ���ݲ�ʹ��
		//void MeasureString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const RectF& _rect, TextAlign textAlign, RectF& outBox);
		void CreateLayer(const Rect& rect, ClipMode clipMode = ClipMode::Valid, int radius = 0);
		void PopLayer();
		void DrawLine(const Color& color, const Point& A, const Point& B, int width = 1);
		void DrawImage(Image* image, const Rect& rect, int radius = 0);
		void SaveImage(const WCHAR* format, const WCHAR* fileName, const Size& size);
	};
	void RenderInitialize();
	void RenderUnInitialize();
};