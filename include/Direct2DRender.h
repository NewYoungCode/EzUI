#pragma once
#include "RenderExType.h"
#ifdef USED_Direct2D
#ifndef UI_EXPORT
#define UI_EXPORT 
#endif
// Windows Header Files:
#include <windows.h>
// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
namespace EzUI {

	extern ID2D1Factory* g_Direct2dFactory;

	class UI_EXPORT D2DImage : public IImage {
	protected:
		IWICBitmapDecoder* bitmapdecoder = NULL;
		IWICBitmapFrameDecode* pframe = NULL;
		IWICFormatConverter* fmtcovter = NULL;//从文件加载
		IWICBitmap* bitMap = NULL;//从HBITMAP中加载
		UINT Width = 0;
		UINT Height = 0;
	public:
		ID2D1Bitmap* d2dBitmap = NULL;
	public:
		void DecodeOfRender(ID2D1RenderTarget* render);
		D2DImage(const std::wstring& file);
		D2DImage(HBITMAP hBitmap);
		UINT GetWidth();
		UINT GetHeight();
		D2DImage() {}
		virtual ~D2DImage();
	};
	class UI_EXPORT Direct2DRender {
	protected:
		HWND hWnd = NULL;
		bool beginDraw = false;
	public:
		ID2D1RenderTarget* d2dRender = NULL;
		std::list<Layer*> Layers;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
	protected:
		virtual void DrawBitmap(ID2D1Bitmap* d2dBitmap, const Rect& rect);
	public:
		Direct2DRender(HDC _dc, int Width = 0, int Height = 0);
		Direct2DRender(HWND hWnd, int Width = 0, int Height = 0);
		D2D_COLOR_F ToColorF(const Color& color) {
			float&& aF = color.GetA() == 255 ? 1.0 : float(color.GetA() * 0.003921568627451);
			float&& rF = float(color.GetR() * 0.003921568627451);
			float&& gF = float(color.GetG() * 0.003921568627451);
			float&& bF = float(color.GetB() * 0.003921568627451);
			return D2D1::ColorF(rF, gF, bF, aF);
		}
		D2D_RECT_F ToRectF(const Rect& rect) {
			return D2D_RECT_F{ (float)rect.X,(float)rect.Y,(float)rect.GetRight(),(float)rect.GetBottom() };
		}
		virtual ~Direct2DRender();
		virtual void DrawRectangle(const Rect& rect, const Color& color, int width = 1, int radius = 0);
		virtual void FillRectangle(const Rect& rect, const Color& color, int radius = 0);
		virtual void DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const Color& color, const Rect& rect, TextAlign textAlign, bool underLine = false);
		virtual void MeasureString(const std::wstring& _text, const std::wstring& fontf, int fontSize, RectF& outBox);
		virtual void CreateLayer(const Layer* rect, ClipMode clipMode = ClipMode::Valid);
		virtual void PopLayer();
		virtual void DrawLine(const Color& color, const Point& A, const Point& B, int width = 1);
		virtual void DrawImage(IImage* image, const Rect& destRect, const Rect& srcRect);
		virtual void DrawImage(IImage* image, const Rect& rect, const ImageSizeMode& imageSizeMode = ImageSizeMode::Zoom, const Margin& margin = 0);
		virtual void DrawGeometry(ID2D1Geometry* geometry, const Color& color, int width = 1);
		virtual void FillGeometry(ID2D1Geometry* geometry, const Color& color);

		void BeginDraw();
		void EndDraw();
	};

	UI_EXPORT void RenderInitialize();
	UI_EXPORT void RenderUnInitialize();
};
#endif