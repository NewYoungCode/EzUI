#pragma once
#include <Windows.h>
namespace EzUI {

	template<typename T>
	class __EzUI__Size
	{
	public:
		T Width;
		T Height;
	public:
		__EzUI__Size()
		{
			Width = Height = 0;
		}
		__EzUI__Size(const __EzUI__Size& __Size)
		{
			Width = __Size.Width;
			Height = __Size.Height;
		}
		__EzUI__Size(T width,
			T height)
		{
			Width = width;
			Height = height;
		}

		bool operator!=(const __EzUI__Size& _right) const
		{
			return !(Width == _right.Width && Height == _right.Height);
		}
		void Scale(const float& scale) {
			Width = (Width * scale) + 0.5;
			Height = (Height * scale) + 0.5;
		}
		bool Equals(const __EzUI__Size& sz) const
		{
			return (Width == sz.Width) && (Height == sz.Height);
		}
		bool Empty() const
		{
			return (Width == 0 && Height == 0);
		}
		__EzUI__Size operator+(const __EzUI__Size& sz) const
		{
			return __EzUI__Size(Width + sz.Width,
				Height + sz.Height);
		}
		__EzUI__Size operator-(const __EzUI__Size& sz) const
		{
			return __EzUI__Size(Width - sz.Width,
				Height - sz.Height);
		}

		bool operator==(const __EzUI__Size& _right) const
		{
			return Equals(_right);
		}
	};

	template<typename T>
	class __EzUI__Point
	{
	public:
		T X;
		T Y;
	public:
		__EzUI__Point()
		{
			X = Y = 0;
		}

		__EzUI__Point(const __EzUI__Point& __Point)
		{
			X = __Point.X;
			Y = __Point.Y;
		}

		__EzUI__Point(const __EzUI__Size<T>& __Size)
		{
			X = __Size.Width;
			Y = __Size.Height;
		}

		__EzUI__Point(T x,
			T y)
		{
			X = x;
			Y = y;
		}
		void Scale(const float& scale) {
			X = (X * scale) + 0.5;
			Y = (Y * scale) + 0.5;
		}
		bool Equals(const __EzUI__Point& __Point)const
		{
			return (X == __Point.X) && (Y == __Point.Y);
		}
		__EzUI__Point operator+(const __EzUI__Point& __Point) const
		{
			return __Point(X + __Point.X,
				Y + __Point.Y);
		}
		__EzUI__Point operator-(const __EzUI__Point& __Point) const
		{
			return __Point(X - __Point.X,
				Y - __Point.Y);
		}
		bool operator==(const __EzUI__Point& __Point) const
		{
			return Equals(__Point);
		}

	};

	template<typename T>
	class __EzUI__Rect
	{
	public:
		T X;
		T Y;
		T Width;
		T Height;
	public:

		__EzUI__Rect()
		{
			X = Y = Width = Height = 0;
		}

		__EzUI__Rect(T x,
			T y,
			T width,
			T height)
		{
			X = x;
			Y = y;
			Width = width;
			Height = height;
		}

		__EzUI__Rect(const RECT& rect) {
			X = rect.left;
			Y = rect.top;
			Width = rect.right - rect.left;
			Height = rect.bottom - rect.top;
		}

		__EzUI__Rect(const __EzUI__Point<T>& location, const __EzUI__Size<T>& size) {
			X = location.X;
			Y = location.Y;
			Width = size.Width;
			Height = size.Height;
		}

		__EzUI__Point<T> GetLocation() const
		{
			return __EzUI__Point<T>{ X, Y };
		}

		__EzUI__Size<T> GetSize() const
		{
			return __EzUI__Size<T>(Width, Height);
		}

		RECT ToRECT() const {
			return RECT{ GetLeft(),GetTop(),GetRight(),GetBottom() };
		}

		T GetLeft() const
		{
			return X;
		}

		T GetTop() const
		{
			return Y;
		}
		T GetRight() const
		{
			return X + Width;
		}
		T GetBottom() const
		{
			return Y + Height;
		}
		bool IsEmptyArea() const
		{
			return (Width <= 0) || (Height <= 0);
		}

		virtual const __EzUI__Rect& Scale(const float& scale) {
			X = (X * scale) + 0.5;
			Y = (Y * scale) + 0.5;
			Width = (Width * scale) + 0.5;
			Height = (Height * scale) + 0.5;
			return *this;
		}

		bool Equals(const __EzUI__Rect& __Rect) const
		{
			return X == __Rect.X &&
				Y == __Rect.Y &&
				Width == __Rect.Width &&
				Height == __Rect.Height;
		}

