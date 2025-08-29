#pragma once
#include <Windows.h>

namespace ezui {
	//size模式
	enum class SizeMode {
		//图片强行拉伸完全填充控件
		//不裁剪,图片变形
		Stretch,
		//图片缩放后完全填充控件
		//图片会裁剪, 保持比例裁剪和控件同大小
		Cover,
		//图片缩放后完整居中显示在控件上 
		//控件会留白
		Fit,
		//图片保持原尺寸, 
		//如果图片小于控件: 控件留白,
		//如果图片大于控件: 控件边界外的部分被裁剪
		Original
	};
	typedef SizeMode ImageSizeMode;

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
	/// 水平状态下的对齐方式
	/// </summary>
	enum class HAlign :int
	{
		Left = Align_Left,
		Center = Align_Center,
		Right = Align_Right
	};
	EZUI_ENUM_OPERATORS(HAlign, int);

	/// <summary>
	/// 垂直状态下的对齐方式
	/// </summary>
	enum class VAlign :int
	{
		Top = Align_Top,
		Mid = Align_Mid,
		Bottom = Align_Bottom
	};
	EZUI_ENUM_OPERATORS(VAlign, int);

	//包含垂直与水平对齐方式
	enum class Align :int {
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
	EZUI_ENUM_OPERATORS(Align, int);

	typedef Align TextAlign;

	enum class FontStyle {
		NORMAL
		/* DWRITE_FONT_STYLE_NORMAL
		字体样式 ：正常。
		DWRITE_FONT_STYLE_OBLIQUE
		字体样式 ：倾斜。
		DWRITE_FONT_STYLE_ITALIC
		字体样式 ：斜体。 */
	};

	//描边样式
	enum class StrokeStyle
	{
		None,//无
		Solid,//实线
		Dash//虚线
	};


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

