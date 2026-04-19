#pragma once
#include "core/UIDef.h"
#include "GraphicsTypes.h"
#include "GraphicsShapes.h"
#include "GraphicsFont.h"
#include "Image.h"

namespace ezui {

#define EZUI_COLOR_SCALE 0.003921569f
#define EZUI_TO_D2D_COLOR_F(color) D2D_COLOR_F{FLOAT(color.GetR() * EZUI_COLOR_SCALE), FLOAT(color.GetG() * EZUI_COLOR_SCALE), FLOAT(color.GetB() * EZUI_COLOR_SCALE),FLOAT(color.GetA() * EZUI_COLOR_SCALE)}
#define EZUI_TO_D2D_RectF(rect) D2D_RECT_F{(FLOAT)rect.X,(FLOAT)rect.Y,(FLOAT)rect.GetRight(),(FLOAT)rect.GetBottom() }
#define EZUI_TO_D2D_PointF(pt)  D2D1_POINT_2F{(FLOAT)pt.X,(FLOAT)pt.Y}

	//安全释放COM对象
	template<typename Interface>
	inline void SafeRelease(
		Interface** ppInterfaceToRelease)
	{
		if (*ppInterfaceToRelease != NULL)
		{
			(*ppInterfaceToRelease)->Release();
			(*ppInterfaceToRelease) = NULL;
		}
	}
	namespace detail {
		class D2DGraphics;
		class FontManager;
		EZUI_API void GraphicsInitialize();//全局初始化direct2d
		EZUI_API void GraphicsShutdown();//释放direct2d
		EZUI_API float GetMaxRadius(float width, float height, float _radius);//获取最大半径 用于自动适应border-radius属性
		EZUI_API bool InstallFont(const char* fileData, uint32_t fileSize, std::wstring* outFontName = NULL);//从内存中安装加载字体
		EZUI_API IWICImagingFactory* GetImagingFactory();
		EZUI_API IDWriteFactory* GetWriteFactory();
		EZUI_API ID2D1Factory* GetD2D1Factory();
		EZUI_API FontManager* GetFontManager();
	};
};

namespace ezui {
	namespace detail {
		class EZUI_API D2DGraphics {
		public:
			struct TransformState {
				float Angle = std::numeric_limits<float>::quiet_NaN();
				Point Offset;
				PointF RotatePoint;
			};
		private:
			bool m_begin = false;
			FillStyle m_fillStyle = FillStyle::Solid;
			float m_angle = std::numeric_limits<float>::quiet_NaN();
			Point m_offset;
			PointF m_rotatePoint;
			Size m_size;
			WindowHandle m_hwnd = NULL;
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
			D2DGraphics(HDC dc, int width, int height);//创建d2d绘图对象
			D2DGraphics(WindowHandle hWnd, int width, int height);//创建d2d绘图对象
			void BeginDraw();
			bool EndDraw();
			virtual ~D2DGraphics();
			//如果此对象是由WindowId创建的才支持重置大小
			void ReSize(int width, int height);
			void BindDC(HDC dc, int width, int height);
			void SetFont(const std::wstring& fontFamily, float fontSize, int fontWeight = 0, FontStyle fontStyle = FontStyle::Normal);//必须先调用
			//设置字体 绘制之前必须先调用
			void SetFont(const Font& _copy_font);
			//设置颜色 绘制之前必须调用
			void SetColor(const Color& color);
			//设置描边样式 虚线/实线
			void SetStrokeStyle(StrokeStyle strokeStyle = StrokeStyle::Solid);
			//设置填充样式 实心/斜线...
			void SetFillStyle(FillStyle fillStyle = FillStyle::Solid);
			void DrawTextLayout(const TextLayout& textLayout, const PointF & = { 0,0 });//根据已有的布局绘制文字
			void DrawString(const std::wstring& text, const RectF& _rect, ezui::TextAlign textAlign);//绘制文字
			void DrawLine(const PointF& _A, const  PointF& _B, float width = 1);//绘制一条线
			void DrawRectangle(const RectF& _rect, float _radius = 0, float width = 1);//绘制矩形
			void FillRectangle(const RectF& _rect, float _radius = 0);
			void FillRectangle(const RectF& rect, const Gradient& gradient, float _radius = 0);//根据渐变配置填充矩形
			//填充矩形
			void PushLayer(const Geometry& geometry, float opacity = 1.0f);
			void PopLayer();
			void PushAxisAlignedClip(const RectF& rectBounds);
			void PopAxisAlignedClip();
			void SetTransform(float offsetX, float offsetY);//对画布进行旋转和偏移
			void SetTransform(float startX, float startY, float angle);//设置旋转起始点与旋转角度
			void SetTransform(float offsetX, float offsetY, float startX, float startY, float angle);
			TransformState GetTransformState() const noexcept;
			void RestoreTransformState(const TransformState& state);
			void PushAbsoluteAxisAlignedClip(const RectF& rectBounds);
			void PopAbsoluteAxisAlignedClip();
			bool IsAxisAlignedRectGeometry(const Geometry& geometry, const RectF& expectedRect, float epsilon = EZUI_FLOAT_EPSILON) const;
			void DrawImage(Image* _image, const  RectF& tagRect, float opacity = 1);//绘制图像
			void DrawBezier(const PointF& startPoint, const Bezier& points, float width = 1);//贝塞尔线
			void DrawBezier(const PointF& startPoint, std::list<Bezier>& points, float width = 1);//贝塞尔线
			void DrawEllipse(const RectF& rectF, float width = 1);
			void FillEllipse(const RectF& rectF);
			void DrawPie(const RectF& rectF, float startAngle, float endAngle, float strokeWidth = 1);
			void FillPie(const RectF& rectF, float startAngle, float endAngle);
			void DrawPoint(const PointF& pt);
			void DrawArrow(const PointF& from, const PointF& to, float headAngle = 45.f, float width = 1.0f);//绘制箭头
			void DrawArc(const RectF& rect, float startAngle, float sweepAngle, float width = 1);
			void DrawArc(const PointF& center, float radius, float startAngle, float sweepAngle, float width = 1);
			void DrawGeometry(ID2D1Geometry* path, float width = 1);
			void FillGeometry(ID2D1Geometry* path);
			void DrawGeometry(Geometry* path, float width = 1);
			void FillGeometry(Geometry* path);
			HRESULT Flush();
			ID2D1RenderTarget* Get();//获取原生d2d对象
		};
	};

	/// <summary>
	/// Graphics绘图上下文(由D2DRenderTarget实现) 功能: 绘制形状、文本、图像、变换等渲染操作
	/// </summary>
	using Graphics = detail::D2DGraphics;
};