		bool  operator == (const __EzUI__Rect& right) {
			return Equals(right);
		}

		bool Contains(T x,
			T y) const
		{
			return x >= X && x < X + Width &&
				y >= Y && y < Y + Height;
		}

		bool Contains(const __EzUI__Point<T>& pt) const
		{
			return Contains(pt.X, pt.Y);
		}

		bool Contains(const __EzUI__Rect& __Rect) const
		{
			return (X <= __Rect.X) && (__Rect.GetRight() <= GetRight()) &&
				(Y <= __Rect.Y) && (__Rect.GetBottom() <= GetBottom());

		}

		void Inflate(T dx,
			T dy)
		{
			X -= dx;
			Y -= dy;
			Width += 2 * dx;
			Height += 2 * dy;
		}

		void Inflate(const __EzUI__Point<T>& point)
		{
			Inflate(point.X, point.Y);
		}

		bool Intersect(const __EzUI__Rect& __Rect)
		{
			return Intersect(*this, *this, __Rect);
		}

		static bool Intersect(__EzUI__Rect& c,
			const __EzUI__Rect& a,
			const __EzUI__Rect& b)
		{
			T right = min(a.GetRight(), b.GetRight());
			T bottom = min(a.GetBottom(), b.GetBottom());
			T left = max(a.GetLeft(), b.GetLeft());
			T top = max(a.GetTop(), b.GetTop());

			c.X = left;
			c.Y = top;
			c.Width = right - left;
			c.Height = bottom - top;
			return !c.IsEmptyArea();
		}

		bool IntersectsWith(const __EzUI__Rect& __Rect) const
		{
			return (GetLeft() < __Rect.GetRight() &&
				GetTop() < __Rect.GetBottom() &&
				GetRight() > __Rect.GetLeft() &&
				GetBottom() > __Rect.GetTop());
		}

		static bool Union(__EzUI__Rect& c,
			const __EzUI__Rect& a,
			const __EzUI__Rect& b)
		{

			if (a.IsEmptyArea()) {
				c = b;
				return !c.IsEmptyArea();
			}
			if (b.IsEmptyArea()) {
				c = a;
				return !c.IsEmptyArea();
			}

			T right = max(a.GetRight(), b.GetRight());
			T bottom = max(a.GetBottom(), b.GetBottom());
			T left = min(a.GetLeft(), b.GetLeft());
			T top = min(a.GetTop(), b.GetTop());

			c.X = left;
			c.Y = top;
			c.Width = right - left;
			c.Height = bottom - top;
			return !c.IsEmptyArea();
		}
		void Offset(const __EzUI__Point<T>& point)
		{
			Offset(point.X, point.Y);
		}
		void Offset(const T& dx,
			const T& dy)
		{
			X += dx;
			Y += dy;
		}
		virtual ~__EzUI__Rect() {}
	};

	class __EzUI__Color
	{
	protected:
		DWORD RGBA = 0;
	public:
		__EzUI__Color() {}
		__EzUI__Color(
			const BYTE& r,
			const BYTE& g,
			const BYTE& b,
			const BYTE& a = 255)
		{
			((BYTE*)&RGBA)[0] = r;
			((BYTE*)&RGBA)[1] = g;
			((BYTE*)&RGBA)[2] = b;
			((BYTE*)&RGBA)[3] = a;
		}
		__EzUI__Color(const DWORD& argb)
		{
			RGBA = argb;
		}
		virtual ~__EzUI__Color() {}
		const BYTE& GetR() const
		{
			return ((BYTE*)&RGBA)[0];
		}
		const BYTE& GetG() const
		{
			return ((BYTE*)&RGBA)[1];
		}
		const BYTE& GetB() const
		{
			return ((BYTE*)&RGBA)[2];
		}
		const BYTE& GetA() const
		{
			return ((BYTE*)&RGBA)[3];
		}
		const DWORD& GetValue() const
		{
			return RGBA;
		}
		void SetValue(DWORD argb)
		{
			RGBA = argb;
		}
		void SetR(const BYTE& value) {
			((BYTE*)&RGBA)[0] = value;
		}
		void SetG(const BYTE& value) {
			((BYTE*)&RGBA)[1] = value;
		}
		void SetB(const BYTE& value) {
			((BYTE*)&RGBA)[2] = value;
		}
		void SetA(const BYTE& value) {
			((BYTE*)&RGBA)[3] = value;
		}
	public:
		// Common color constants
		enum :DWORD
		{
			Transparent = 0,
			Red = 4278190335,
			Green = 4278255360,
			Blue = 4294901760,
			Black = 4278190080,
			White = 4294967295,
			Gray = 4286611584,
		};
	};

