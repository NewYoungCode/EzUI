#pragma once
#include "UIDef.h"
#if USED_GDIPLUS
#include <string>
#include <list>
#include <map>
#include <Windows.h>
#include <Windowsx.h>
#include <comdef.h>
#include <GdiPlus.h>
#include <GdiplusTypes.h>
#include <gdiplusimagecodec.h>
#include <gdiplusimaging.h>
#include <gdiplusimageattributes.h>
#include <shlwapi.h>
#include <usp10.h>
#pragma comment (lib, "usp10.lib")
#include "RenderType.h"
namespace EzUI {

#define __Rect RenderType::Rect
#define __RectF RenderType::RectF
#define __Color RenderType::Color
#define __Point RenderType::Point
#define __PointF RenderType::PointF
#define __ARGB RenderType::ARGB
#define __Size RenderType::Size
#define __SizeF RenderType::SizeF

	class TextFormat {
	public:
		std::wstring fontFamilly; int fontSize; TextAlign textAlign;
	public:
		TextFormat(const std::wstring& fontFamilly, int fontSize, TextAlign textAlign)
		{
			this->fontFamilly = fontFamilly;
			this->fontSize = fontSize;
			this->textAlign = textAlign;
		}
	};
	class TextLayout {
	public:
		SCRIPT_STRING_ANALYSIS m_Analysis = NULL;	//字符串分析结果;
		std::wstring text;
		__Size maxSize;
		TextFormat* textFormat;
		SCRIPT_STATE ScriptState{ 0 };
		SCRIPT_CONTROL ScriptControl{ 0 };
	public:
		TextLayout(const std::wstring& _text, __Size _maxSize, TextFormat* _textFormat) {
			text = _text;
			maxSize = _maxSize;
			textFormat = _textFormat;
			::ScriptApplyDigitSubstitution(NULL, &ScriptControl, &ScriptState);
			HDC m_hDc = ::GetDC(NULL);
			HRESULT hr = ::ScriptStringAnalyse(m_hDc,
				text.c_str(),
				text.length() + 1,
				text.length() * 3 / 2 + 16,
				-1,
				SSA_GLYPHS | SSA_BREAK | SSA_FALLBACK | SSA_LINK,
				0,
				&ScriptControl,
				&ScriptState,
				NULL,
				NULL,
				NULL,
				&m_Analysis);
			::ReleaseDC(NULL, m_hDc);
		}
		__Point HitTestTextPosition(int textPos, BOOL A_isTrailingHit) {
			return { 0,0 };
		}
		__Point HitTestPoint(__Point point_Start, int& A_TextPos, BOOL& A_isTrailingHit) {
			::ScriptStringXtoCP(m_Analysis, point_Start.X, &A_TextPos, &A_isTrailingHit);
			int outX = 0;
			::ScriptStringCPtoX(m_Analysis, A_TextPos, A_isTrailingHit, &outX);
			return { outX,0 };
		}
		__Size GetFontSize() {
			const SIZE* sz = ::ScriptString_pSize(m_Analysis);
			return __Size{ sz->cx,sz->cy };
		}
		virtual ~TextLayout() {
			if (m_Analysis) {
				ScriptStringFree(&m_Analysis);
			}
		}
	};

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
		void DrawString(const std::wstring& text, const Gdiplus::Font* font, const __Color& color, const __RectF& rect, TextAlign textAlign, bool underLine = false);
		void CreateFormat(TextAlign textAlign, Gdiplus::StringFormat& outStrFormat);
	public:
		std::list<__Rect*> Layers;
		int OffsetX = 0;
		int OffsetY = 0;
	public:
		//static Region* IntersectRound(const __Rect& clientRect, int r, const __Rect& _ClipRect) {
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
		////圆角控件 使用纹理的方式 (这样做是为了控件内部无论怎么绘制都不会超出圆角部分) 无抗锯齿
		//	HRGN clientRgn = ::CreateRoundRectRgn(clientRect.X, clientRect.Y, clientRect.GetRight(), clientRect.GetBottom(), r, r);
		//	HRGN clipRectRgn = ::CreateRectRgn(_ClipRect.X, _ClipRect.Y, _ClipRect.GetRight(), _ClipRect.GetBottom());
		//	::IntersectRgn(clipRectRgn, clientRgn, clipRectRgn);
		//	DeleteRgn(clientRgn);
		//	layer.RGN = clipRectRgn;

		Gdiplus::Rect ToRect(const __Rect& _rect) {
			return Gdiplus::Rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
		}

		Gdiplus::Color ToColor(const __Color& color) {
			return  Gdiplus::Color(color.GetValue());
		}

		Gdiplus::Graphics* base = NULL;
		std::map<std::wstring, HFONT> CacheFont;

		GdiplusRender(HDC hdc, int Width = 0, int Height = 0);
		GdiplusRender(HWND hWnd);
		GdiplusRender(Gdiplus::Image* image);
		virtual ~GdiplusRender();
		void DrawRectangle(const __Rect& rect, const __Color& color, int width = 1, int radius = 0);
		void FillRectangle(const __Rect& rect, const __Color& color, int radius = 0);
		void DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const __Color& color, const __Rect& rect, TextAlign textAlign, bool underLine = false);
		void DrawTextLayout(const __Point& pt, TextLayout* textLayout, const __Color& color);
		void MeasureString(const std::wstring& _text, const std::wstring& fontf, int fontSize, __RectF& outBox);
		void PushLayer(const __Rect& rect, ClipMode clipMode = ClipMode::Valid);
		void PopLayer();
		HFONT CreateSafeFont(const std::wstring& fontFamily, int fontSize, HDC DC, bool lfUnderline = false);
		void DrawLine(const __Color& color, const __Point& A, const __Point& B, int width = 1);
		void DrawImage(IImage* image, const __Rect& destRect, const __Rect& srcRect);
		void DrawImage(IImage* image, const __Rect& rect, const ImageSizeMode& imageSizeMode = ImageSizeMode::Zoom, const Margin& margin = 0);
		void SaveImage(const WCHAR* format, const WCHAR* fileName, const __Size& size);
		void BeginDraw();
		void EndDraw();
	};

	UI_EXPORT void RenderInitialize();
	UI_EXPORT void RenderUnInitialize();
	UI_EXPORT void HighQualityMode(Gdiplus::Graphics* graphics);
	UI_EXPORT void CreateRectangle(Gdiplus::GraphicsPath& path, const __Rect& rect, int radius);//申明
	UI_EXPORT int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	UI_EXPORT BOOL SaveHDCToFile(HDC hDC, LPRECT lpRect, const WCHAR* format, const WCHAR* filename);
	UI_EXPORT BOOL SaveHDCToFile(HDC hDC, const __Rect& rect, const std::wstring& fileName);
	UI_EXPORT GdiplusImage* ClipImage(GdiplusImage* img, const __Size& sz, int _radius);

};

#endif