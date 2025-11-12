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
#include "GraphicsTypes.h"

namespace ezui {
	class Image;

	namespace detail {
		class D2DGraphics;

		UI_EXPORT void RenderInitialize();//全局初始化direct2d
		UI_EXPORT void RenderUnInitialize();//释放direct2d
		UI_EXPORT float GetMaxRadius(float width, float height, float _radius);//获取最大半径 用于自动适应border-radius属性
		UI_EXPORT bool InstallFont(const char* fileData, uint32_t fileSize, std::wstring* outFontName = NULL);//从内存中安装加载字体
	};

	//字体类
	class UI_EXPORT Font {
	private:
		Font() = delete;
		bool m_ref = false;
		FontStyle m_fontStyle = FontStyle::Normal;
		float m_fontSize = 0;
		int m_fontWeight = 0;
		IDWriteTextFormat* m_value = NULL;
		std::wstring m_fontFamily;
		void Copy(const Font& _copy);
	public:
		Font(const Font& _copy);
		Font(const std::wstring& fontFamily, float fontSize, int fontweight = 0, FontStyle fontStyle = FontStyle::Normal);
		float GetFontSize()const;
		float GetFontWeight()const;
		FontStyle GetFontStyle() const;
		const std::wstring& GetFontFamily()const;
		IDWriteTextFormat* Get() const;
		bool operator==(const Font& _right);
		virtual ~Font();
	};

	//文本命中测试数据
	class UI_EXPORT HitTestMetrics {
	public:
		bool IsTrailingHit;//命中位置是否在尾部
		int Length;
		int TextPos;//命中的下标
		RectF FontBox;//文字的矩形位置
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
			return int(FontBox.Height + 0.5);
		}
	};

	class UI_EXPORT TextLayout {
	private:
		TextLayout(const TextLayout& rightValue) = delete;
		int m_unicodeSize = 0;
		float m_fontSize = 0;
		IDWriteTextLayout* m_textLayout = NULL;
		DWRITE_TEXT_METRICS m_textMetrics = {};
		std::vector<RectF> m_lineRects;
		std::wstring m_fontFamily;
	public:
		void GetMetrics();
		TextLayout(const std::wstring& text, const Font& font, const SizeF& maxSize = SizeF{ EZUI_FLOAT_MAX,EZUI_FLOAT_MAX }, TextAlign textAlgin = TextAlign::TopLeft);
		Point HitTestPoint(const Point& pt, int* outTextPos, BOOL* outIsTrailingHit, int* fontHeight);
		void HitTestPoint(const Point& pt, HitTestMetrics* hitTestMetrics);//根据坐标执行命中测试
		Point HitTestTextPosition(int textPos, BOOL isTrailingHit);//根据文字下标执行命中测试
		const std::wstring& GetFontFamily();
		float GetFontSize();
		int Width();
		int Height();
		//获取文本整体的占用空间
		Size GetFontBox();
		//获取每行文字的矩形位置
		const std::vector<RectF>& GetLineRects();
		int GetFontHeight();//获取字体高度
		int GetLineCount();//获取一共有多少行
		IDWriteTextLayout* Get() const;
		void SetTextAlign(TextAlign textAlign);
		void SetUnderline(int pos = 0, int count = 0);
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
	private:
		void Create(float x, float y, float width, float height, float _radius);
	public:
		RectangleGeometry(float x, float y, float width, float height, float _radius = 0);
		RectangleGeometry(const RectF& _rect, float radius = 0);
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

	//DxImage
	namespace detail {
		class UI_EXPORT DXImage : public detail::IImage {
			friend class D2DGraphics;
			struct GifFrame
			{
				IWICBitmap* wicBitmap; // 存一份完整帧
				UINT delay;
			};
		protected:
			int m_width = 0;
			int m_height = 0;
			IWICBitmap* m_bitMap = NULL;//从HBITMAP中加载
			std::vector<GifFrame> m_frames;
		private:
			void CreateFormStream(IStream* istram);
			void CreateFromFile(const std::wstring& file);
			void Init(IWICBitmapDecoder* m_bitmapdecoder);
			void CopyFrames(UINT fCount, IWICBitmapDecoder* m_bitmapdecoder);
			HRESULT CreateD2DBitmap(ID2D1RenderTarget* render, ID2D1Bitmap** outD2DBitmap);
		public:
			bool Visible = true;
			//如果HBITMAP带有透明通道 确保传入的图像颜色值已经与 Alpha 通道预乘
			DXImage(DXImage* dxImage);
			DXImage(HBITMAP hBitmap);
			DXImage(IStream* istram);
			DXImage(const std::wstring& file);
			//创建带预乘Alpha的BGRA图片
			DXImage(int width, int height);
			DXImage(const void* data, size_t count);
			IWICBitmap* GetIWICBitmap();
			int Width();
			int Height();
			virtual int NextFrame()override;
			virtual DXImage* Clone();
			virtual ~DXImage();
		};
	};

	//贝塞尔曲线
	class Bezier {
	public:
		Point point1;
		Point point2;
		Point point3;
	};
};

