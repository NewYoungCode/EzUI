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

	namespace D2D {
		extern ID2D1Factory* g_Direct2dFactory;
		extern IDWriteFactory* g_WriteFactory;
		extern IWICImagingFactory* g_ImageFactory;
	}
	class TextFormat {
	private:
		TextFormat() = delete;
		TextFormat(const TextFormat& _copy) = delete;
		TextFormat& operator=(const TextFormat& _copy) = delete;
		IDWriteTextFormat* value = NULL;
	public:
		TextFormat(const std::wstring& fontFamily, int fontSize);
		IDWriteTextFormat* operator->();
		operator IDWriteTextFormat* ();
		IDWriteTextFormat* Get() {
			return value;
		}
		virtual ~TextFormat();
	};
	class TextLayout {
	private:
		TextLayout() = delete;
		TextLayout(const TextLayout& _copy) = delete;
		TextLayout& operator=(const TextLayout& _copy) = delete;
		IDWriteTextLayout* value = NULL;
	public:
		TextLayout(const std::wstring& text, TextFormat* pTextFormat, TextAlign textAlgin = TextAlign::TopLeft, __Size maxSize = __Size{ 16777216,16777216 });
		__Point HitTestPoint(const __Point& pt, int& textPos, BOOL& isTrailingHit);
		__Point HitTestTextPosition(int textPos, BOOL isTrailingHit);
		__Size GetFontSize();
		IDWriteTextLayout* operator->();
		operator IDWriteTextLayout* ();
		IDWriteTextLayout* Get() const {
			return value;
		}
		virtual ~TextLayout();
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
			D2D::g_Direct2dFactory->CreateRectangleGeometry(rectF, (ID2D1RectangleGeometry**)&rgn);
		}
		/*	Geometry(const __Rect& rect) :Geometry(rect.X, rect.Y, rect.Width, rect.Height) {};*/
		Geometry(int x, int y, int width, int height, int _radius) {
			float radius = _radius / 2.0f;
			D2D1_ROUNDED_RECT rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) ,radius ,radius };
			D2D::g_Direct2dFactory->CreateRoundedRectangleGeometry(rectF, (ID2D1RoundedRectangleGeometry**)&rgn);
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
			D2D::g_Direct2dFactory->CreatePathGeometry(&outPathGeometry);
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
	public:
		void CreateFormStream(IStream* istram);
		void CreateFromFile(const std::wstring& file);
		void Init();
	public:
		void DecodeOfRender(ID2D1RenderTarget* render);
		DXImage(HBITMAP hBitmap);
		DXImage(IStream* istram);
		DXImage(const std::wstring& file);
		UINT GetWidth();
		UINT GetHeight();
		virtual size_t NextFrame()override;
		DXImage() {}
		virtual ~DXImage();
	public:
		static DXImage* FromFile() {

		}
	};
};

namespace EzUI {
	UI_EXPORT void RenderInitialize();//全局初始化direct2d
	UI_EXPORT void RenderUnInitialize();//释放direct2d

	class UI_EXPORT D2DRender {
	private:
		std::list<bool> layers;
		ID2D1DCRenderTarget* render = NULL;
		ID2D1SolidColorBrush* brush = NULL;
		TextFormat* textFormat = NULL;
	private:
		ID2D1SolidColorBrush* GetBrush();
	public:
		D2DRender(HDC dc, int x, int y, int width, int height);
		virtual ~D2DRender();
		void SetFont(const std::wstring& fontFamily, int fontSize);//必须先调用
		void SetColor(const __Color& color);//会之前必须调用
		void DrawString(const TextLayout& textLayout, __Point = { 0,0 });//根据已有的布局绘制文字
		void DrawString(const std::wstring& text, const  __Rect& _rect, EzUI::TextAlign textAlign, size_t underLinePos1 = 0, size_t underLineCount = 0);
		void DrawLine(const __Point& _A, const __Point& _B, int width);//绘制一条线
		void DrawRectangle(const  __Rect& _rect, int _radius = 0, int width = 1);//绘制矩形
		void FillRectangle(const __Rect& _rect, int _radius = 0);//填充矩形
		void PushLayer(const __Rect& rectBounds);//速度较快
		void PushLayer(const Geometry& dxGeometry);//比较耗性能!!! 但是可以异形抗锯齿裁剪
		void PopLayer();//弹出最后一个裁剪
		void DrawImage(IImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Margin& margin = 0);//绘制图像
		void SetTransform(int xOffset, int yOffset, int angle = 0);//对画布进行旋转和偏移
	};
	using Painter = D2DRender;
};
#endif