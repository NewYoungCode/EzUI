#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include <string>
#include <cmath>

#ifdef EZUI_OS_WIN
#include <Windows.h>
#endif

namespace ezui {
	//size模式
	enum class SizeMode :int16_t {
		None,//未指定
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
	using ImageSizeMode = SizeMode;

#define EZUI_ALIGN_TOP  1
#define EZUI_ALIGN_BOTTOM  2
#define EZUI_ALIGN_LEFT  4
#define EZUI_ALIGN_RIGHT  8
#define EZUI_ALIGN_MID  16
#define EZUI_ALIGN_CENTER  32

	/// <summary>
	/// 水平状态下的对齐方式
	/// </summary>
	enum class HAlign :int16_t
	{
		Left = EZUI_ALIGN_LEFT,//左边对齐
		Center = EZUI_ALIGN_CENTER,//水平居中
		Right = EZUI_ALIGN_RIGHT//右边对齐
	};
	EZUI_ENUM_OPERATORS(HAlign, int16_t);

	/// <summary>
	/// 垂直状态下的对齐方式
	/// </summary>
	enum class VAlign :int16_t
	{
		Top = EZUI_ALIGN_TOP,//顶部对齐
		Middle = EZUI_ALIGN_MID,//垂直居中
		Bottom = EZUI_ALIGN_BOTTOM//底部对齐
	};
	EZUI_ENUM_OPERATORS(VAlign, int16_t);

	//包含垂直与水平对齐方式
	enum class Align :int16_t {
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上左边对齐。
		TopLeft = (int16_t)VAlign::Top | (int16_t)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上居中对齐。
		TopCenter = (int16_t)VAlign::Top | (int16_t)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上右边对齐。
		TopRight = (int16_t)VAlign::Top | (int16_t)HAlign::Right,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上左边对齐。
		MiddleLeft = (int16_t)VAlign::Middle | (int16_t)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上居中对齐。
		MiddleCenter = (int16_t)VAlign::Middle | (int16_t)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上右边对齐。
		MiddleRight = (int16_t)VAlign::Middle | (int16_t)HAlign::Right,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上左边对齐。
		BottomLeft = (int16_t)VAlign::Bottom | (int16_t)HAlign::Left,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上居中对齐。
		BottomCenter = (int16_t)VAlign::Bottom | (int16_t)HAlign::Center,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上右边对齐。
		BottomRight = (int16_t)VAlign::Bottom | (int16_t)HAlign::Right
	};
	EZUI_ENUM_OPERATORS(Align, int16_t);

	inline Align operator|(Align a, VAlign b)
	{
		return static_cast<Align>((int16_t(a) & ~(int16_t(VAlign::Top | VAlign::Middle | VAlign::Bottom))) | int16_t(b));
	}
	inline Align operator|(VAlign a, Align b)
	{
		return b | a; // 调用上面的函数
	}
	inline Align operator|(Align a, HAlign b)
	{
		return static_cast<Align>((int16_t(a) & ~(int16_t(HAlign::Left | HAlign::Center | HAlign::Right))) | int16_t(b));
	}
	inline Align operator|(HAlign a, Align b)
	{
		return b | a; // 调用上面的函数
	}
	inline bool EnumExist(Align a, VAlign b) {
		return (static_cast<int16_t>(a) & static_cast<int16_t>(b)) != 0;
	}
	inline bool EnumExist(Align a, HAlign b) {
		return (static_cast<int16_t>(a) & static_cast<int16_t>(b)) != 0;
	}

	using TextAlign = Align;

	// 字体样式 倾斜等...
	enum class FontStyle :int16_t
	{
		/// <summary>
		/// 正常字体
		/// </summary>
		Normal,
		/// <summary>
		/// 倾斜字体（斜体，非正式斜体）
		/// </summary>
		Oblique,
		/// <summary>
		/// 斜体字体（正式斜体）
		/// </summary>
		Italic
	};

	//描边样式
	enum class StrokeStyle :int16_t
	{
		None,//无
		Solid,//实线
		Dash//虚线
	};
	//填充样式
	enum class FillStyle :int16_t
	{
		Solid,      // 实心填充
		Diagonal,   // 斜线填充
		Cross,      // 交叉斜线填充
		Horizontal, // 水平线填充
		Vertical    // 垂直线填充
	};

	namespace detail {

