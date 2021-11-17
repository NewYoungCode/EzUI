#pragma once
#include "EzUI.h"
#include <comdef.h>
#include <GdiPlus.h>
#include <gdiplusimagecodec.h>
#include <gdiplusimaging.h>
#include <gdiplusimageattributes.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")


using Bitmap = Gdiplus::Bitmap;
using Rect = Gdiplus::Rect;
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

class UI_EXPORT  Image :public  Gdiplus::Image {
public:
	Bitmap *BufBitmap = NULL;//Ԥ����
	Rect Box;//ָ��ͼƬ������ʲôλ�� //��ָ�����Զ����쵽��ǰ�ؼ���
	Image(const EString&filename, int radius = 0);
	~Image();
};

#define HImage Tuple<Image*>
void HighQualityMode(Gdiplus::Graphics*graphics);
void CreateRectangle(GraphicsPath &path, const Rect& rect, float radius);//����
void ClipImage(Image * img, const Size&sz, int _radius, Bitmap ** outBitmap);//

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
	MiddleLeft = Align_Mid| Align_Left,
	//
	// ժҪ: 
	//     �����ڴ�ֱ�������м���룬��ˮƽ�����Ͼ��ж��롣
	MiddleCenter = Align_Mid| Align_Center,
	//
	// ժҪ: 
	//     �����ڴ�ֱ�������м���룬��ˮƽ�������ұ߶��롣
	MiddleRight = Align_Mid| Align_Right,
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
public:
	_Color()
	{
		Argb = (ARGB)Color::Black;
		valid = false;
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
	_Color& operator=(const _Color&Align_Right_Color) {
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

	void DrawString(const std::wstring&text, const Gdiplus::Font*font, const Color&color, const RectF&rect, TextAlign textAlign, bool underLine = false);
	void CreateFormat(TextAlign textAlign, Gdiplus::StringFormat & outStrFormat);
public:
	static Region* IntersectRound(const Rect& clientRect, int r, const Rect& _ClipRect) {
		GraphicsPath gp;//�ؼ�����Ĺ�դ��·��
		gp.AddArc(clientRect.X, clientRect.Y, r, r, 180, 90);//���Ͻ�
		gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y, r, r, 270, 90);//���Ͻ�
		gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y + clientRect.Height - r, r, r, 0, 90);//���½�
		gp.AddArc(clientRect.X, clientRect.Y + clientRect.Height - r, r, r, 90, 90);//���½�
		gp.CloseFigure();
		Region *region1 = new Region(&gp);
		region1->Intersect(_ClipRect);//�ؼ��������� �ͽ��� ������ �������� 
		return region1;
	}
	Gdiplus::Graphics *base = NULL;
	std::list<const Rect*> Layer;
	HDC DC = NULL;
	int OffsetX = 0;
	int OffsetY = 0;
	CPURender(Bitmap *image);
	CPURender(HDC hdc);
	CPURender(HWND hWnd);
	virtual ~CPURender();
	void DrawRectangle(const Color&color, const Rect&rect, float width = 1, float radius = 0);
	void FillRectangle(const Color&color, const Rect&rect, int radius = 0);
	void DrawString(const EString &text, const EString& fontFamily, float fontSize, const Color & color, const RectF & rect, TextAlign textAlign, bool underLine = false);
	void DrawString(const std::wstring & text, const EString & fontFamily, float fontSize, const Color & color, const RectF & _rect, TextAlign textAlign, bool underLine = false);
	void MeasureString(const EString&_text, const EString&fontf, float fontSize, RectF &outBox);
	void MeasureString(const std::wstring & _text, const EString & fontf, float fontSize, RectF & outBox);
	void CreateLayer(const Rect&rect, ClipMode clipMode = ClipMode::Valid, int radius = 0);
	void PopLayer();
	void DrawLine(const Color & color, const Point&A, const Point&B, float width = 1);
	void DrawImage(Image*image, const Rect&rect, float radius = 0);
	void SaveImage(const WCHAR * format, const WCHAR* fileName, const Size&size);
};
void RenderInitialize();
void RenderUnInitialize();
