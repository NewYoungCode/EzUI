#pragma once
#include "EzUI.h"
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
#include <string>
#include <list>
#include <map>
#include "RenderType.h"
namespace EzUI {
	extern ID2D1Factory* g_Direct2dFactory;
#define DX_Rect RenderType::Rect
#define DX_RectF RenderType::RectF
#define DX_Color RenderType::Color
#define DX_Point RenderType::Point
#define DX_PointF RenderType::PointF
#define DX_ARGB RenderType::ARGB
#define DX_Size RenderType::Size
#define DX_SizeF RenderType::SizeF

	class DxGeometry
	{
	public:
		bool Ref = false;
		ID2D1Geometry* Geometry = NULL;
	protected:
		void Copy(const DxGeometry& _copy) {
			((DxGeometry&)(_copy)).Ref = true;
			this->Geometry = ((DxGeometry&)(_copy)).Geometry;
		}
	public:
		DxGeometry() {}
		DxGeometry(const DxGeometry& _copy) {
			Copy(_copy);
		}
		DxGeometry& operator =(const DxGeometry& _right) {
			Copy(_right);
		}
		DxGeometry(int x, int y, int width, int height) {
			D2D_RECT_F rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) };
			g_Direct2dFactory->CreateRectangleGeometry(rectF, (ID2D1RectangleGeometry**)&Geometry);
		}
		DxGeometry(const DX_Rect& rect) :DxGeometry(rect.X, rect.Y, rect.Width, rect.Height) {};
		DxGeometry(int x, int y, int width, int height, int radius) {
			radius = radius / 2.0;
			D2D1_ROUNDED_RECT rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) ,radius ,radius };
			g_Direct2dFactory->CreateRoundedRectangleGeometry(rectF, (ID2D1RoundedRectangleGeometry**)&Geometry);
		}
		virtual ~DxGeometry() {
			if (Geometry && !Ref) {
				Geometry->Release();
			}
		}
	public:
		static void Combine(DxGeometry& out, const DxGeometry& a, const DxGeometry& b, D2D1_COMBINE_MODE COMBINE_MODE)
		{
			ID2D1PathGeometry* outPathGeometry = NULL;
			g_Direct2dFactory->CreatePathGeometry(&outPathGeometry);
			ID2D1GeometrySink* geometrySink = NULL;
			outPathGeometry->Open(&geometrySink);
			HRESULT ret = a.Geometry->CombineWithGeometry(b.Geometry, COMBINE_MODE, NULL, geometrySink);
			geometrySink->Close();
			if (out.Geometry) {
				out.Geometry->Release();
			}
			out.Geometry = outPathGeometry;
			geometrySink->Release();
		}
		//两块区域取最大边界
		static void Union(DxGeometry& out, const DxGeometry& a, const DxGeometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_UNION);
		}
		//两块区域有交集的部分
		static void Intersect(DxGeometry& out, const DxGeometry& a, const DxGeometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_INTERSECT);
		}
		static void Xor(DxGeometry& out, const DxGeometry& a, const DxGeometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_XOR);
		}
		static void Exclude(DxGeometry& out, const DxGeometry& a, const DxGeometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_EXCLUDE);
		}
	};


	class UI_EXPORT DXImage : public IImage {
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
		DXImage(const std::wstring& file);
		DXImage(HBITMAP hBitmap);
		UINT GetWidth();
		UINT GetHeight();
		DXImage() {}
		virtual ~DXImage();
	};
	class UI_EXPORT Direct2DRender {
	protected:
		HWND hWnd = NULL;
		bool beginDraw = false;
	public:
		ID2D1RenderTarget* d2dRender = NULL;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
		std::map<DX_ARGB, ID2D1Brush*> CacheBrush;
		std::map<std::wstring, IDWriteTextFormat*> CacheTextFormat;
	protected:
		virtual void DrawBitmap(ID2D1Bitmap* d2dBitmap, const  DX_Rect& rect);
	public:
		static D2D_COLOR_F ToColorF(const  DX_Color& color) {
			FLOAT&& aF = color.GetA() == 255 ? 1.0 : FLOAT(color.GetA() * 0.003921568627451);
			FLOAT&& rF = FLOAT(color.GetR() * 0.003921568627451);
			FLOAT&& gF = FLOAT(color.GetG() * 0.003921568627451);
			FLOAT&& bF = FLOAT(color.GetB() * 0.003921568627451);
			return D2D1::ColorF(rF, gF, bF, aF);
		}
		static D2D_RECT_F ToRectF(const DX_Rect& rect) {
			return D2D_RECT_F{ (FLOAT)rect.X,(FLOAT)rect.Y,(FLOAT)rect.GetRight(),(FLOAT)rect.GetBottom() };
		}
	public:
		Direct2DRender(HDC _dc, int Width = 0, int Height = 0);
		Direct2DRender(HWND hWnd, int Width = 0, int Height = 0);
		virtual ~Direct2DRender();
		virtual void DrawRectangle(const  DX_Rect& rect, const  DX_Color& color, int width = 1, int radius = 0);
		virtual void FillRectangle(const  DX_Rect& rect, const  DX_Color& color, int radius = 0);
		virtual void DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  DX_Color& color, const  DX_Rect& rect, EzUI::TextAlign textAlign, bool underLine = false);
		virtual void MeasureString(const std::wstring& _text, const std::wstring& fontf, int fontSize, DX_RectF& outBox);
		virtual void PushLayer(const DxGeometry& dxGeometry, EzUI::ClipMode clipMode = EzUI::ClipMode::Valid);//不规则裁剪区域
		virtual void PushLayer(const DX_Rect& layer, ClipMode clipMode);//矩形裁剪区域
		virtual void PopLayer();
		virtual void DrawLine(const  DX_Color& color, const  DX_Point& A, const  DX_Point& B, int width = 1);
		virtual void DrawImage(IImage* image, const  DX_Rect& destRect, const DX_Rect& srcRect);
		virtual void DrawImage(IImage* image, const  DX_Rect& rect, const EzUI::ImageSizeMode& imageSizeMode = EzUI::ImageSizeMode::Zoom, const EzUI::Margin& margin = 0);
		virtual void DrawGeometry(ID2D1Geometry* geometry, const  DX_Color& color, int width = 1);
		virtual void FillGeometry(ID2D1Geometry* geometry, const  DX_Color& color);
		virtual void DrawGeometry(const DxGeometry& geometry, const  DX_Color& color, int width = 1) {
			DrawGeometry(geometry.Geometry, color);
		}
		virtual void FillGeometry(const DxGeometry& geometry, const  DX_Color& color) {
			FillGeometry(geometry.Geometry, color);
		}
		virtual ID2D1Brush* CreateSafeSolidBrush(const DX_Color& color);
		virtual IDWriteTextFormat* CreateSafeTextFormat(const std::wstring& fontFamily, int fontSize);
		void BeginDraw();
		void EndDraw();
	};

	UI_EXPORT void RenderInitialize();//全局初始化direct2d
	UI_EXPORT void RenderUnInitialize();//释放direct2d
};
#endif