#pragma once
#include "UIDef.h"
#if USED_DIRECT2D
#ifndef UI_EXPORT
#define UI_EXPORT 
#endif

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include "RenderTypes.h"

namespace ezui {
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
		std::wstring m_fontFamily;
		float m_fontSize = 0;
		IDWriteTextFormat* m_value = NULL;
		bool m_ref = false;
		void Copy(const Font& _copy);
	public:
		Font(const Font& _copy);
		Font(const std::wstring& fontFamily, float fontSize);
		float GetFontSize()const;
		const std::wstring& GetFontFamily()const;
		IDWriteTextFormat* Get() const;
		Font& operator=(const Font& _copy);
		bool operator==(const Font& _right);
		virtual ~Font();
	};

	//文本命中测试数据
	class UI_EXPORT HitTestMetrics {
	public:
		int_t Length;
		int_t TextPos;//命中的下标
		RectF FontBox;//文字的矩形位置
		bool IsTrailingHit;//命中位置是否在尾部
	public:
		Rect GetCare() {
			float x = FontBox.X;
			if (IsTrailingHit) {
				x += FontBox.Width;
			}
			float y = FontBox.Y;
			return Rect(x, y, 1, (FontBox.Height + 0.5));
		}
		int_t GetFontHeight() {
			return FontBox.Height + 0.5;
		}
	};

	class UI_EXPORT TextLayout {
	private:
		TextLayout() = delete;
		TextLayout(const TextLayout& rightValue) = delete;
		TextLayout& operator=(const TextLayout& rightValue) = delete;
		IDWriteTextLayout* value = NULL;
		void SetTextAlign(TextAlign textAlign);
		std::wstring m_fontFamily;
		DWRITE_TEXT_METRICS m_textMetrics{ 0 };
		float m_fontSize = 0;
	public:
		void GetMetrics();
		TextLayout(const std::wstring& text, const Font& font, const SizeF& maxSize = SizeF{ __MAXFLOAT,__MAXFLOAT }, TextAlign textAlgin = TextAlign::TopLeft);
		Point HitTestPoint(const Point& pt, int_t* outTextPos, BOOL* outIsTrailingHit, int_t* fontHeight);
		void HitTestPoint(const Point& pt, HitTestMetrics* hitTestMetrics);//根据坐标执行命中测试
		Point HitTestTextPosition(int_t textPos, BOOL isTrailingHit);//根据文字下标执行命中测试
		const std::wstring& GetFontFamily();
		float GetFontSize();
		int_t Width();
		int_t Height();
		Size GetFontBox();//获取文字绘制的时候占用多少行
		Rect GetLineBox(int_t lineIndex);
		int_t GetFontHeight();//获取字体高度
		int_t GetLineCount();//获取一共有多少行
		IDWriteTextLayout* Get() const;
		void SetUnderline(size_t pos = 0, size_t count = 0);
		virtual ~TextLayout();
	};
	class UI_EXPORT Geometry
	{
	protected:
		bool m_ref = false;
		ID2D1Geometry* m_rgn = NULL;
	protected:
		void Copy(const Geometry& _copy) {
			((Geometry&)(_copy)).m_ref = true;
			this->m_rgn = ((Geometry&)(_copy)).m_rgn;
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
		Geometry(float x, float y, float width, float height);
		Geometry(float x, float y, float width, float height, float _radius);
		Geometry(const RectF& _rect, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius);
		ID2D1Geometry* Get() const {
			return m_rgn;
		}
		virtual ~Geometry() {
			if (m_rgn && !m_ref) {
				m_rgn->Release();
			}
		}
	public:
		static void Combine(Geometry& out, const Geometry& a, const Geometry& b, D2D1_COMBINE_MODE COMBINE_MODE)
		{
			ID2D1PathGeometry* outPathGeometry = NULL;
			D2D::g_Direct2dFactory->CreatePathGeometry(&outPathGeometry);
			ID2D1GeometrySink* geometrySink = NULL;
			outPathGeometry->Open(&geometrySink);
			HRESULT ret = a.m_rgn->CombineWithGeometry(b.m_rgn, COMBINE_MODE, NULL, geometrySink);
			geometrySink->Close();
			if (out.m_rgn) {
				out.m_rgn->Release();
			}
			out.m_rgn = outPathGeometry;
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
		ID2D1GeometrySink* m_pSink = NULL;
		ID2D1PathGeometry* m_pathGeometry = NULL;
		bool m_isBegin = false;
	public:
		DXPath() {
			D2D::g_Direct2dFactory->CreatePathGeometry(&m_pathGeometry);
			m_pathGeometry->Open(&m_pSink);
		}
		void AddRectangle(const Rect& rect) {
			if (!m_isBegin) {
				m_pSink->BeginFigure({ (float)rect.GetLeft(),(float)rect.GetTop() }, D2D1_FIGURE_BEGIN_FILLED);
				m_isBegin = true;
			}
			else {
				m_pSink->AddLine({ (float)rect.GetLeft(),(float)rect.GetTop() });
			}
			m_pSink->AddLine({ (float)rect.GetRight(),(float)rect.GetTop() });
			m_pSink->AddLine({ (float)rect.GetRight(),(float)rect.GetBottom() });
			m_pSink->AddLine({ (float)rect.GetLeft(),(float)rect.GetBottom() });
		}
		void AddArc(const Rect& rect, int_t startAngle, int_t sweepAngle) {

		}
		void AddLine(const Rect& rect) {

		}
		void CloseFigure() {
			m_pSink->EndFigure(D2D1_FIGURE_END_OPEN);
			m_pSink->Close();
		}
		virtual ~DXPath() {
			if (m_pathGeometry) {
				m_pathGeometry->Release();
			}
			if (m_pSink) {
				m_pSink->Release();
			}
		}
		ID2D1PathGeometry* Get()const {
			return m_pathGeometry;
		}
		ID2D1GeometrySink* operator ->() {
			return m_pSink;
		}
		ID2D1PathGeometry* operator *() {
			return m_pathGeometry;
		}
	};

	class UI_EXPORT DXImage : public IImage {
	protected:
		IWICBitmapDecoder* m_bitmapdecoder = NULL;
		IWICBitmapFrameDecode* m_pframe = NULL;
		IWICFormatConverter* m_fmtcovter = NULL;//从文件加载
		IWICBitmap* m_bitMap = NULL;//从HBITMAP中加载
		int_t m_width = 0;
		int_t m_height = 0;
		ID2D1Bitmap* m_d2dBitmap = NULL;
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
		DXImage(int_t width, int_t height);
		DXImage(const void* data, size_t count);
		ID2D1Bitmap* Get();
		IWICBitmap* GetIWICBitmap();
		int_t GetWidth();
		int_t GetHeight();
		virtual WORD NextFrame()override;
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

namespace ezui {

	class UI_EXPORT DXRender {
	private:
		ID2D1DCRenderTarget* m_render = NULL;
		ID2D1SolidColorBrush* m_brush = NULL;
		Font* m_font = NULL;
		ID2D1StrokeStyle* m_pStrokeStyle = NULL;
		Point m_offset;
		PointF m_rotatePoint;
		float m_angle = 0;
	private:
		DXRender() = delete;
		DXRender(const DXRender& rightValue) = delete;
		DXRender& operator=(const DXRender&) = delete;
	public:
		ID2D1SolidColorBrush* GetBrush();
		ID2D1StrokeStyle* GetStrokeStyle();
	public:
		DXRender(DXImage* dxImage);
		DXRender(HDC dc, int_t x, int_t y, int_t width, int_t height);//创建dx绘图对象
		virtual ~DXRender();
		void SetFont(const std::wstring& fontFamily, float fontSize);//必须先调用
		void SetFont(const Font& _copy_font);//必须先调用
		void SetColor(const __EzUI__Color& color);//会之前必须调用
		void SetStrokeStyle(StrokeStyle strokeStyle = StrokeStyle::Solid, float dashWidth = 3);//设置样式 虚线/实线
		void DrawTextLayout(const TextLayout& textLayout, const PointF & = { 0,0 });//根据已有的布局绘制文字
		void DrawString(const std::wstring& text, const RectF& _rect, ezui::TextAlign textAlign);//绘制文字
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