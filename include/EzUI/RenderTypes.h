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
		uint32_t BGRA = 0;
	public:
		__EzUI__Color() {}
		__EzUI__Color(
			const uint8_t& r,
			const uint8_t& g,
			const uint8_t& b,
			const uint8_t& a = 255)
		{
			BGRA |= static_cast<uint32_t>(b);         // 蓝色占最低8位
			BGRA |= static_cast<uint32_t>(g) << 8;    // 绿色占第2字节
			BGRA |= static_cast<uint32_t>(r) << 16;   // 红色占第3字节
			BGRA |= static_cast<uint32_t>(a) << 24;   // Alpha 占最高字节
		}
		__EzUI__Color(uint32_t bgra)
		{
			BGRA = bgra;
		}
		virtual ~__EzUI__Color() {}
		uint8_t GetR() const {
			return (BGRA >> 16) & 0xFF;
		}
		uint8_t GetG() const {
			return (BGRA >> 8) & 0xFF;
		}
		uint8_t GetB() const {
			return BGRA & 0xFF;
		}
		uint8_t GetA() const {
			return (BGRA >> 24) & 0xFF;
		}
		uint32_t GetValue() const
		{
			return BGRA;
		}
		void SetValue(uint32_t bgra)
		{
			BGRA = bgra;
		}
		void SetR(uint8_t value) {
			BGRA = (BGRA & 0xFF00FFFF) | (uint32_t(value) << 16);
		}
		void SetG(uint8_t value) {
			BGRA = (BGRA & 0xFFFF00FF) | (uint32_t(value) << 8);
		}
		void SetB(uint8_t value) {
			BGRA = (BGRA & 0xFFFFFF00) | uint32_t(value);
		}
		void SetA(uint8_t value) {
			BGRA = (BGRA & 0x00FFFFFF) | (uint32_t(value) << 24);
		}

	public:
		// Common color constants (BGRA format)
		enum : uint32_t
		{
			Transparent = 0x00000000, // 全透明
			Red = 0xFFFF0000,     // 红色
			Yellow = 0xFFFFFF00,  // 黄色
			Blue = 0xFF0000FF,    // 蓝色
			Black = 0xFF000000,   // 黑色
			White = 0xFFFFFFFF,   // 白色
			Green = 0xFF008000,   // 绿色
			Orange = 0xFFFFA500,  // 橙色
			Purple = 0xFF800080,  // 紫色
			Gray = 0xFF808080     // 灰色
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

	//可以控制值开关的模板类
	template<typename T>
	class Value {
		bool m_enable = false;
		T m_value{};
	public:
		Value() = default;
		Value(const T& value) : m_value(value), m_enable(true) {}
		Value& operator=(const T& value) {
			m_value = value;
			m_enable = true;
			return *this;
		}
		Value(const Value& rightValue)
			: m_value(rightValue.m_value), m_enable(rightValue.m_enable) {
		}
		Value& operator=(const Value& rightValue) {
			if (this != &rightValue) {
				m_value = rightValue.m_value;
				m_enable = rightValue.m_enable;
			}
			return *this;
		}
		void Set(const T& value) { m_value = value; }
		void SetEnabled(bool bEnable) { m_enable = bEnable; }
		bool IsEnabled() const { return m_enable; }
		operator T() const { return m_enable ? m_value : T{}; }
		T& operator->() { return m_value; }
	};


	struct Distance {
	public:
		int16_t Left, Top, Right, Bottom;
		Distance() {
			Left = Top = Right = Bottom = 0;
		}
		Distance(int16_t distanceAll) {
			Left = Top = Right = Bottom = distanceAll;
		}
		Distance& operator=(int16_t distanceAll) {
			Left = Top = Right = Bottom = distanceAll;
			return *this;
		}
		void Scale(float scale) {
			Top = int16_t(Top * scale + 0.5);
			Bottom = int16_t(Bottom * scale + 0.5);
			Left = int16_t(Left * scale + 0.5);
			Right = int16_t(Right * scale + 0.5);
		}
		//获取垂直所占空间
		int16_t GetVSpace() const {
			return Top + Bottom;
		}
		//获取水平所占空间
		int16_t GetHSpace() const {
			return Left + Right;
		}
	};


	class IImage {
	protected:
		int m_frameCount = 0;//总帧数
		int m_framePos = 0;//当前帧率索引
	public:
		Rect Clip;//取出图像部分区域进行绘制
		Point DrawPosition;//绘制在owner矩形坐标
		ezui::Size DrawSize;//绘制在owner矩形的大小
		ImageSizeMode SizeMode = ImageSizeMode::Fit;// 图像显示模式
	public:
		virtual ~IImage() {}
		int FrameCount() {
			return m_frameCount;
		}
		//跳转到下一帧 并且获取下一帧的延迟
		virtual int NextFrame() = 0;
	};

};