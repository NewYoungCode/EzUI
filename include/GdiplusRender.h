#pragma once
#include "RenderExType.h"
#ifdef USED_GDIPLUS
#include <comdef.h>
#include <GdiPlus.h>
#include <GdiplusTypes.h>
#include <gdiplusimagecodec.h>
#include <gdiplusimaging.h>
#include <gdiplusimageattributes.h>

namespace EzUI {
	class UI_EXPORT GdiplusImage :public Gdiplus::Bitmap, public IImage {
	public:
		virtual ~GdiplusImage() {}
		GdiplusImage(const std::wstring& fileName) :Gdiplus::Bitmap(fileName.c_str()) {}
		GdiplusImage(HBITMAP hBitmap) :Gdiplus::Bitmap(hBitmap, NULL) {}
		GdiplusImage(INT width, INT height, Gdiplus::PixelFormat pixelFormat = PixelFormat32bppARGB) :Gdiplus::Bitmap(width, height, pixelFormat) {}
		void Save(const  std::wstring& fileName);
	};

	class UI_EXPORT GdiplusRender
	{
	protected:
		//函数供内部使用 没有设置偏移所不可直接访问 
		void DrawString(const std::wstring& text, const Gdiplus::Font* font, const Color& color, const RectF& rect, TextAlign textAlign, bool underLine = false);
		void CreateFormat(TextAlign textAlign, Gdiplus::StringFormat& outStrFormat);
		HWND _hwnd = NULL;
	public:
		std::list<Layer*> Layers;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
	public:
		//static Region* IntersectRound(const Rect& clientRect, int r, const Rect& _ClipRect) {
		//	GraphicsPath gp;//控件本身的光栅化路径
		//	gp.AddArc(clientRect.X, clientRect.Y, r, r, 180, 90);//左上角
		//	gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y, r, r, 270, 90);//右上角
		//	gp.AddArc(clientRect.Width + clientRect.X - r, clientRect.Y + clientRect.Height - r, r, r, 0, 90);//右下角
		//	gp.AddArc(clientRect.X, clientRect.Y + clientRect.Height - r, r, r, 90, 90);//左下角
		//	gp.CloseFigure();
		//	Region* region1 = new Region(&gp);
		//	region1->Intersect(_ClipRect);//控件本身区域 和交集 区域做 交集处理 
		//	return region1;
		//}

		Gdiplus::Rect ToRect(const Rect& _rect) {
			return Gdiplus::Rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
		}

		Gdiplus::Color ToColor(const Color& color) {
			return  Gdiplus::Color(color.GetValue());
		}

		Gdiplus::Graphics* base = NULL;

		GdiplusRender(HDC hdc,int Width=0,int Height=0);
		GdiplusRender(HWND hWnd);
		GdiplusRender(Gdiplus::Image* image);
		virtual ~GdiplusRender();
		void DrawRectangle(const Rect& rect, const Color& color, int width = 1, int radius = 0);
		void FillRectangle(const Rect& rect, const Color& color, int radius = 0);
		void DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const Color& color, const Rect& rect, TextAlign textAlign, bool underLine = false);
		void MeasureString(const std::wstring& _text, const std::wstring& fontf, int fontSize, RectF& outBox);
		void CreateLayer(const Layer* rect, ClipMode clipMode = ClipMode::Valid);
		void PopLayer();
		void DrawLine(const Color& color, const Point& A, const Point& B, int width = 1);
		void DrawImage(IImage* image, const Rect& destRect, const Rect& srcRect);
		void DrawImage(IImage* image, const Rect& rect, const ImageSizeMode& imageSizeMode = ImageSizeMode::Zoom, const Margin& margin = 0);
		void SaveImage(const WCHAR* format, const WCHAR* fileName, const Size& size);
		void BeginDraw();
		void EndDraw();
	};

	UI_EXPORT void RenderInitialize();
	UI_EXPORT void RenderUnInitialize();
	UI_EXPORT void HighQualityMode(Gdiplus::Graphics* graphics);
	UI_EXPORT void CreateRectangle(Gdiplus::GraphicsPath& path, const Gdiplus::Rect& rect, int radius);//申明
	UI_EXPORT int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	UI_EXPORT BOOL SaveHDCToFile(HDC hDC, LPRECT lpRect, const WCHAR* format, const WCHAR* filename);
};

#endif