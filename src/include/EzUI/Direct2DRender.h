#pragma once
#include "UIDef.h"
#if USED_DIRECT2D
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
#define __MAXFLOAT 16777216
	namespace D2D {
		extern UI_VAR_EXPORT ID2D1Factory* g_Direct2dFactory;
		extern UI_VAR_EXPORT IDWriteFactory* g_WriteFactory;
		extern UI_VAR_EXPORT IWICImagingFactory* g_ImageFactory;
	}
	UI_EXPORT void RenderInitialize();//全局初始化direct2d
	UI_EXPORT void RenderUnInitialize();//释放direct2d
	UI_EXPORT float GetMaxRadius(float width, float height, float _radius);//获取最大半径 用于自动适应border-radius属性
	class UI_EXPORT Font {
	private:
		Font() = delete;
		std::wstring fontFamily;
		float fontSize = 0;
		IDWriteTextFormat* value = NULL;
		void Copy(const Font& _copy);
	public:
		bool Ref = false;
		Font(const Font& _copy);
		Font(const std::wstring& fontFamily, const float& fontSize);
		const float& GetFontSize()const;
		const std::wstring& GetFontFamily()const;
		IDWriteTextFormat* Get() const;
		Font& operator=(const Font& _copy);
		bool operator==(const Font& _right);
		virtual ~Font();
	};

	//文本命中测试数据
	class UI_EXPORT HitTestMetrics {
	public:
		size_t Length;
		size_t TextPos;//命中的下标
		RectF FontBox;//文字的矩形位置
		bool IsTrailingHit;//命中位置是否在尾部
	public:
		Rect GetCare() {
			float x = FontBox.X;
			if (IsTrailingHit) {
				x += FontBox.Width;
			}
			float y = FontBox.Y;
			return Rect((int)x, (int)y, 1, (int)(FontBox.Height + 0.5));
		}
		int GetFontHeight() {
			return (int)(FontBox.Height + 0.5);
		}
	};

	class UI_EXPORT TextLayout {
	private:
		TextLayout() = delete;
		TextLayout(const TextLayout& _copy) = delete;
		TextLayout& operator=(const TextLayout& _copy) = delete;
		IDWriteTextLayout* value = NULL;
		void SetTextAlign(TextAlign textAlign);
		std::wstring fontFamily;
		DWRITE_TEXT_METRICS textMetrics{ 0 };
		float fontSize = 0;
	public:
		void GetMetrics();
		TextLayout(const std::wstring& text, const Font& font, const SizeF &maxSize = SizeF{ __MAXFLOAT,__MAXFLOAT }, TextAlign textAlgin = TextAlign::TopLeft);
		Point HitTestPoint(const Point& pt, int* outTextPos, BOOL* outIsTrailingHit, int* fontHeight);
		void HitTestPoint(const Point& pt, HitTestMetrics* hitTestMetrics);//根据坐标执行命中测试
		Point HitTestTextPosition(int textPos, BOOL isTrailingHit);//根据文字下标执行命中测试
		const std::wstring& GetFontFamily();
		const float& GetFontSize();
		int Width();
		int Height();
		Size GetFontBox();//获取文字绘制的时候占用多少行
		Rect GetLineBox(int lineIndex);
		int GetFontHeight();//获取字体高度
		int GetLineCount();//获取一共有多少行
		IDWriteTextLayout* Get() const;
		void SetUnderline(size_t pos = 0, size_t count = 0);
		virtual ~TextLayout();
	};
	class UI_EXPORT Geometry
	{
	protected:
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
		Geometry(int x, int y, int width, int height);
		Geometry(int x, int y, int width, int height, int _radius);
		Geometry(const Rect& _rect, int topLeftRadius, int topRightRadius, int bottomRightRadius, int bottomLeftRadius);
		ID2D1Geometry* Get() const {
			return rgn;
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

	class UI_EXPORT DXPath {
	private:
		ID2D1GeometrySink* pSink = NULL;
		ID2D1PathGeometry* pathGeometry = NULL;
		bool isBegin = false;
	public:
		DXPath() {
			D2D::g_Direct2dFactory->CreatePathGeometry(&pathGeometry);
			pathGeometry->Open(&pSink);
		}
		void AddRectangle(const Rect& rect) {
			if (!isBegin) {
				pSink->BeginFigure({ (float)rect.GetLeft(),(float)rect.GetTop() }, D2D1_FIGURE_BEGIN_FILLED);
				isBegin = true;
			}
			else {
				pSink->AddLine({ (float)rect.GetLeft(),(float)rect.GetTop() });
			}
			pSink->AddLine({ (float)rect.GetRight(),(float)rect.GetTop() });
			pSink->AddLine({ (float)rect.GetRight(),(float)rect.GetBottom() });
			pSink->AddLine({ (float)rect.GetLeft(),(float)rect.GetBottom() });
		}
		void AddArc(const Rect& rect, int startAngle, int sweepAngle) {

		}
		void AddLine(const Rect& rect) {

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
		int Width = 0;
		int Height = 0;
		ID2D1Bitmap* d2dBitmap = NULL;
	private:
		void CreateFormStream(IStream* istram);
		void CreateFromFile(const std::wstring& file);
		void Init();
	public:
		bool Visible = true;
		void DecodeOfRender(ID2D1RenderTarget* render);
		DXImage(HBITMAP hBitmap);
		DXImage(IStream* istram);
		DXImage(const std::wstring& file);
		DXImage(int width, int height);
		DXImage(const void* data, size_t count);
		ID2D1Bitmap* Get();
		IWICBitmap* GetIWICBitmap();
		int GetWidth();
		int GetHeight();
		virtual size_t NextFrame()override;
		DXImage* Clone();
		DXImage() {}
		virtual ~DXImage();
	};

	class Bezier {
	public:
		Point point1;
		Point point2;
		Point point3;
	};
};

namespace EzUI {

	enum class StrokeStyle
	{
		Solid,//实线
		Dash//虚线
	};

	class UI_EXPORT DXRender {
	private:
		ID2D1DCRenderTarget* render = NULL;
		ID2D1SolidColorBrush* brush = NULL;
		Font* font = NULL;
		ID2D1StrokeStyle* pStrokeStyle = NULL;
		Point Offset;
		PointF RotatePoint;
		float Angle = 0;
	public:
		ID2D1SolidColorBrush* GetBrush();
		ID2D1StrokeStyle* GetStrokeStyle();
	public:
		DXRender(DXImage* dxImage);
		DXRender(HDC dc, int x, int y, int width, int height);//创建dx绘图对象
		virtual ~DXRender();
		void SetFont(const std::wstring& fontFamily, const float& fontSize);//必须先调用
		void SetFont(const Font& _copy_font);//必须先调用
		void SetColor(const __EzUI__Color& color);//会之前必须调用
		void SetStrokeStyle(StrokeStyle strokeStyle = StrokeStyle::Solid, float dashWidth = 3);//设置样式 虚线/实线
		void DrawTextLayout(const TextLayout& textLayout, const PointF & = { 0,0 });//根据已有的布局绘制文字
		void DrawString(const std::wstring& text, const RectF& _rect, EzUI::TextAlign textAlign);//绘制文字
		void DrawLine(const PointF& _A, const  PointF& _B, float width = 1);//绘制一条线
		void DrawRectangle(const RectF& _rect, float _radius = 0, float width = 1);//绘制矩形
		void FillRectangle(const RectF& _rect, float _radius = 0);
		//填充矩形
		void PushLayer(const Geometry& dxGeometry);
		void PopLayer();
		void PushAxisAlignedClip(const RectF& rectBounds);
		void PopAxisAlignedClip();
		void SetTransform(float offsetX, float offsetY);//对画布进行旋转和偏移
		void SetTransform(float startX, float startY, float angle);//设置旋转起始点与旋转角度
		void SetTransform(float offsetX, float offsetY, float startX, float startY, float angle);
		void DrawImage(DXImage* _image, const  RectF& tagRect, float opacity = 1);//绘制图像
		void DrawBezier(const PointF& startPoint, const Bezier& points, float width = 1);//贝塞尔线
		void DrawBezier(const PointF& startPoint, std::list<Bezier>& points, float width = 1);//贝塞尔线
		void DrawEllipse(const PointF& point, float radiusX, float radiusY, float width = 1);
		void FillEllipse(const PointF& point, float radiusX, float radiusY);
		void DrawPoint(const PointF& pt);
		void DrawArc(const RectF& rect, float startAngle, float sweepAngle, float width = 1);//未实现
		void DrawArc(const PointF& point1, const  PointF& point2, const PointF& point3, float width = 1);
		void DrawGeometry(ID2D1Geometry* path, float width);
		void FillGeometry(ID2D1Geometry* path);
		void Flush();
		//绘制弧线 未实现
		void DrawPath(const DXPath& path, float width = 1);//绘制path 未实现
		void FillPath(const DXPath& path);//填充Path 未实现
		ID2D1DCRenderTarget* Get();//获取原生DX对象
	};
};
#endif