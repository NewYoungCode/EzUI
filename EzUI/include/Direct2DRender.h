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
	public:
		IDWriteTextFormat* value = NULL;
	public:
		TextFormat(const std::wstring& fontFamily, int fontSize, TextAlign textAlign);
		IDWriteTextFormat* operator->();
		operator IDWriteTextFormat* ();
		virtual ~TextFormat();
	};
	class TextLayout {
	private:
		TextLayout() = delete;
		TextLayout(const TextLayout& _copy) = delete;
		TextLayout& operator=(const TextLayout& _copy) = delete;
	public:
		IDWriteTextLayout* value = NULL;
	public:
		TextLayout(const std::wstring& text, __Size maxSize, TextFormat* pTextFormat);
		__Point HitTestPoint(const __Point& pt, int& textPos, BOOL& isTrailingHit);
		__Point HitTestTextPosition(int textPos, BOOL isTrailingHit);
		__Size GetFontSize();
		IDWriteTextLayout* operator->();
		operator IDWriteTextLayout* ();
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
	protected:
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
	};
	using Painter = ID2D1RenderTarget;
};

namespace EzUI {
	UI_EXPORT void RenderInitialize();//全局初始化direct2d
	UI_EXPORT void RenderUnInitialize();//释放direct2d
	UI_EXPORT ID2D1DCRenderTarget* CreateRender(HDC _dc,int x,int y, int Width, int Height);
	UI_EXPORT void ReleaseRender(ID2D1DCRenderTarget* d2dRender);
	UI_EXPORT void FillRectangle(ID2D1RenderTarget* d2dRender, const __Rect& _rect, const __Color& color, int _radius = 0);
	UI_EXPORT void DrawRectangle(ID2D1RenderTarget* d2dRender, const  __Rect& _rect, const  __Color& color, int width = 1, int _radius = 0);
	UI_EXPORT void SetTransform(ID2D1RenderTarget* d2dRender, int xOffset, int yOffset);
	UI_EXPORT void DrawLine(ID2D1RenderTarget* d2dRender, const __Color& color, const __Point& _A, const __Point& _B, int width);
	//正规矩形速度快!!! 不支持异形抗锯齿裁剪
	UI_EXPORT void PushAxisAlignedClip(ID2D1RenderTarget* d2dRender, const __Rect& rectBounds);
	UI_EXPORT void PopAxisAlignedClip(ID2D1RenderTarget* d2dRender);
	//layer巨tm的耗性能!!! 但是可以异形抗锯齿裁剪
	UI_EXPORT void PushLayer(ID2D1RenderTarget* d2dRender, const Geometry& dxGeometry);
	UI_EXPORT void PopLayer(ID2D1RenderTarget* d2dRender);//弹出最后一个裁剪
	UI_EXPORT void DrawImage(ID2D1RenderTarget* d2dRender, IImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Margin& margin = 0);
	UI_EXPORT void DrawTextLayout(ID2D1RenderTarget* d2dRender, const __Point& startLacation, IDWriteTextLayout* textLayout, const __Color& color);
	UI_EXPORT void DrawString(ID2D1RenderTarget* d2dRender, const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  __Color& color, const  __Rect& _rect, EzUI::TextAlign textAlign, bool underLine = false);
};
#endif