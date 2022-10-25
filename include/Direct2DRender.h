#pragma once
#include "UIDef.h"
#if USED_Direct2D
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
	extern IDWriteFactory* g_WriteFactory;
	extern IWICImagingFactory* g_ImageFactory;
	extern int Dpi;

	template<typename T>
	class DxSafeObject {
	public:
		T* value = NULL;
		DxSafeObject(T* v) :value(v) {}
		operator T* () {
			return value;
		}
		~DxSafeObject() {
			if (value) {
				value->Release();
			}
		}
		T* operator->() {
			return value;
		}
	};

	class TextFormat {
	public:
		IDWriteTextFormat* value = NULL;
	public:
		TextFormat(const std::wstring& fontFamily, int fontSize, TextAlign textAlign) {
			float fh = (float)MulDiv((int)fontSize, Dpi, 72);
			g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fh, L"", &value);
#define __Top DWRITE_PARAGRAPH_ALIGNMENT_NEAR
#define	__Bottom DWRITE_PARAGRAPH_ALIGNMENT_FAR
#define	__Left DWRITE_TEXT_ALIGNMENT_LEADING
#define	__Right DWRITE_TEXT_ALIGNMENT_TRAILING
#define	__Middle DWRITE_PARAGRAPH_ALIGNMENT_CENTER
#define __Center DWRITE_TEXT_ALIGNMENT_CENTER
			do
			{
				if (textAlign == TextAlign::BottomCenter) {
					value->SetParagraphAlignment(__Bottom);
					value->SetTextAlignment(__Center);
					break;
				}
				if (textAlign == TextAlign::BottomLeft) {
					value->SetParagraphAlignment(__Bottom);
					value->SetTextAlignment(__Left);
					break;
				}
				if (textAlign == TextAlign::BottomRight) {
					value->SetParagraphAlignment(__Bottom);
					value->SetTextAlignment(__Right);
					break;
				}
				if (textAlign == TextAlign::MiddleCenter) {
					value->SetParagraphAlignment(__Middle);
					value->SetTextAlignment(__Center);
					break;
				}
				if (textAlign == TextAlign::MiddleLeft) {
					value->SetParagraphAlignment(__Middle);
					value->SetTextAlignment(__Left);
					break;
				}
				if (textAlign == TextAlign::MiddleRight) {
					value->SetParagraphAlignment(__Middle);
					value->SetTextAlignment(__Right);
					break;
				}
				if (textAlign == TextAlign::TopCenter) {
					value->SetParagraphAlignment(__Top);
					value->SetTextAlignment(__Center);
					break;
				}
				if (textAlign == TextAlign::TopLeft) {
					value->SetParagraphAlignment(__Top);
					value->SetTextAlignment(__Left);
					break;
				}
				if (textAlign == TextAlign::TopRight) {
					value->SetParagraphAlignment(__Top);
					value->SetTextAlignment(__Right);
					break;
				}
			} while (0);
#undef __Top 
#undef __Bottom 
#undef __Left 
#undef __Right 
#undef __Middle 
#undef __Center 
		}
		IDWriteTextFormat* operator->() {
			return value;
		}
		operator IDWriteTextFormat* () {
			return value;
		}
		virtual ~TextFormat() {
			if (value) {
				value->Release();
			}
		}
	};
	class TextLayout {
	public:
		IDWriteTextLayout* value = NULL;
	public:
		TextLayout(const std::wstring& text, __Size maxSize, TextFormat* pTextFormat) {
			g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), pTextFormat->value, (FLOAT)maxSize.Width, (FLOAT)maxSize.Height, &value);
		}
		__Point HitTestPoint(const __Point& pt, int& textPos, BOOL& isTrailingHit) {
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			//BOOL isTrailingHit;
			BOOL isInside;
			{
				FLOAT x = (FLOAT)pt.X, y = (FLOAT)pt.Y;
				value->HitTestPoint(
					(FLOAT)x,
					(FLOAT)y,
					&isTrailingHit,
					&isInside,
					&hitTestMetrics
				);
			}
			////绘制光标
			int posX = (int)(hitTestMetrics.left + 0.5);
			if (isTrailingHit) {//判断前侧还是尾侧
				posX += (int)(hitTestMetrics.width + 0.5);
			}
			textPos = hitTestMetrics.textPosition;
			return __Point{ posX,(int)(hitTestMetrics.top + 0.5) };
		}
		__Point HitTestTextPosition(int textPos, BOOL isTrailingHit) {
			DWRITE_HIT_TEST_METRICS hitTestMetrics;
			FLOAT X, Y;
			value->HitTestTextPosition(textPos, isTrailingHit, &X, &Y, &hitTestMetrics);
			return __Point((int)(X + 0.5), (int)(Y + 0.5));
		}
		__Size GetFontSize() {
			DWRITE_TEXT_METRICS textMetrics;
			value->GetMetrics(&textMetrics);
			D2D1_SIZE_F size = D2D1::SizeF(ceil(textMetrics.widthIncludingTrailingWhitespace), ceil(textMetrics.height));
			return  __Size{ (int)(size.width + 0.5) ,(int)((size.height / textMetrics.lineCount) + 0.5) };
		}
		IDWriteTextLayout* operator->() {
			return value;
		}
		operator IDWriteTextLayout* () {
			return value;
		}
		virtual ~TextLayout() {
			if (value) {
				value->Release();
			}
		}
	};

	class Geometry
	{
	public:
		bool Ref = false;
		ID2D1Geometry* rgn = NULL;
	protected:
		void Copy(const Geometry& _copy) {
			((Geometry&)(_copy)).Ref = true;
			this->rgn = ((Geometry&)(_copy)).rgn;
		}
	public:
		Geometry() {}
		Geometry(const Geometry& _copy) {
			Copy(_copy);
		}
		Geometry& operator =(const Geometry& _right) {
			Copy(_right);
			return *this;
		}
		Geometry(int x, int y, int width, int height) {
			D2D_RECT_F rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) };
			g_Direct2dFactory->CreateRectangleGeometry(rectF, (ID2D1RectangleGeometry**)&rgn);
		}
		/*	Geometry(const __Rect& rect) :Geometry(rect.X, rect.Y, rect.Width, rect.Height) {};*/
		Geometry(int x, int y, int width, int height, int _radius) {
			float radius = _radius / 2.0f;
			D2D1_ROUNDED_RECT rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) ,radius ,radius };
			g_Direct2dFactory->CreateRoundedRectangleGeometry(rectF, (ID2D1RoundedRectangleGeometry**)&rgn);
		}
		virtual ~Geometry() {
			if (rgn && !Ref) {
				rgn->Release();
			}
		}
	public:
		static void Combine(Geometry& out, const Geometry& a, const Geometry& b, D2D1_COMBINE_MODE COMBINE_MODE)
		{
			ID2D1PathGeometry* outPathGeometry = NULL;
			g_Direct2dFactory->CreatePathGeometry(&outPathGeometry);
			ID2D1GeometrySink* geometrySink = NULL;
			outPathGeometry->Open(&geometrySink);
			HRESULT ret = a.rgn->CombineWithGeometry(b.rgn, COMBINE_MODE, NULL, geometrySink);
			geometrySink->Close();
			if (out.rgn) {
				out.rgn->Release();
			}
			out.rgn = outPathGeometry;
			geometrySink->Release();
		}
		//两块区域取最大边界
		static void Union(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_UNION);
		}
		//两块区域有交集的部分
		static void Intersect(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_INTERSECT);
		}
		static void Xor(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_XOR);
		}
		static void Exclude(Geometry& out, const Geometry& a, const Geometry& b) {
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
	protected:
		void CreateFormStream(IStream* istram);
		void CreateFromFile(const std::wstring& file);
	public:
		void DecodeOfRender(ID2D1RenderTarget* render);
		DXImage(HBITMAP hBitmap);
		DXImage(IStream* istram);
		DXImage(const std::wstring& file);
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
		int Count = 0;
		ID2D1RenderTarget* d2dRender = NULL;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
		ID2D1SolidColorBrush* SolidColorBrush = NULL;
	protected:
		virtual void DrawBitmap(ID2D1Bitmap* d2dBitmap, const  __Rect& rect);

	public:
		Direct2DRender(HDC _dc, int Width = 0, int Height = 0);
		Direct2DRender(HWND hWnd, int Width = 0, int Height = 0);
		virtual ~Direct2DRender();
		void DrawRectangle(const  __Rect& rect, const  __Color& color, int width = 1, int radius = 0);
		void FillRectangle(const  __Rect& rect, const  __Color& color, int radius = 0);
		void DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  __Color& color, const  __Rect& rect, EzUI::TextAlign textAlign, bool underLine = false);
		void DrawTextLayout(const __Point&, IDWriteTextLayout* textLayout, const __Color& color);
		void DrawTextLayout(const __Point& pt, TextLayout* textLayout, const __Color& color) {
			DrawTextLayout(pt, textLayout->value, color);
		}
		void DrawLine(const  __Color& color, const  __Point& A, const  __Point& B, int width = 1);
		void DrawImage(IImage* image, const  __Rect& destRect, const __Rect& srcRect);
		void DrawImage(IImage* image, const  __Rect& rect, const EzUI::ImageSizeMode& imageSizeMode = EzUI::ImageSizeMode::Zoom, const EzUI::Margin& margin = 0);
		void DrawGeometry(ID2D1Geometry* geometry, const  __Color& color, int width = 1);
		void FillGeometry(ID2D1Geometry* geometry, const  __Color& color);
		void DrawGeometry(const Geometry& geometry, const  __Color& color, int width = 1) {
			DrawGeometry(geometry.rgn, color);
		}
		void FillGeometry(const Geometry& geometry, const  __Color& color) {
			FillGeometry(geometry.rgn, color);
		}
		void PushLayer(const Geometry& Geometry);
		void PopLayer();
		void PushAxisAlignedClip(const __Rect& rectBounds);
		void PopAxisAlignedClip();
		void Flush();
		ID2D1SolidColorBrush* GetSolidColorBrush(const __Color& _color);
		void BeginDraw();
		void EndDraw();
	};
	UI_EXPORT void RenderInitialize();//全局初始化direct2d
	UI_EXPORT void RenderUnInitialize();//释放direct2d

	using Painter = Direct2DRender;
};
#endif