		template<typename T>
		class BasicSize
		{
		public:
			T Width;
			T Height;
		public:
			BasicSize()
			{
				Width = Height = 0;
			}
			BasicSize(const BasicSize& size)
			{
				Width = size.Width;
				Height = size.Height;
			}
			BasicSize(T width,
				T height)
			{
				Width = width;
				Height = height;
			}

#ifdef EZUI_OS_WIN
			BasicSize(const SIZE& size)
			{
				Width = size.cx;
				Height = size.cy;
			}
#endif // EZUI_OS_WIN

			bool operator!=(const BasicSize& right) const
			{
				return !(Width == right.Width && Height == right.Height);
			}
			void Scale(float scale) {
				Width = std::round((Width * scale));
				Height = std::round((Height * scale));
			}

			template<typename U>
			BasicSize operator*(U scale) const {
				return BasicSize(
					T(std::round(Width * scale)),
					T(std::round(Height * scale))
				);
			}
			virtual bool Equals(const BasicSize& sz) const
			{
				return (Width == sz.Width) && (Height == sz.Height);
			}
			bool Empty() const
			{
				return (Width == 0 && Height == 0);
			}
			BasicSize operator+(const BasicSize& sz) const
			{
				return BasicSize(Width + sz.Width,
					Height + sz.Height);
			}
			BasicSize operator-(const BasicSize& sz) const
			{
				return BasicSize(Width - sz.Width,
					Height - sz.Height);
			}

			bool operator==(const BasicSize& right) const
			{
				return Equals(right);
			}
		public:
			static BasicSize Rescale(const BasicSize& size, float fromScale, float toScale) {
				if (std::fabsf(fromScale - toScale) <= EZUI_FLOAT_EPSILON) {
					return size;
				}
				float scale = toScale / fromScale;
				return BasicSize(std::round(size.Width * scale), std::round(size.Height * scale));
			}
		};

		template<typename T>
		class BasicPoint
		{
		public:
			T X;
			T Y;
		public:
			BasicPoint()
			{
				X = Y = 0;
			}

			BasicPoint(const BasicPoint& point)
			{
				X = point.X;
				Y = point.Y;
			}

#ifdef EZUI_OS_WIN
			BasicPoint(const POINT& point) {
				X = point.x;
				Y = point.y;
			}
#endif // EZUI_OS_WIN

			BasicPoint(T x,
				T y)
			{
				X = x;
				Y = y;
			}
			void Scale(float scale) {
				X = std::round((X * scale));
				Y = std::round((Y * scale));
			}

			template<typename U>
			BasicPoint operator*(U scale) const {
				return BasicPoint(
					T(std::round(X * scale)),
					T(std::round(Y * scale))
				);
			}

			bool Equals(const BasicPoint& point)const
			{
				return (X == point.X) && (Y == point.Y);
			}
			BasicPoint operator+(const BasicPoint& point) const
			{
				return BasicPoint(X + point.X, Y + point.Y);
			}
			BasicPoint operator-(const BasicPoint& point) const
			{
				return BasicPoint(X - point.X, Y - point.Y);
			}
			bool operator==(const BasicPoint& point) const
			{
				return Equals(point);
			}
			static BasicPoint Rescale(const BasicPoint& point, float fromScale, float toScale) {
				if (std::fabsf(fromScale - toScale) <= EZUI_FLOAT_EPSILON) {
					return point;
				}
				float scale = toScale / fromScale;
				return BasicPoint(std::round(point.X * scale), std::round(point.Y * scale));
			}

		};

		template<typename T>
		class BasicRect
		{
		public:
			T X;
			T Y;
			T Width;
			T Height;
		public:

			BasicRect()
			{
				X = Y = Width = Height = 0;
			}

			BasicRect(T x,
				T y,
				T width,
				T height)
			{
				X = x;
				Y = y;
				Width = width;
				Height = height;
			}
			BasicRect(const BasicPoint<T>& position, const BasicSize<T>& size) {
				X = position.X;
				Y = position.Y;
				Width = size.Width;
				Height = size.Height;
			}


#ifdef EZUI_OS_WIN
			BasicRect(const RECT& rect) {
				X = rect.left;
				Y = rect.top;
				Width = rect.right - rect.left;
				Height = rect.bottom - rect.top;
			}
			RECT ToRECT() const {
				return RECT{ (LONG)GetLeft(), (LONG)GetTop(), (LONG)GetRight(), (LONG)GetBottom() };
			}
#endif

