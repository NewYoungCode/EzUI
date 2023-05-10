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
	class Font {
	private:
		Font() = delete;
		std::wstring fontFamily;
		int fontSize = 0;
		IDWriteTextFormat* value = NULL;
		void Copy(const Font& _copy);
	public:
		bool Ref = false;
		Font(const Font& _copy);
		Font(const std::wstring& fontFamily, int fontSize);
		const int& GetFontSize()const;
		const std::wstring& GetFontFamily()const;
		IDWriteTextFormat* Get() const;
		Font& operator=(const Font& _copy);
		bool operator==(const Font& _right);
		virtual ~Font();
	};
	class TextLayout {
	private:
		TextLayout() = delete;
		TextLayout(const TextLayout& _copy) = delete;
		TextLayout& operator=(const TextLayout& _copy) = delete;
		IDWriteTextLayout* value = NULL;
	public:
		TextLayout(const std::wstring& text, const Font& pTextFormat, TextAlign textAlgin = TextAlign::TopLeft, __Size maxSize = __Size{ 16777216,16777216 });
		__Point HitTestPoint(const __Point& pt, int& textPos, BOOL& isTrailingHit);
		__Point HitTestTextPosition(int textPos, BOOL isTrailingHit);
		__Size GetFontBox();
		void SetTextAlign(TextAlign textAlign);
		IDWriteTextLayout* Get() const;
		void SetUnderline(size_t pos = 0, size_t count = 0);
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

	class DXPath {
	private:
		ID2D1GeometrySink* pSink = NULL;
		ID2D1PathGeometry* pathGeometry = NULL;
		bool isBegin = false;
	public:
		DXPath() {
			D2D::g_Direct2dFactory->CreatePathGeometry(&pathGeometry);
			pathGeometry->Open(&pSink);
		}
		void AddRectangle(const __Rect& rect) {
			if (!isBegin) {
				pSink->BeginFigure({ (FLOAT)rect.GetLeft(),(FLOAT)rect.GetTop() }, D2D1_FIGURE_BEGIN_FILLED);
				isBegin = true;
			}
			else {
				pSink->AddLine({ (FLOAT)rect.GetLeft(),(FLOAT)rect.GetTop() });
			}
			pSink->AddLine({ (FLOAT)rect.GetRight(),(FLOAT)rect.GetTop() });
			pSink->AddLine({ (FLOAT)rect.GetRight(),(FLOAT)rect.GetBottom() });
			pSink->AddLine({ (FLOAT)rect.GetLeft(),(FLOAT)rect.GetBottom() });
		}
		void AddArc(const __Rect& rect, int startAngle, int sweepAngle) {

		}
		void AddLine(const __Rect& rect) {

		}
		void CloseFigure() {
			pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			pSink->Close();
		}
		virtual ~DXPath() {
			if (pathGeometry) {
				pathGeometry->Release();
			}
			if (pSink) {
				pSink->Release();
			}
		}
		ID2D1PathGeometry* Get()const {
			return pathGeometry;
		}
		ID2D1GeometrySink* operator ->() {
			return pSink;
		}
		ID2D1PathGeometry* operator *() {
			return pathGeometry;
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
		DXImage(UINT width, UINT height);
		ID2D1Bitmap* Get();
		IWICBitmap* GetIWICBitmap();
		UINT GetWidth();
		UINT GetHeight();
		virtual size_t NextFrame()override;
		DXImage() {}
		virtual ~DXImage();
	};

	class Bezier {
	public:
		__Point point1;
		__Point point2;
		__Point point3;
	};
};

namespace EzUI {
	UI_EXPORT void RenderInitialize();//全局初始化direct2d
	UI_EXPORT void RenderUnInitialize();//释放direct2d

	enum class StrokeStyle
	{
		Solid,//实线
		Dash//虚线
	};

	class UI_EXPORT DXRender {
	private:
		std::list<bool> layers;
		ID2D1DCRenderTarget* render = NULL;
		ID2D1SolidColorBrush* brush = NULL;
		Font* font = NULL;
		ID2D1StrokeStyle* pStrokeStyle = NULL;
	public:
		ID2D1SolidColorBrush* GetBrush();
		ID2D1StrokeStyle* GetStrokeStyle();
	public:
		DXRender(DXImage* dxImage);
		DXRender(HDC dc, int x, int y, int width, int height);//创建dx绘图对象
		virtual ~DXRender();
		void SetFont(const std::wstring& fontFamily, int fontSize);//必须先调用
		void SetFont(const Font& _copy_font);//必须先调用
		void SetColor(const __Color& color);//会之前必须调用
		void SetStrokeStyle(StrokeStyle strokeStyle = StrokeStyle::Solid, int dashWidth = 3);//设置样式 虚线/实线
		void DrawString(const TextLayout& textLayout, __Point = { 0,0 });//根据已有的布局绘制文字
		void DrawString(const std::wstring& text, const  __Rect& _rect, EzUI::TextAlign textAlign);//绘制文字
		void DrawLine(const __Point& _A, const __Point& _B, int width);//绘制一条线
		void DrawRectangle(const  __Rect& _rect, int _radius = 0, int width = 1);//绘制矩形
		void FillRectangle(const __Rect& _rect, int _radius = 0);//填充矩形
		void PushLayer(const __Rect& rectBounds);//速度较快
		void PushLayer(const Geometry& dxGeometry);//比较耗性能,但是可以异形抗锯齿裁剪
		void PopLayer();//弹出最后一个裁剪
		void DrawImage(DXImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode= ImageSizeMode::StretchImage, const EzUI::Margin& margin = 0);//绘制图像
		void SetTransform(int xOffset, int yOffset, int angle = 0);//对画布进行旋转和偏移
		void DrawBezier(const __Point& startPoint, const Bezier& points, int width = 1);//贝塞尔线
		void DrawBezier(const __Point& startPoint, std::list<Bezier>& points, int width = 1);//贝塞尔线
		void DrawEllipse(const __Point& point, int radiusX, int radiusY, int width = 1);
		void FillEllipse(const __Point& point, int radiusX, int radiusY);
		void DrawArc(const __Rect& rect, int startAngle, int sweepAngle, int width = 1);//未实现
		void DrawArc(const __Point& point1, const __Point& point2, const __Point& point3, int width = 1);//绘制弧线 未实现
		void DrawPath(const DXPath& path, int width = 1);//绘制path 未实现
		void FillPath(const DXPath& path);//填充Path 未实现
		ID2D1DCRenderTarget* Get();//获取原生DX对象
	};
};
#endif