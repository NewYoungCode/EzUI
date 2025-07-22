#pragma once
#include <Windows.h>

namespace ezui {
#ifdef _WIN64
	typedef __int64  int_t;
#else
	typedef int      int_t;
#endif
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
		void Scale(float scale) {
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
		void Scale(float scale) {
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
			return RECT{ (LONG)GetLeft(), (LONG)GetTop(), (LONG)GetRight(), (LONG)GetBottom() };
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

		virtual const __EzUI__Rect& Scale(float scale) {
			X = T((X * scale) + 0.5);
			Y = T((Y * scale) + 0.5);
			Width = T((Width * scale) + 0.5);
			Height = T((Height * scale) + 0.5);
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
		void Offset(T dx,
			T dy)
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
		__EzUI__Color(DWORD argb)
		{
			RGBA = argb;
		}
		virtual ~__EzUI__Color() {}
		BYTE GetR() const
		{
			return ((BYTE*)&RGBA)[0];
		}
		BYTE GetG() const
		{
			return ((BYTE*)&RGBA)[1];
		}
		BYTE GetB() const
		{
			return ((BYTE*)&RGBA)[2];
		}
		BYTE GetA() const
		{
			return ((BYTE*)&RGBA)[3];
		}
		DWORD GetValue() const
		{
			return RGBA;
		}
		void SetValue(DWORD argb)
		{
			RGBA = argb;
		}
		void SetR(BYTE value) {
			((BYTE*)&RGBA)[0] = value;
		}
		void SetG(BYTE value) {
			((BYTE*)&RGBA)[1] = value;
		}
		void SetB(BYTE value) {
			((BYTE*)&RGBA)[2] = value;
		}
		void SetA(BYTE value) {
			((BYTE*)&RGBA)[3] = value;
		}
	public:
		// Common color constants
		enum : DWORD
		{
			Transparent = 0x00000000, // 全透明

			Black = 0xFF000000, // 黑色
			White = 0xFFFFFFFF, // 白色
			Gray = 0xFF808080, // 灰色
			LightGray = 0xFFD3D3D3, // 浅灰色
			DarkGray = 0xFFA9A9A9, // 深灰色

			Red = 0xFF0000FF, // 红色
			DarkRed = 0xFF00008B, // 深红色
			LightCoral = 0xFF8080F0, // 浅珊瑚红
			Tomato = 0xFF4763FF, // 番茄色
			Crimson = 0xFF3C14DC, // 猩红色

			Green = 0xFF00FF00, // 绿色
			Lime = 0xFF00FF00, // 酸橙绿（亮绿）
			DarkGreen = 0xFF006400, // 深绿色
			LawnGreen = 0xFF00FC7C, // 草坪绿
			PaleGreen = 0xFF98FB98, // 苍绿色

			Blue = 0xFFFF0000, // 蓝色
			RoyalBlue = 0xFFE16941, // 皇家蓝
			DodgerBlue = 0xFFFF901E, // 道奇蓝
			DeepSkyBlue = 0xFFFFBF00, // 深天蓝
			LightBlue = 0xFFE6D8AD, // 浅蓝色

			Yellow = 0xFFFFFF00, // 黄色
			Gold = 0xFFD700FF, // 金色
			LightYellow = 0xFFE0FFFF, // 浅黄色
			Khaki = 0xFF8CE6F0, // 卡其色

			Orange = 0xFFA500FF, // 橙色
			DarkOrange = 0xFF008CFF, // 深橙色
			Coral = 0xFF507FFF, // 珊瑚色
			Salmon = 0xFF7280FA, // 鲑鱼色

			Purple = 0xFF800080, // 紫色
			MediumPurple = 0xFFDB7093, // 中紫色
			Indigo = 0xFF82004B, // 靛青色
			Violet = 0xFFEE82EE, // 紫罗兰
			Plum = 0xFFDDA0DD, // 李子紫

			Cyan = 0xFFFFFF00, // 青色
			Teal = 0xFF008080, // 水鸭色
			Aqua = 0xFFFFFF00, // 浅绿色（水色）
			Turquoise = 0xFFD0E040, // 绿松石色

			Brown = 0xFF2A2AA5, // 棕色
			Maroon = 0xFF000080, // 栗色（褐红）
			Tan = 0xFF8CB4D2, // 茶色
			Beige = 0xFFDCF5F5, // 米色

			Navy = 0xFF800000, // 藏青色
			Olive = 0xFF008080, // 橄榄色
			Silver = 0xFFC0C0C0  // 银色
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

	typedef  __EzUI__Point<int_t> Point;
	typedef  __EzUI__Point<float> PointF;
	typedef  __EzUI__Line<int_t> Line;
	typedef  __EzUI__Line<float> LineF;
	typedef  __EzUI__Size<int_t> Size;
	typedef  __EzUI__Size<float> SizeF;
	typedef  __EzUI__Rect<int_t> Rect;

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
		virtual const RectF& Scale(float scale) {
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
		WORD Left, Top, Right, Bottom;
		Distance() {
			Left = Top = Right = Bottom = 0;
		}
		Distance(WORD distanceAll) {
			Left = Top = Right = Bottom = distanceAll;
		}
		Distance& operator=(WORD distanceAll) {
			Left = Top = Right = Bottom = distanceAll;
			return *this;
		}
		void Scale(float scale) {
			Top = Top * scale + 0.5;
			Bottom = Bottom * scale + 0.5;
			Left = Left * scale + 0.5;
			Right = Right * scale + 0.5;
		}
		//获取垂直所占空间
		WORD GetVSpace() {
			return Top + Bottom;
		}
		//获取水平所占空间
		WORD GetHSpace() {
			return Left + Right;
		}
	};

	enum class ImageSizeMode {
		//
		// 摘要:
		//     Owner控件 中的图像被拉伸或收缩，以适合 Owner控件
		//     的大小。
		StretchImage,
		//
		// 摘要:
		//     如果 Owner控件 比图像大，则图像将居中显示。如果图像比 Owner控件
		//     大，则图片将居于 Owner控件 中心，而外边缘将被剪裁掉。
		CenterImage,
		//
		// 摘要:
		//     图像大小按其原有的大小比例被增加或减小。
		Zoom,
		//
		// 摘要:
		//     图像按原始大小居中显示，不进行缩放。
		OriginalSize
	};


	//描边样式
	enum class StrokeStyle
	{
		None,//无
		Solid,//实线
		Dash//虚线
	};

	/// <summary>
	/// 描述边框的一些信息
	/// </summary>
	class  Border {
	public:
		WORD Left = 0;//左边边框大小
		WORD Top = 0;//顶部边框大小
		WORD Right = 0;//右边边框大小
		WORD Bottom = 0;//底部边框大小
		WORD TopLeftRadius = 0;
		WORD TopRightRadius = 0;
		WORD BottomRightRadius = 0;
		WORD BottomLeftRadius = 0;
		__EzUI__Color Color;
		StrokeStyle Style = StrokeStyle::None;
	public:
		class Radius {
			Border& Border;
		public:
			Radius(ezui::Border& bd) :Border(bd) {}
			//对四个角度同时设置半径大小
			Radius& operator=(WORD radius) {
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
		Border& operator=(WORD borderWidth) {
			Left = borderWidth;
			Top = borderWidth;
			Right = borderWidth;
			Bottom = borderWidth;
			return *this;
		}
		void Scale(float scale) {
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
	enum class HAlign :int_t
	{
		Left = Align_Left,
		Center = Align_Center,
		Right = Align_Right
	};
	/// <summary>
	/// 垂直状态下的对其方式
	/// </summary>
	enum class VAlign :int_t
	{
		Top = Align_Top,
		Mid = Align_Mid,
		Bottom = Align_Bottom
	};

	enum class __Align :int_t {
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上左边对齐。
		TopLeft = (int_t)VAlign::Top | (int_t)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上居中对齐。
		TopCenter = (int_t)VAlign::Top | (int_t)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上右边对齐。
		TopRight = (int_t)VAlign::Top | (int_t)HAlign::Right,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上左边对齐。
		MiddleLeft = (int_t)VAlign::Mid | (int_t)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上居中对齐。
		MiddleCenter = (int_t)VAlign::Mid | (int_t)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上右边对齐。
		MiddleRight = (int_t)VAlign::Mid | (int_t)HAlign::Right,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上左边对齐。
		BottomLeft = (int_t)VAlign::Bottom | (int_t)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上居中对齐。
		BottomCenter = (int_t)VAlign::Bottom | (int_t)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上右边对齐。
		BottomRight = (int_t)VAlign::Bottom | (int_t)HAlign::Right
	};

	typedef __Align TextAlign;

	enum class FontStyle :int_t {
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
		WORD _frameCount = 0;//总帧数
		WORD _framePos = 0;//当前帧率索引
	public:
		Rect Offset;//取出图像部分区域进行绘制
		ImageSizeMode SizeMode = ImageSizeMode::Zoom;// 图像显示模式
	public:
		virtual ~IImage() {}
		WORD FrameCount() {
			return _frameCount;
		}
		//跳转到下一帧 并且获取下一帧的延迟
		virtual WORD NextFrame() {
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
				return Rect(rect.X, (int_t)y, (int_t)clientWidth, (int_t)zoomHeight);
			}
			else {
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;
				float x = (clientWidth - zoomWidth) / 2.0f + rect.X;
				return Rect((int_t)x, rect.Y, (int_t)zoomWidth, (int_t)clientHeight);
			}
		}
		if (imageSizeMode == ImageSizeMode::CenterImage) {
			if (clientRate < imgRate) {
				//1000 670 客户端
				//1000 300 图片
				//2233 670     缩放后的图片大小 
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;//图片应该这么宽才对
				float x = (zoomWidth - clientWidth) / 2.0f + 0.5f;
				return Rect((int_t)(rect.X - x), rect.Y, (int_t)zoomWidth, (int_t)clientHeight);
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				float zoomHeight = clientWidth / imgWidth * imgHeight + 0.5f;//图片应该这么高才对
				float y = (zoomHeight - clientHeight) / 2.0f + 0.5f;
				return Rect(rect.X, (int_t)(rect.Y - y), (int_t)clientWidth, (int_t)zoomHeight);
			}
		}
		//按照图片原大小居中显示
		if (imageSizeMode == ImageSizeMode::OriginalSize) {
			int_t x = (rect.Width - imgSize.Width) / 2.0f + 0.5;
			int_t y = (rect.Height - imgSize.Height) / 2.0f + 0.5;
			return Rect(x, y, imgSize.Width, imgSize.Height);
		}
		return rect;
	}
};