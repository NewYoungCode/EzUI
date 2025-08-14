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
		int Length;
		int TextPos;//命中的下标
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
		int GetFontHeight() {
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
		Point HitTestPoint(const Point& pt, int* outTextPos, BOOL* outIsTrailingHit, int* fontHeight);
		void HitTestPoint(const Point& pt, HitTestMetrics* hitTestMetrics);//根据坐标执行命中测试
		Point HitTestTextPosition(int textPos, BOOL isTrailingHit);//根据文字下标执行命中测试
		const std::wstring& GetFontFamily();
		float GetFontSize();
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

	//几何图形基础类(支持自定义路径)
	class UI_EXPORT Geometry {
	protected:
		ID2D1GeometrySink* m_pSink = NULL;
		ID2D1Geometry* m_rgn = NULL;
		Geometry(const Geometry& rightCopy) = delete;
	public:
		Geometry();
		virtual ~Geometry();
		void AddArc(const PointF& endPoint, float radius);
		void AddAcr(const D2D1_ARC_SEGMENT& arc);
		void AddLine(const PointF& endPoint);
		void BeginFigure(const PointF& startPoint, D2D1_FIGURE_BEGIN figureBegin = D2D1_FIGURE_BEGIN_FILLED);
		void CloseFigure(D2D1_FIGURE_END figureEnd = D2D1_FIGURE_END_CLOSED);
		ID2D1Geometry* Get()const;
	public:
		/// <summary>
		/// 将两个几何图形通过指定的合并模式（Union、Intersect、Xor、Exclude）合并到一个输出几何中。
		/// </summary>
		/// <param name="out">合并结果输出到该 Geometry。</param>
		/// <param name="a">参与合并的第一个 Geometry。</param>
		/// <param name="b">参与合并的第二个 Geometry。</param>
		/// <param name="COMBINE_MODE">几何合并模式，取值如 D2D1_COMBINE_MODE_UNION、INTERSECT、XOR、EXCLUDE。</param>
		static void Combine(Geometry& out, const Geometry& a, const Geometry& b, D2D1_COMBINE_MODE COMBINE_MODE);

		/// <summary>
		/// 合并两个区域，取它们的联合部分（即最大边界区域）。
		/// </summary>
		static void Union(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_UNION);
		}

		/// <summary>
		/// 获取两个区域的交集部分。
		/// </summary>
		static void Intersect(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_INTERSECT);
		}

		/// <summary>
		/// 合并两个区域，保留不重叠的部分（异或运算）。
		/// </summary>
		static void Xor(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_XOR);
		}

		/// <summary>
		/// 从第一个区域中排除第二个区域的部分（差集）。
		/// </summary>
		static void Exclude(Geometry& out, const Geometry& a, const Geometry& b) {
			Combine(out, a, b, D2D1_COMBINE_MODE::D2D1_COMBINE_MODE_EXCLUDE);
		}
	};

	//矩形(已经完成闭合)
	class UI_EXPORT RectangleGeometry :public Geometry {
	public:
		RectangleGeometry(float x, float y, float width, float height);
		RectangleGeometry(float x, float y, float width, float height, float _radius);
		RectangleGeometry(const RectF& _rect, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius);
		virtual ~RectangleGeometry() {};
	};

	//扇形(已经完成闭合)
	class UI_EXPORT PieGeometry :public Geometry {
	public:
		PieGeometry(const RectF& rectF, float startAngle, float endAngle);
		virtual ~PieGeometry() {};
	};

	//圆形/椭圆(已经完成闭合)
	class UI_EXPORT EllipseGeometry :public PieGeometry {
	public:
		EllipseGeometry(const RectF& rectF) :PieGeometry(rectF, 0, 360) {}
		virtual ~EllipseGeometry() {};
	};

	class UI_EXPORT DXImage : public IImage {
		struct GifFrame
		{
			IWICBitmap* wicBitmap; // 存一份完整帧
			UINT delay;
		};
	protected:
		std::vector<GifFrame> m_frames;
		IWICBitmapDecoder* m_bitmapdecoder = NULL;
		IWICBitmapFrameDecode* m_pframe = NULL;
		IWICFormatConverter* m_fmtcovter = NULL;//从文件加载
		IWICBitmap* m_bitMap = NULL;//从HBITMAP中加载
		int m_width = 0;
		int m_height = 0;
		ID2D1Bitmap* m_d2dBitmap = NULL;
	private:
		void CreateFormStream(IStream* istram);
		void CreateFromFile(const std::wstring& file);
		void Init();
	public:
		bool Visible = true;
		void DecodeOfRender(ID2D1RenderTarget* render);
		//如果HBITMAP带有透明通道 确保传入的图像颜色值已经与 Alpha 通道预乘
		DXImage(HBITMAP hBitmap);
		DXImage(IStream* istram);
		DXImage(const std::wstring& file);
		//创建带预乘Alpha的BGRA图片
		DXImage(int width, int height);
		DXImage(const void* data, size_t count);
		ID2D1Bitmap* Get();
		IWICBitmap* GetIWICBitmap();
		int Width();
		int Height();
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
		DXRender(HDC dc, int x, int y, int width, int height);//创建dx绘图对象
		virtual ~DXRender();
		void SetFont(const std::wstring& fontFamily, float fontSize);//必须先调用
		void SetFont(const Font& _copy_font);//必须先调用
		void SetColor(const __EzUI__Color& color);//会之前必须调用
		void SetStrokeStyle(StrokeStyle strokeStyle = StrokeStyle::Solid);//设置样式 虚线/实线
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
		void DrawEllipse(const RectF& rectF, float width = 1);
		void FillEllipse(const RectF& rectF);
		void DrawPie(const RectF& rectF, float startAngle, float endAngle, float strokeWidth = 1);
		void FillPie(const RectF& rectF, float startAngle, float endAngle);
		void DrawPoint(const PointF& pt);
		void DrawArc(const RectF& rect, float startAngle, float sweepAngle, float width = 1);//未实现
		void DrawArc(const PointF& point1, const  PointF& point2, const PointF& point3, float width = 1);
		void DrawGeometry(ID2D1Geometry* path, float width = 1);
		void FillGeometry(ID2D1Geometry* path);
		void DrawGeometry(Geometry* path, float width = 1);
		void FillGeometry(Geometry* path);
		void Flush();
		ID2D1DCRenderTarget* Get();//获取原生DX对象
	};
};
#endif