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
	Bitmap *BufBitmap = NULL;//预绘制
	Rect Box;//指定图片绘制在什么位置 //不指定就自动拉伸到当前控件上
	Image(const EString&filename, int radius = 0);
	~Image();
};

#define HImage Tuple<Image*>
void HighQualityMode(Gdiplus::Graphics*graphics);
void CreateRectangle(GraphicsPath &path, const Rect& rect, float radius);//申明
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
	MiddleLeft = Align_Mid| Align_Left,
	//
	// 摘要: 
	//     内容在垂直方向上中间对齐，在水平方向上居中对齐。
	MiddleCenter = Align_Mid| Align_Center,
	//
	// 摘要: 
	//     内容在垂直方向上中间对齐，在水平方向上右边对齐。
	MiddleRight = Align_Mid| Align_Right,
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
	Valid = Gdiplus::CombineMode::CombineModeReplace,//设置有效区域
	Invalid = Gdiplus::CombineMode::CombineModeExclude,//设置无效区域
};

class UI_EXPORT CPURender
{
protected:
	//函数供内部使用 没有设置偏移所不可直接访问 

	void DrawString(const std::wstring&text, const Gdiplus::Font*font, const Color&color, const RectF&rect, TextAlign textAlign, bool underLine = false);
	void CreateFormat(TextAlign textAlign, Gdiplus::StringFormat & outStrFormat);
public:
	static Region* IntersectRound(const Rect& clientRect, int r, const Rect& _ClipRect) {
		GraphicsPath gp;//控件本身的光栅化路径
		gp.AddArc(clientRect.X, clientRect.Y, r, r, 180, 90);//左上角
		gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y, r, r, 270, 90);//右上角
		gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y + clientRect.Height - r, r, r, 0, 90);//右下角
		gp.AddArc(clientRect.X, clientRect.Y + clientRect.Height - r, r, r, 90, 90);//左下角
		gp.CloseFigure();
		Region *region1 = new Region(&gp);
		region1->Intersect(_ClipRect);//控件本身区域 和交集 区域做 交集处理 
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