namespace ezui {
	namespace detail {
		class UI_EXPORT D2DGraphics {
		private:
			bool m_begin = false;
			FillStyle m_fillStyle = FillStyle::Solid;
			float m_angle = std::numeric_limits<float>::quiet_NaN();
			Point m_offset;
			PointF m_rotatePoint;
			Size m_size;
			HWND m_hwnd = NULL;
			HDC m_hdc = NULL;
			ID2D1RenderTarget* m_render = NULL;
			ID2D1SolidColorBrush* m_brush = NULL;
			ID2D1Brush* m_fillBrush = NULL;
			Font* m_font = NULL;
			ID2D1StrokeStyle* m_pStrokeStyle = NULL;
			D2D_COLOR_F m_color = {};
			std::list<ID2D1Layer*> m_layers;
		private:
			D2DGraphics(const D2DGraphics&) = delete;            // 禁止拷贝构造
			D2DGraphics& operator=(const D2DGraphics&) = delete; // 禁止拷贝赋值
			D2DGraphics(D2DGraphics&&) = delete;                 // 禁止移动构造
			D2DGraphics& operator=(D2DGraphics&&) = delete;      // 禁止移动赋值
		private:
			ID2D1Brush* GetStrokeBrush();// 获取用于描边（Stroke）绘制的画刷。
			ID2D1Brush* GetFillBrush();// 获取用于填充（Fill）绘制的画刷。
			ID2D1StrokeStyle* GetStrokeStyle();
		public:
			D2DGraphics(Image* image);
			D2DGraphics(HDC dc, int width, int height);//创建dx绘图对象
			D2DGraphics(HWND hWnd, int width, int height);//创建dx绘图对象
			void BeginDraw();
			void EndDraw();
			virtual ~D2DGraphics();
			//如果此对象是由HWND创建的才支持重置大小
			void ReSize(int width, int height);
			void BindDC(HDC dc, int width, int height);
			void SetFont(const std::wstring& fontFamily, float fontSize, int fontWeight = 0, FontStyle fontStyle = FontStyle::Normal);//必须先调用
			//设置字体 绘制之前必须先调用
			void SetFont(const Font& _copy_font);
			//设置颜色 绘制之前必须调用
			void SetColor(const detail::__EzUI__Color& color);
			//设置描边样式 虚线/实线
			void SetStrokeStyle(StrokeStyle strokeStyle = StrokeStyle::Solid);
			//设置填充样式 实心/斜线...
			void SetFillStyle(FillStyle fillStyle = FillStyle::Solid);
			void DrawTextLayout(const TextLayout& textLayout, const PointF & = { 0,0 });//根据已有的布局绘制文字
			void DrawString(const std::wstring& text, const RectF& _rect, ezui::TextAlign textAlign);//绘制文字
			void DrawLine(const PointF& _A, const  PointF& _B, float width = 1);//绘制一条线
			void DrawRectangle(const RectF& _rect, float _radius = 0, float width = 1);//绘制矩形
			void FillRectangle(const RectF& _rect, float _radius = 0);
			//填充矩形
			void PushLayer(const Geometry& dxGeometry, float opacity = 1.0f);
			void PopLayer();
			void PushAxisAlignedClip(const RectF& rectBounds);
			void PopAxisAlignedClip();
			void SetTransform(float offsetX, float offsetY);//对画布进行旋转和偏移
			void SetTransform(float startX, float startY, float angle);//设置旋转起始点与旋转角度
			void SetTransform(float offsetX, float offsetY, float startX, float startY, float angle);
			void DrawImage(Image* _image, const  RectF& tagRect, float opacity = 1);//绘制图像
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
			HRESULT Flush();
			ID2D1RenderTarget* Get();//获取原生DX对象
		};
	};
};
#endif