	template<typename T>
	class __EzUI__Line {
	public:
		__EzUI__Point<T> pointA;
		__EzUI__Point<T> pointB;
	public:
		__EzUI__Line() {
			pointA.X = 0;
			pointA.Y = 0;
			pointB.X = 0;
			pointB.Y = 0;
		}
		__EzUI__Line(const __EzUI__Point<T>& _pointA, const __EzUI__Point<T>& _pointB) {
			this->pointA = _pointA;
			this->pointB = _pointB;
		}
	};

	typedef  __EzUI__Point<int> Point;
	typedef  __EzUI__Point<float> PointF;
	typedef  __EzUI__Line<int> Line;
	typedef  __EzUI__Line<float> LineF;
	typedef  __EzUI__Size<int> Size;
	typedef  __EzUI__Size<float> SizeF;
	typedef  __EzUI__Rect<int> Rect;

	class RectF :public __EzUI__Rect<float> {
	public:
		RectF() {
			this->X = 0;
			this->Y = 0;
			this->Width = 0;
			this->Height = 0;
		}
		RectF(const Rect& rect) {
			this->X = rect.X;
			this->Y = rect.Y;
			this->Width = rect.Width;
			this->Height = rect.Height;
		}
		RectF(const RectF& rect) {
			this->X = rect.X;
			this->Y = rect.Y;
			this->Width = rect.Width;
			this->Height = rect.Height;
		}
		RectF(float x, float y, float width, float height) {
			this->X = x;
			this->Y = y;
			this->Width = width;
			this->Height = height;
		}
		virtual const RectF& Scale(const float& scale) {
			X = (X * scale);
			Y = (Y * scale);
			Width = (Width * scale);
			Height = (Height * scale);
			return *this;
		}
		virtual ~RectF() {};
	};

	struct Distance {
	public:
		size_t Left, Top, Right, Bottom;
		Distance() {
			Left = Top = Right = Bottom = 0;
		}
		Distance(const size_t& distanceAll) {
			Left = Top = Right = Bottom = distanceAll;
		}
		Distance& operator=(const size_t& distanceAll) {
			Left = Top = Right = Bottom = distanceAll;
			return *this;
		}
		void Scale(const float& scale) {
			Top = Top * scale + 0.5;
			Bottom = Bottom * scale + 0.5;
			Left = Left * scale + 0.5;
			Right = Right * scale + 0.5;
		}
		//获取垂直所占空间
		size_t GetVSpace() {
			return Top + Bottom;
		}
		//获取水平所占空间
		size_t GetHSpace() {
			return Left + Right;
		}
	};

	enum class ImageSizeMode {
		//
		// 摘要:
		//     Owner控件 中的图像被拉伸或收缩，以适合 Owner控件
		//     的大小。
		StretchImage = 1,
		// 摘要:
		//     如果 Owner控件 比图像大，则图像将居中显示。如果图像比 Owner控件
		//     大，则图片将居于 Owner控件 中心，而外边缘将被剪裁掉。
		CenterImage = 3,
		//
		// 摘要:
		//     图像大小按其原有的大小比例被增加或减小。
		Zoom = 4
	};
	/// <summary>
	/// 描述边框的一些信息
	/// </summary>
	class  Border {
	public:
		int Left = 0;//左边边框大小
		int Top = 0;//顶部边框大小
		int Right = 0;//右边边框大小
		int Bottom = 0;//底部边框大小
		int TopLeftRadius = 0;
		int TopRightRadius = 0;
		int BottomRightRadius = 0;
		int BottomLeftRadius = 0;
		__EzUI__Color Color;
	public:
		class Radius {
			Border& Border;
		public:
			Radius(EzUI::Border& bd) :Border(bd) {}
			//对四个角度同时设置半径大小
			Radius& operator=(const int& radius) {
				Border.TopLeftRadius = radius;
				Border.TopRightRadius = radius;
				Border.BottomRightRadius = radius;
				Border.BottomLeftRadius = radius;
				return *this;
			}
		};
	public:
		Border::Radius Radius = (*this);
	public:
		Border() {}
		//对四个边设置大小
		Border& operator=(const int& borderWidth) {
			Left = borderWidth;
			Top = borderWidth;
			Right = borderWidth;
			Bottom = borderWidth;
			return *this;
		}
		void Scale(const float& scale) {
			Left = Left * scale + 0.5;
			Top = Top * scale + 0.5;
			Right = Right * scale + 0.5;
			Bottom = Bottom * scale + 0.5;
			TopLeftRadius = TopLeftRadius * scale + 0.5;
			TopRightRadius = TopRightRadius * scale + 0.5;
			BottomRightRadius = BottomRightRadius * scale + 0.5;
			BottomLeftRadius = BottomLeftRadius * scale + 0.5;
		}
	};
#if 1
#define Align_Top  1
#define Align_Bottom  2
#define Align_Left  4
#define Align_Right  8
#define Align_Mid  16
#define Align_Center  32
#else //GDI
#define Align_Top  DT_TOP
#define Align_Bottom  DT_BOTTOM
#define Align_Left  DT_LEFT
#define Align_Right  DT_RIGHT
#define Align_Mid   DT_VCENTER
#define Align_Center   DT_CENTER
#endif
	/// <summary>
	/// 水平状态下的对其方式
	/// </summary>
	enum class HAlign :int
	{
		Left = Align_Left,
		Center = Align_Center,
		Right = Align_Right
	};
	/// <summary>
	/// 垂直状态下的对其方式
	/// </summary>
	enum class VAlign :int
	{
		Top = Align_Top,
		Mid = Align_Mid,
		Bottom = Align_Bottom
	};