			template<typename U>
			BasicRect operator*(U scale) const {
				return BasicRect(
					T(std::round(X * scale)),
					T(std::round(Y * scale)),
					T(std::round(Width * scale)),
					T(std::round(Height * scale))
				);
			}

			BasicPoint<T> GetPosition() const
			{
				return BasicPoint<T>{ X, Y };
			}

			BasicSize<T> GetSize() const
			{
				return BasicSize<T>(Width, Height);
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

			virtual const BasicRect& Scale(float scale) {
				X = (T)std::round(X * scale);
				Y = (T)std::round(Y * scale);
				Width = (T)std::round(Width * scale);
				Height = (T)std::round(Height * scale);
				return *this;
			}

			virtual bool Equals(const BasicRect& rect) const
			{
				return (X == rect.X && Y == rect.Y && Width == rect.Width && Height == rect.Height);
			}

			bool  operator == (const BasicRect& right) {
				return Equals(right);
			}

			BasicRect& operator+=(T value) {
				X -= value;
				Y -= value;
				Width += value * 2;
				Height += value * 2;
				return *this;
			}
			BasicRect& operator-=(T value) {
				X += value;
				Y += value;
				Width -= value * 2;
				Height -= value * 2;
				return *this;
			}
			BasicRect operator+(T value) const {
				BasicRect out = *this;
				out += value;
				return out;
			}
			BasicRect operator-(T value) const {
				BasicRect out = *this;
				out -= value;
				return out;
			}

			bool Contains(T x,
				T y) const
			{
				return x >= X && x < X + Width &&
					y >= Y && y < Y + Height;
			}

			bool Contains(const BasicPoint<T>& pt) const
			{
				return Contains(pt.X, pt.Y);
			}

			bool Contains(const BasicRect& rect) const
			{
				return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
					(Y <= rect.Y) && (rect.GetBottom() <= GetBottom());

			}

			void Inflate(T dx,
				T dy)
			{
				X -= dx;
				Y -= dy;
				Width += 2 * dx;
				Height += 2 * dy;
			}

			void Inflate(const BasicPoint<T>& point)
			{
				Inflate(point.X, point.Y);
			}

			bool Intersect(const BasicRect& rect)
			{
				return Intersect(*this, *this, rect);
			}
			static BasicRect Rescale(const BasicRect& rect, float fromScale, float toScale) {
				if (std::fabsf(fromScale - toScale) <= EZUI_FLOAT_EPSILON) {
					return rect;
				}
				BasicRect newRect = rect;
				float scale = toScale / fromScale;
				newRect.X = std::round(rect.X * scale);
				newRect.Y = std::round(rect.Y * scale);
				newRect.Width = std::round(rect.Width * scale);
				newRect.Height = std::round(rect.Height * scale);
				return newRect;
			}
			static bool Intersect(BasicRect& c,
				const BasicRect& a,
				const BasicRect& b)
			{
				T right = EZUI_MIN(a.GetRight(), b.GetRight());
				T bottom = EZUI_MIN(a.GetBottom(), b.GetBottom());
				T left = EZUI_MAX(a.GetLeft(), b.GetLeft());
				T top = EZUI_MAX(a.GetTop(), b.GetTop());

				c.X = left;
				c.Y = top;
				c.Width = right - left;
				c.Height = bottom - top;
				return !c.IsEmptyArea();
			}

			bool IntersectsWith(const BasicRect& rect) const
			{
				return (GetLeft() < rect.GetRight() &&
					GetTop() < rect.GetBottom() &&
					GetRight() > rect.GetLeft() &&
					GetBottom() > rect.GetTop());
			}

			static bool Union(BasicRect& c,
				const BasicRect& a,
				const BasicRect& b)
			{

				if (a.IsEmptyArea()) {
					c = b;
					return !c.IsEmptyArea();
				}
				if (b.IsEmptyArea()) {
					c = a;
					return !c.IsEmptyArea();
				}

				T right = EZUI_MAX(a.GetRight(), b.GetRight());
				T bottom = EZUI_MAX(a.GetBottom(), b.GetBottom());
				T left = EZUI_MIN(a.GetLeft(), b.GetLeft());
				T top = EZUI_MIN(a.GetTop(), b.GetTop());

				c.X = left;
				c.Y = top;
				c.Width = right - left;
				c.Height = bottom - top;
				return !c.IsEmptyArea();
			}
			void Offset(const BasicPoint<T>& point)
			{
				Offset(point.X, point.Y);
			}
			void Offset(T dx,
				T dy)
			{
				X += dx;
				Y += dy;
			}
			virtual ~BasicRect() {}
		};