		__EzUI__Rect& operator+=(T value) {
			X -= value;
			Y -= value;
			Width += value * 2;
			Height += value * 2;
			return *this;
		}
		__EzUI__Rect& operator-=(T value) {
			X += value;
			Y += value;
			Width -= value * 2;
			Height -= value * 2;
			return *this;
		}
		__EzUI__Rect operator+(T value) const {
			__EzUI__Rect out = *this;
			out += value;
			return out;
		}
		__EzUI__Rect operator-(T value) const {
			__EzUI__Rect out = *this;
			out -= value;
			return out;
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
		DWORD BGRA = 0;
	public:
		__EzUI__Color() {}
		__EzUI__Color(
			const BYTE& r,
			const BYTE& g,
			const BYTE& b,
			const BYTE& a = 255)
		{
			((BYTE*)&BGRA)[0] = b;
			((BYTE*)&BGRA)[1] = g;
			((BYTE*)&BGRA)[2] = r;
			((BYTE*)&BGRA)[3] = a;
		}
		__EzUI__Color(DWORD bgra)
		{
			BGRA = bgra;
		}
		virtual ~__EzUI__Color() {}
		BYTE GetR() const
		{
			return ((BYTE*)&BGRA)[2];
		}
		BYTE GetG() const
		{
			return ((BYTE*)&BGRA)[1];
		}
		BYTE GetB() const
		{
			return ((BYTE*)&BGRA)[0];
		}
		BYTE GetA() const
		{
			return ((BYTE*)&BGRA)[3];
		}
		DWORD GetValue() const
		{
			return BGRA;
		}
		void SetValue(DWORD bgra)
		{
			BGRA = bgra;
		}
		void SetR(BYTE value) {
			((BYTE*)&BGRA)[2] = value;
		}
		void SetG(BYTE value) {
			((BYTE*)&BGRA)[1] = value;
		}
		void SetB(BYTE value) {
			((BYTE*)&BGRA)[0] = value;
		}
		void SetA(BYTE value) {
			((BYTE*)&BGRA)[3] = value;
		}
	public:
		// Common color constants (BGRA format)
		enum : DWORD
		{
			Transparent = 0x00000000, // 全透明

			Black = 0xFF000000,       // 黑色
			White = 0xFFFFFFFF,       // 白色
			Gray = 0xFF808080,        // 灰色
			LightGray = 0xFFD3D3D3,   // 浅灰色
			DarkGray = 0xFFA9A9A9,    // 深灰色

			Red = 0xFFFF0000,         // 红色
			DarkRed = 0xFF8B0000,     // 深红色
			LightCoral = 0xFFF08080,  // 浅珊瑚红
			Tomato = 0xFFFF6347,      // 番茄色
			Crimson = 0xFFDC143C,     // 猩红色

			Green = 0xFF00FF00,       // 绿色
			Lime = 0xFF00FF00,        // 酸橙绿（亮绿）
			DarkGreen = 0xFF006400,   // 深绿色
			LawnGreen = 0xFF7CFC00,   // 草坪绿
			PaleGreen = 0xFF98FB98,   // 苍绿色

			Blue = 0xFF0000FF,        // 蓝色
			RoyalBlue = 0xFF4169E1,   // 皇家蓝
			DodgerBlue = 0xFF1E90FF,  // 道奇蓝
			DeepSkyBlue = 0xFF00BFFF, // 深天蓝
			LightBlue = 0xFFADD8E6,   // 浅蓝色

			Yellow = 0xFF00FFFF,      // 黄色
			Gold = 0xFFFFD700,        // 金色
			LightYellow = 0xFFFFFFE0, // 浅黄色
			Khaki = 0xFFF0E68C,       // 卡其色

			Orange = 0xFFFFA500,      // 橙色
			DarkOrange = 0xFFFF8C00,  // 深橙色
			Coral = 0xFFFF7F50,       // 珊瑚色
			Salmon = 0xFFFA8072,      // 鲑鱼色

			Purple = 0xFF800080,      // 紫色
			MediumPurple = 0xFF9370DB,// 中紫色
			Indigo = 0xFF4B0082,      // 靛青色
			Violet = 0xFFEE82EE,      // 紫罗兰
			Plum = 0xFFDDA0DD,        // 李子紫

			Cyan = 0xFF00FFFF,        // 青色
			Teal = 0xFF808000,        // 水鸭色（G+B）
			Aqua = 0xFF00FFFF,        // 浅绿色（水色）

			Turquoise = 0xFF40E0D0,   // 绿松石色

			Brown = 0xFFA52A2A,       // 棕色
			Maroon = 0xFF800000,      // 栗色（褐红）
			Tan = 0xFFD2B48C,         // 茶色
			Beige = 0xFFF5F5DC,       // 米色

			Navy = 0xFF000080,        // 藏青色
			Olive = 0xFF808000,       // 橄榄色
			Silver = 0xFFC0C0C0       // 银色
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
	typedef  __EzUI__Rect<int> Rect;

	class SizeF :public __EzUI__Size<float> {
	public:
		SizeF()
		{
			Width = Height = 0;
		}
		SizeF(float width,
			float height)
		{
			Width = width;
			Height = height;
		}
		SizeF(const SizeF& __Size)
		{
			Width = __Size.Width;
			Height = __Size.Height;
		}
		SizeF(const Size& __Size)
		{
			Width = (float)__Size.Width;
			Height = (float)__Size.Height;
		}
	};

	class RectF :public __EzUI__Rect<float> {
	public:
		RectF() {
			this->X = 0;
			this->Y = 0;
			this->Width = 0;
			this->Height = 0;
		}
		RectF(const Rect& rect) {
			this->X = (float)rect.X;
			this->Y = (float)rect.Y;
			this->Width = (float)rect.Width;
			this->Height = (float)rect.Height;
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
		//转换
		static RectF Transformation(SizeMode sizeMode, const RectF& container, const SizeF& contentSize) {
			if (sizeMode == SizeMode::Stretch) {
				return container;
			}
			//容器数据
			float containerWidth = container.Width;
			float containerHeight = container.Height;
			float containerRatio = containerWidth / containerHeight;//宽高比
			//内容数据
			float contentWidth = contentSize.Width;
			float contentHeight = contentSize.Height;
			float contentRatio = contentWidth / contentHeight; //宽高比

			if (sizeMode == SizeMode::Fit) {
				if (containerRatio < contentRatio) {
					float zoomHeight = containerWidth / contentWidth * contentHeight;
					float y = (containerHeight - zoomHeight) / 2.0f + container.Y;
					return RectF(container.X, y, containerWidth, zoomHeight);
				}
				else {
					float zoomWidth = containerHeight / contentHeight * contentWidth;
					float x = (containerWidth - zoomWidth) / 2.0f + container.X;
					return RectF(x, container.Y, zoomWidth, containerHeight);
				}
			}
			if (sizeMode == SizeMode::Cover) {
				if (containerRatio < contentRatio) {
					//1000 670 容器大小
					//1000 300 内容大小
					//2233 670     缩放后的内容大小 
					float zoomWidth = containerHeight / contentHeight * contentWidth;//内容应该这么宽才对
					float x = (zoomWidth - containerWidth) / 2.0f;
					return RectF(container.X - x, container.Y, zoomWidth, containerHeight);
				}
				else {
					//1000 600 容器大小
					//400  600 内容大小
					//1000 1500     缩放后的内容大小 
					float zoomHeight = containerWidth / contentWidth * contentHeight;//内容应该这么高才对
					float y = (zoomHeight - containerHeight) / 2.0f;
					return RectF(container.X, container.Y - y, containerWidth, zoomHeight);
				}
			}
			//按照内容原大小居中显示
			if (sizeMode == SizeMode::Original) {
				float x = (container.Width - contentSize.Width) / 2.0f;
				float y = (container.Height - contentSize.Height) / 2.0f;
				return RectF(x, y, contentSize.Width, contentSize.Height);
			}
			return container;
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
			Top = WORD(Top * scale + 0.5);
			Bottom = WORD(Bottom * scale + 0.5);
			Left = WORD(Left * scale + 0.5);
			Right = WORD(Right * scale + 0.5);
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

	/// <summary>
	/// 描述边框的一些信息
	/// </summary>
	class Border {
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
			Left = WORD(Left * scale + 0.5);
			Top = WORD(Top * scale + 0.5);
			Right = WORD(Right * scale + 0.5);
			Bottom = WORD(Bottom * scale + 0.5);
			TopLeftRadius = WORD(TopLeftRadius * scale + 0.5);
			TopRightRadius = WORD(TopRightRadius * scale + 0.5);
			BottomRightRadius = WORD(BottomRightRadius * scale + 0.5);
			BottomLeftRadius = WORD(BottomLeftRadius * scale + 0.5);
		}
	};


	class IImage {
	protected:
		WORD m_frameCount = 0;//总帧数
		WORD m_framePos = 0;//当前帧率索引
	public:
		Rect Clip;//取出图像部分区域进行绘制
		Point DrawPosition;//绘制在owner矩形坐标
		ezui::Size DrawSize;//绘制在owner矩形的大小
		ImageSizeMode SizeMode = ImageSizeMode::Fit;// 图像显示模式
	public:
		virtual ~IImage() {}
		WORD FrameCount() {
			return m_frameCount;
		}
		//跳转到下一帧 并且获取下一帧的延迟
		virtual WORD NextFrame() = 0;
	};

};