	enum class __Align :int {
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上左边对齐。
		TopLeft = (int)VAlign::Top | (int)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上居中对齐。
		TopCenter = (int)VAlign::Top | (int)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上右边对齐。
		TopRight = (int)VAlign::Top | (int)HAlign::Right,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上左边对齐。
		MiddleLeft = (int)VAlign::Mid | (int)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上居中对齐。
		MiddleCenter = (int)VAlign::Mid | (int)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上右边对齐。
		MiddleRight = (int)VAlign::Mid | (int)HAlign::Right,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上左边对齐。
		BottomLeft = (int)VAlign::Bottom | (int)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上居中对齐。
		BottomCenter = (int)VAlign::Bottom | (int)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上右边对齐。
		BottomRight = (int)VAlign::Bottom | (int)HAlign::Right
	};

	typedef __Align TextAlign;

	enum class FontStyle :int {
		NORMAL = 0
		/* DWRITE_FONT_STYLE_NORMAL
		字体样式 ：正常。
		DWRITE_FONT_STYLE_OBLIQUE
		字体样式 ：倾斜。
		DWRITE_FONT_STYLE_ITALIC
		字体样式 ：斜体。 */
	};

	class IImage {
	protected:
		size_t _frameCount = 0;//总帧数
		size_t _framePos = 0;//当前帧率索引
	public:
		Rect Offset;//取出图像部分区域进行绘制
		ImageSizeMode SizeMode = ImageSizeMode::Zoom;// 图像显示模式
	public:
		virtual ~IImage() {}
		size_t FrameCount() {
			return _frameCount;
		}
		//跳转到下一帧 并且获取下一帧的延迟
		virtual size_t NextFrame() {
			if (_framePos >= _frameCount) {
				_framePos = 0;
			}
			++_framePos;
			return 0;
		}
	};

	inline Rect Transformation(ImageSizeMode imageSizeMode, const Rect& rect, const Size& imgSize) {

		if (imageSizeMode == ImageSizeMode::StretchImage) {
			return rect;
		}

		//客户端数据
		float clientWidth = (float)rect.Width;
		float clientHeight = (float)rect.Height;
		float clientRate = clientWidth / clientHeight;
		//图片数据
		float imgWidth = (float)imgSize.Width;
		float imgHeight = (float)imgSize.Height;
		float imgRate = imgWidth / imgHeight;

		if (imageSizeMode == ImageSizeMode::Zoom) {
			if (clientRate < imgRate) {
				float zoomHeight = clientWidth / imgWidth * imgHeight + 0.5f;
				float y = (clientHeight - zoomHeight) / 2.0f + rect.Y;
				return Rect(rect.X, y, clientWidth, zoomHeight);
			}
			else {
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;
				float x = (clientWidth - zoomWidth) / 2.0f + rect.X;
				return Rect(x, rect.Y, zoomWidth, clientHeight);
			}
		}
		if (imageSizeMode == ImageSizeMode::CenterImage) {
			if (clientRate < imgRate) {
				//1000 670 客户端
				//1000 300 图片
				//2233 670     缩放后的图片大小 
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;//图片应该这么宽才对
				float x = (zoomWidth - clientWidth) / 2.0f + 0.5f;
				return Rect((rect.X - x), rect.Y, zoomWidth, clientHeight);
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				float zoomHeight = clientWidth / imgWidth * imgHeight + 0.5f;//图片应该这么高才对
				float y = (zoomHeight - clientHeight) / 2.0f + 0.5f;
				return Rect(rect.X, (rect.Y - y), clientWidth, zoomHeight);
			}
		}
		return rect;
	}
};