		template<typename T>
		class BasicLine {
		public:
			BasicPoint<T> pointA;
			BasicPoint<T> pointB;
		public:
			BasicLine() {
				pointA.X = 0;
				pointA.Y = 0;
				pointB.X = 0;
				pointB.Y = 0;
			}
			BasicLine(const BasicPoint<T>& _pointA, const BasicPoint<T>& _pointB) {
				this->pointA = _pointA;
				this->pointB = _pointB;
			}
		};
	};

	using Point = detail::BasicPoint<int>;
	using PointF = detail::BasicPoint<float>;
	using Line = detail::BasicLine<int>;
	using LineF = detail::BasicLine<float>;
	using Size = detail::BasicSize<int>;
	using Rect = detail::BasicRect<int>;
	// 二维向量
	using Vec2 = PointF;
	// 边框样式
	using BorderStyle = StrokeStyle;

	class Color
	{
	protected:
		uint32_t m_BGRA = 0;
#ifdef DEBUG
		//用于调试
		std::string m_colorStr;
#endif
	public:
		Color() {}
		Color(
			const uint8_t& r,
			const uint8_t& g,
			const uint8_t& b,
			const uint8_t& a = 255)
		{
			m_BGRA |= static_cast<uint32_t>(b);         // 蓝色占最低8位
			m_BGRA |= static_cast<uint32_t>(g) << 8;    // 绿色占第2字节
			m_BGRA |= static_cast<uint32_t>(r) << 16;   // 红色占第3字节
			m_BGRA |= static_cast<uint32_t>(a) << 24;   // Alpha 占最高字节
		}
		Color(uint32_t bgra)
		{
			m_BGRA = bgra;
		}
		virtual ~Color() {}
		uint8_t GetR() const {
			return (m_BGRA >> 16) & 0xFF;
		}
		uint8_t GetG() const {
			return (m_BGRA >> 8) & 0xFF;
		}
		uint8_t GetB() const {
			return m_BGRA & 0xFF;
		}
		uint8_t GetA() const {
			return (m_BGRA >> 24) & 0xFF;
		}
		uint32_t GetValue() const
		{
			return m_BGRA;
		}
		void SetValue(uint32_t bgra)
		{
			m_BGRA = bgra;
		}
		void SetR(uint8_t value) {
			m_BGRA = (m_BGRA & 0xFF00FFFF) | (uint32_t(value) << 16);
		}
		void SetG(uint8_t value) {
			m_BGRA = (m_BGRA & 0xFFFF00FF) | (uint32_t(value) << 8);
		}
		void SetB(uint8_t value) {
			m_BGRA = (m_BGRA & 0xFFFFFF00) | uint32_t(value);
		}
		void SetA(uint8_t value) {
			m_BGRA = (m_BGRA & 0x00FFFFFF) | (uint32_t(value) << 24);
		}
		bool operator==(const Color& right) const {
			return this->GetValue() == right.GetValue();
		}
		bool operator!=(const Color& right) const {
			return !(*this == right);
		}
	public:
		// HSV 转 RGB
		static Color HSVtoColor(float h, float s, float v, uint8_t a = 255) {
			float r, g, b;
			int i = (int)(h * 6);
			float f = h * 6 - i;
			float p = v * (1 - s);
			float q = v * (1 - f * s);
			float t = v * (1 - (1 - f) * s);
			switch (i % 6) {
			case 0: r = v; g = t; b = p; break;
			case 1: r = q; g = v; b = p; break;
			case 2: r = p; g = v; b = t; break;
			case 3: r = p; g = q; b = v; break;
			case 4: r = t; g = p; b = v; break;
			case 5: r = v; g = p; b = q; break;
			default: r = g = b = 0; break;
			}
			return Color((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), a);
		}
		// RGB 转 HSV
		static void ColorToHSV(const Color& c, float& h, float& s, float& v) {
			float r = c.GetR() / 255.0f;
			float g = c.GetG() / 255.0f;
			float b = c.GetB() / 255.0f;
			float maxC = EZUI_MAX(r, EZUI_MAX(g, b));
			float minC = EZUI_MIN(r, EZUI_MIN(g, b));
			float delta = maxC - minC;
			v = maxC;
			s = (maxC == 0) ? 0 : delta / maxC;
			if (delta == 0) { h = 0; }
			else if (maxC == r) { h = (g - b) / delta; if (h < 0) h += 6; h /= 6; }
			else if (maxC == g) { h = ((b - r) / delta + 2) / 6; }
			else { h = ((r - g) / delta + 4) / 6; }
		}
		//构建一个Color
		static Color Make(const std::string& colorStr, bool* _isGood = NULL) {
			Color color;
			bool _bCopy;
			bool* isGood = _isGood ? _isGood : &_bCopy;
			*isGood = false;
			do {
				if (colorStr == "red") {
					color = Color::Red; *isGood = true; break;
				}
				else if (colorStr == "yellow") {
					color = Color::Yellow; *isGood = true; break;
				}
				else if (colorStr == "blue") {
					color = Color::Blue; *isGood = true; break;
				}
				else if (colorStr == "black") {
					color = Color::Black; *isGood = true; break;
				}
				else if (colorStr == "white") {
					color = Color::White; *isGood = true; break;
				}
				else if (colorStr == "green") {
					color = Color::Green; *isGood = true; break;
				}
				else if (colorStr == "orange") {
					color = Color::Orange; *isGood = true; break;
				}
				else if (colorStr == "purple") {
					color = Color::Purple; *isGood = true; break;
				}
				else if (colorStr == "gray") {
					color = Color::Gray; *isGood = true; break;
				}
				else if (colorStr == "transparent") {
					color = Color::Transparent; *isGood = true; break;
				}
				else if (colorStr.find("#") == 0 && (colorStr.size() == 7 || colorStr.size() == 9)) { //"#4e6ef2"
					unsigned int r, g, b, a = 255;
					int matchCount = std::sscanf(colorStr.c_str() + 1, "%2x%2x%2x%2x", &r, &g, &b, &a);
					if (matchCount == 3 || matchCount == 4)
					{
						color = Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a));
						*isGood = true;
					}
					break;
				}
				else if (colorStr.find("rgb") == 0) {
					size_t pos1 = colorStr.find('(');
					size_t pos2 = colorStr.rfind(')');
					if (pos1 == std::string::npos || pos2 == std::string::npos || pos2 <= pos1) {
						break;
					}
					const char* p = colorStr.c_str() + pos1 + 1;
					uint8_t r = 0, g = 0, b = 0;
					float alphaRatio = 1.0f;
					int ret = std::sscanf(p, " %hhu , %hhu , %hhu , %f ", &r, &g, &b, &alphaRatio);
					if (ret < 3) {
						break;
					}
					if (ret >= 4) {
						// 判断是否包含百分号
						auto percentPos = colorStr.rfind('%');
						if (percentPos != std::string::npos && percentPos < pos2) {
							alphaRatio /= 100.0f;
						}
						// clamp
						if (alphaRatio < 0.0f) alphaRatio = 0.0f;
						if (alphaRatio > 1.0f) alphaRatio = 1.0f;
					}
					uint8_t a = static_cast<uint8_t>(alphaRatio * 255.0f + 0.5f);
					color = Color(r, g, b, a);
					*isGood = true;
					break;
				}
			} while (false);
#ifdef DEBUG
			if (*isGood) {
				color.m_colorStr = colorStr;
			}
#endif // DEBUG
			return color;
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

	class SizeF :public  detail::BasicSize<float> {
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
		SizeF(const SizeF& size)
		{
			Width = size.Width;
			Height = size.Height;
		}
		bool Equals(const SizeF& size) const
		{
			bool a = (std::fabs(Width - size.Width) < EZUI_FLOAT_EPSILON);
			bool b = (std::fabs(Height - size.Height) < EZUI_FLOAT_EPSILON);
			return (a && b);
		}
		SizeF(const Size& size)
		{
			Width = (float)size.Width;
			Height = (float)size.Height;
		}
	};

	class RectF :public  detail::BasicRect<float> {
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
		virtual bool Equals(const BasicRect& rect) const override {
			bool a = (std::fabs(X - rect.X) < EZUI_FLOAT_EPSILON);
			bool b = (std::fabs(Y - rect.Y) < EZUI_FLOAT_EPSILON);
			bool c = (std::fabs(Width - rect.Width) < EZUI_FLOAT_EPSILON);
			bool d = (std::fabs(Height - rect.Height) < EZUI_FLOAT_EPSILON);
			return (a && b && c && d);
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

	//用于描述上下左右的距离
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
		bool operator==(const Distance& right) const {
			return Left == right.Left
				&& Top == right.Top
				&& Right == right.Right
				&& Bottom == right.Bottom;
		}
		bool operator!=(const Distance& right) const {
			return !(*this == right);
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

	namespace detail {
		class IImage {
		public:
			//像素格式
			enum class PixelFormat :int16_t {
				PixelFormat32bppPBGRA = 0,//预乘Alpha的32位BGRA格式
				PixelFormat32bppPRGBA = 1,//预乘Alpha的32位RGBA格式
			};
		protected:
			int m_frameCount = 0;//总帧数
			int m_framePos = 0;//当前帧率索引
		public:
			Rect Clip;//取出图像部分区域进行绘制
			Point DrawPosition;//绘制在owner矩形坐标
			ezui::Size DrawSize;//绘制在owner矩形的大小
			ImageSizeMode SizeMode = ImageSizeMode::Fit;// 图像显示模式
		private:
			IImage(const IImage&) = delete;            // 禁止拷贝构造
			IImage& operator=(const IImage&) = delete; // 禁止拷贝赋值
			IImage(IImage&&) = delete;                 // 禁止移动构造
			IImage& operator=(IImage&&) = delete;      // 禁止移动赋值
		protected:
			IImage() {};
		public:
			virtual ~IImage() {}
			int FrameCount() {
				return m_frameCount;
			}
			//跳转到下一帧 并且获取下一帧的延迟
			virtual int NextFrame() = 0;
		};
	};

	//渐变类型
	enum class GradientType :int16_t
	{
		Linear,// 线性渐变
		Radial,// 径向渐变
		Conic  // 锥形渐变
	};
	//渐变色的一个颜色点
	struct GradientStop
	{
		float position;   // 位置，范围为0.0到1.0
		Color color;// 颜色
	};
	//渐变扩展模式
	enum class GradientSpread :int16_t
	{
		Pad,        // 超出范围使用边界色
		Repeat,     // 重复
		Reflect     // 镜像
	};

	//渐变配置类
	class Gradient
	{
	private:
		GradientType m_type = GradientType::Linear;
		GradientSpread m_spread = GradientSpread::Pad;
		Vec2 m_start;
		Vec2 m_end;
		Vec2 m_center;
		float m_radius = 0.0f;
		float m_angle = 0.0f;
		std::vector<GradientStop> m_stops;
		Gradient() = default;
	public:
		//创建线性渐变(从start到end)
		static Gradient Linear(Vec2 start, Vec2 end) {
			Gradient g;
			g.m_type = GradientType::Linear;
			g.m_start = start;
			g.m_end = end;
			return g;
		}
		//创建径向渐变(以center为圆心,radius为半径)
		static Gradient Radial(Vec2 center, float radius) {
			Gradient g;
			g.m_type = GradientType::Radial;
			g.m_center = center;
			g.m_radius = radius;
			return g;
		}
		//创建锥形渐变(以center为圆心,startAngle为起始角度)
		static Gradient Conic(Vec2 center, float startAngle) {
			Gradient g;
			g.m_type = GradientType::Conic;
			g.m_center = center;
			g.m_angle = startAngle;
			return g;
		}
		//添加颜色停靠点(pos范围0.0~1.0)
		Gradient& AddStop(float pos, const Color& color) {
			m_stops.push_back({ pos, color });
			return *this;
		}
		//设置渐变扩展模式
		Gradient& SetSpread(GradientSpread spread) {
			m_spread = spread;
			return *this;
		}
		//获取所有颜色停靠点
		const std::vector<GradientStop>& Stops() const {
			return m_stops;
		}
		//获取渐变类型
		GradientType Type() const {
			return m_type;
		}
		//获取线性渐变起点
		const Vec2& Start() const { return m_start; }
		//获取线性渐变终点
		const Vec2& End() const { return m_end; }
		//获取径向/锥形渐变圆心
		const Vec2& Center() const { return m_center; }
		//获取径向渐变半径
		float Radius() const { return m_radius; }
		//获取锥形渐变起始角度
		float Angle() const { return m_angle; }
		//获取渐变扩展模式
		GradientSpread Spread() const { return m_spread; }
	};

};

#ifdef _MSC_VER
#pragma warning(pop)
#endif