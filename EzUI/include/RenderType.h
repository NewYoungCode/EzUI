#pragma once
#include <Windows.h>
namespace EzUI {

	namespace RenderType {
		typedef DWORD ARGB;
		typedef float REAL;
#define REAL_MAX            FLT_MAX
#define REAL_MIN            FLT_MIN
#define REAL_TOLERANCE     (FLT_MIN * 100)
#define REAL_EPSILON        1.192092896e-07F        /* FLT_EPSILON */

		class Size;
		class SizeF;
		class Point;
		class PointF;
		class Rect;
		class RectF;

		class SizeF
		{
		public:
			REAL Width;
			REAL Height;
		public:
			SizeF()
			{
				Width = Height = 0.0f;
			}

			SizeF(IN const SizeF& size)
			{
				Width = size.Width;
				Height = size.Height;
			}

			SizeF(IN REAL width,
				IN REAL height)
			{
				Width = width;
				Height = height;
			}

			SizeF operator+(IN const SizeF& sz) const
			{
				return SizeF(Width + sz.Width,
					Height + sz.Height);
			}

			SizeF operator-(IN const SizeF& sz) const
			{
				return SizeF(Width - sz.Width,
					Height - sz.Height);
			}

			bool Equals(IN const SizeF& sz) const
			{
				return (Width == sz.Width) && (Height == sz.Height);
			}

			bool Empty() const
			{
				return (Width == 0.0f && Height == 0.0f);
			}


		};

		class Size
		{
		public:
			INT Width;
			INT Height;
		public:
			Size()
			{
				Width = Height = 0;
			}

			Size(IN const Size& size)
			{
				Width = size.Width;
				Height = size.Height;
			}

			Size(IN INT width,
				IN INT height)
			{
				Width = width;
				Height = height;
			}

			Size operator+(IN const Size& sz) const
			{
				return Size(Width + sz.Width,
					Height + sz.Height);
			}

			Size operator-(IN const Size& sz) const
			{
				return Size(Width - sz.Width,
					Height - sz.Height);
			}

			bool operator==(IN const Size& _right) const
			{
				return (Width == _right.Width && Height == _right.Height);
			}
			bool operator!=(IN const Size& _right) const
			{
				return !(Width == _right.Width && Height == _right.Height);
			}

			bool Equals(IN const Size& sz) const
			{
				return (Width == sz.Width) && (Height == sz.Height);
			}

			bool Empty() const
			{
				return (Width == 0 && Height == 0);
			}


		};

		class PointF
		{
		public:
			REAL X;
			REAL Y;
		public:
			PointF()
			{
				X = Y = 0.0f;
			}

			PointF(IN const PointF& point)
			{
				X = point.X;
				Y = point.Y;
			}

			PointF(IN const SizeF& size)
			{
				X = size.Width;
				Y = size.Height;
			}

			PointF(IN REAL x,
				IN REAL y)
			{
				X = x;
				Y = y;
			}

			PointF operator+(IN const PointF& point) const
			{
				return PointF(X + point.X,
					Y + point.Y);
			}

			PointF operator-(IN const PointF& point) const
			{
				return PointF(X - point.X,
					Y - point.Y);
			}

			bool Equals(IN const PointF& point)
			{
				return (X == point.X) && (Y == point.Y);
			}


		};

		class Point
		{
		public:
			INT X;
			INT Y;
		public:
			Point()
			{
				X = Y = 0;
			}

			Point(IN const Point& point)
			{
				X = point.X;
				Y = point.Y;
			}

			Point(IN const Size& size)
			{
				X = size.Width;
				Y = size.Height;
			}

			Point(IN INT x,
				IN INT y)
			{
				X = x;
				Y = y;
			}

			Point operator+(IN const Point& point) const
			{
				return Point(X + point.X,
					Y + point.Y);
			}

			Point operator-(IN const Point& point) const
			{
				return Point(X - point.X,
					Y - point.Y);
			}

			bool Equals(IN const Point& point)
			{
				return (X == point.X) && (Y == point.Y);
			}


		};
		class RectF
		{
		public:
			REAL X;
			REAL Y;
			REAL Width;
			REAL Height;
		public:

			RectF()
			{
				X = Y = Width = Height = 0.0f;
			}

			RectF(IN REAL x,
				IN REAL y,
				IN REAL width,
				IN REAL height)
			{
				X = x;
				Y = y;
				Width = width;
				Height = height;
			}
			/*		RectF(const Rect& rect) {
						X = (FLOAT)rect.X;
						Y = (FLOAT)rect.Y;
						Width = (FLOAT)rect.Width;
						Height = (FLOAT)rect.Height;
					}*/

			RectF(IN const PointF& location,
				IN const SizeF& size)
			{
				X = location.X;
				Y = location.Y;
				Width = size.Width;
				Height = size.Height;
			}

			RectF* Clone() const
			{
				return new RectF(X, Y, Width, Height);
			}

			void GetLocation(OUT PointF* point) const
			{
				point->X = X;
				point->Y = Y;
			}

			void GetSize(OUT SizeF* size) const
			{
				size->Width = Width;
				size->Height = Height;
			}

			void GetBounds(OUT RectF* rect) const
			{
				rect->X = X;
				rect->Y = Y;
				rect->Width = Width;
				rect->Height = Height;
			}

			REAL GetLeft() const
			{
				return X;
			}

			REAL GetTop() const
			{
				return Y;
			}

			REAL GetRight() const
			{
				return X + Width;
			}

			REAL GetBottom() const
			{
				return Y + Height;
			}

			bool IsEmptyArea() const
			{
				return (Width <= REAL_EPSILON) || (Height <= REAL_EPSILON);
			}

			bool Equals(IN const RectF& rect) const
			{
				return X == rect.X &&
					Y == rect.Y &&
					Width == rect.Width &&
					Height == rect.Height;
			}

			bool Contains(IN REAL x,
				IN REAL y) const
			{
				return x >= X && x < X + Width &&
					y >= Y && y < Y + Height;
			}

			bool Contains(IN const PointF& pt) const
			{
				return Contains(pt.X, pt.Y);
			}

			bool Contains(IN const RectF& rect) const
			{
				return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
					(Y <= rect.Y) && (rect.GetBottom() <= GetBottom());
			}

			void Inflate(IN REAL dx,
				IN REAL dy)
			{
				X -= dx;
				Y -= dy;
				Width += 2 * dx;
				Height += 2 * dy;
			}

			void Inflate(IN const PointF& point)
			{
				Inflate(point.X, point.Y);
			}

			bool Intersect(IN const RectF& rect)
			{
				return Intersect(*this, *this, rect);
			}

			static bool Intersect(OUT RectF& c,
				IN const RectF& a,
				IN const RectF& b)
			{
				REAL right = min(a.GetRight(), b.GetRight());
				REAL bottom = min(a.GetBottom(), b.GetBottom());
				REAL left = max(a.GetLeft(), b.GetLeft());
				REAL top = max(a.GetTop(), b.GetTop());

				c.X = left;
				c.Y = top;
				c.Width = right - left;
				c.Height = bottom - top;
				return !c.IsEmptyArea();
			}

			bool IntersectsWith(IN const RectF& rect) const
			{
				return (GetLeft() < rect.GetRight() &&
					GetTop() < rect.GetBottom() &&
					GetRight() > rect.GetLeft() &&
					GetBottom() > rect.GetTop());
			}

			static bool Union(OUT RectF& c,
				IN const RectF& a,
				IN const RectF& b)
			{
				REAL right = max(a.GetRight(), b.GetRight());
				REAL bottom = max(a.GetBottom(), b.GetBottom());
				REAL left = min(a.GetLeft(), b.GetLeft());
				REAL top = min(a.GetTop(), b.GetTop());

				c.X = left;
				c.Y = top;
				c.Width = right - left;
				c.Height = bottom - top;
				return !c.IsEmptyArea();
			}

			void Offset(IN const PointF& point)
			{
				Offset(point.X, point.Y);
			}

			void Offset(IN REAL dx,
				IN REAL dy)
			{
				X += dx;
				Y += dy;
			}


		};
		class Rect
		{
		public:
			INT X;
			INT Y;
			INT Width;
			INT Height;
		public:

			Rect()
			{
				X = Y = Width = Height = 0;
			}

			Rect(IN INT x,
				IN INT y,
				IN INT width,
				IN INT height)
			{
				X = x;
				Y = y;
				Width = width;
				Height = height;
			}

			Rect(IN const Point& location,
				IN const Size& size)
			{
				X = location.X;
				Y = location.Y;
				Width = size.Width;
				Height = size.Height;
			}

			Rect* Clone() const
			{
				return new Rect(X, Y, Width, Height);
			}

			void GetLocation(OUT Point* point) const
			{
				point->X = X;
				point->Y = Y;
			}
			Point GetLocation() const
			{
				return Point{ X,Y };
			}

			void GetSize(OUT Size* size) const
			{
				size->Width = Width;
				size->Height = Height;
			}
			Size GetSize() const
			{
				return Size{ Width,Height };
			}

			void GetBounds(OUT Rect* rect) const
			{
				rect->X = X;
				rect->Y = Y;
				rect->Width = Width;
				rect->Height = Height;
			}

			INT GetLeft() const
			{
				return X;
			}

			INT GetTop() const
			{
				return Y;
			}

			INT GetRight() const
			{
				return X + Width;
			}

			INT GetBottom() const
			{
				return Y + Height;
			}

			bool IsEmptyArea() const
			{
				return (Width <= 0) || (Height <= 0);
			}

			bool Equals(IN const Rect& rect) const
			{
				return X == rect.X &&
					Y == rect.Y &&
					Width == rect.Width &&
					Height == rect.Height;
			}

			bool  operator == (const Rect& right) {
				return Equals(right);
			}

			bool Contains(IN INT x,
				IN INT y) const
			{
				return x >= X && x < X + Width &&
					y >= Y && y < Y + Height;
			}

			bool Contains(IN const Point& pt) const
			{
				return Contains(pt.X, pt.Y);
			}

			bool Contains(IN const Rect& rect) const
			{
				return (X <= rect.X) && (rect.GetRight() <= GetRight()) &&
					(Y <= rect.Y) && (rect.GetBottom() <= GetBottom());

			}

			void Inflate(IN INT dx,
				IN INT dy)
			{
				X -= dx;
				Y -= dy;
				Width += 2 * dx;
				Height += 2 * dy;
			}

			void Inflate(IN const Point& point)
			{
				Inflate(point.X, point.Y);
			}

			bool Intersect(IN const Rect& rect)
			{
				return Intersect(*this, *this, rect);
			}
			RECT WinRECT() const {
				return RECT{ X,Y,GetRight(),GetBottom() };
			}

			static bool Intersect(OUT Rect& c,
				IN const Rect& a,
				IN const Rect& b)
			{
				INT right = min(a.GetRight(), b.GetRight());
				INT bottom = min(a.GetBottom(), b.GetBottom());
				INT left = max(a.GetLeft(), b.GetLeft());
				INT top = max(a.GetTop(), b.GetTop());

				c.X = left;
				c.Y = top;
				c.Width = right - left;
				c.Height = bottom - top;
				return !c.IsEmptyArea();
			}

			bool IntersectsWith(IN const Rect& rect) const
			{
				return (GetLeft() < rect.GetRight() &&
					GetTop() < rect.GetBottom() &&
					GetRight() > rect.GetLeft() &&
					GetBottom() > rect.GetTop());
			}

			static bool Union(OUT Rect& c,
				IN const Rect& a,
				IN const Rect& b)
			{
				/*	if (a.IsEmptyArea()) {
						c = b;
						return !c.IsEmptyArea();
					}
					else if (b.IsEmptyArea()) {
						c = a;
						return !c.IsEmptyArea();
					}*/

				INT right = max(a.GetRight(), b.GetRight());
				INT bottom = max(a.GetBottom(), b.GetBottom());
				INT left = min(a.GetLeft(), b.GetLeft());
				INT top = min(a.GetTop(), b.GetTop());

				c.X = left;
				c.Y = top;
				c.Width = right - left;
				c.Height = bottom - top;
				return !c.IsEmptyArea();
			}

			void Offset(IN const Point& point)
			{
				Offset(point.X, point.Y);
			}



			void Offset(IN INT dx,
				IN INT dy)
			{
				X += dx;
				Y += dy;
			}


		};
		class Color
		{
		protected:
			ARGB Argb;
		public:

			Color()
			{
				Argb = (ARGB)Color::Black;
			}

			// Construct an opaque Color object with
			// the specified Red, Green, Blue values.
			//
			// Color values are not premultiplied.

			Color(IN BYTE r,
				IN BYTE g,
				IN BYTE b)
			{
				Argb = MakeARGB(255, r, g, b);
			}

			Color(IN BYTE a,
				IN BYTE r,
				IN BYTE g,
				IN BYTE b)
			{
				Argb = MakeARGB(a, r, g, b);
			}

			Color(IN ARGB argb)
			{
				Argb = argb;
			}

			BYTE GetAlpha() const
			{
				return (BYTE)(Argb >> AlphaShift);
			}

			BYTE GetA() const
			{
				return GetAlpha();
			}

			BYTE GetRed() const
			{
				return (BYTE)(Argb >> RedShift);
			}

			BYTE GetR() const
			{
				return GetRed();
			}

			BYTE GetGreen() const
			{
				return (BYTE)(Argb >> GreenShift);
			}

			BYTE GetG() const
			{
				return GetGreen();
			}

			BYTE GetBlue() const
			{
				return (BYTE)(Argb >> BlueShift);
			}

			BYTE GetB() const
			{
				return GetBlue();
			}

			ARGB GetValue() const
			{
				return Argb;
			}

			void SetValue(IN ARGB argb)
			{
				Argb = argb;
			}

			void SetFromCOLORREF(IN COLORREF rgb)
			{
				Argb = MakeARGB(255, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
			}

			COLORREF ToCOLORREF() const
			{
				return RGB(GetRed(), GetGreen(), GetBlue());
			}

		public:

			// Common color constants

			enum
			{
				AliceBlue = 0xFFF0F8FF,
				AntiqueWhite = 0xFFFAEBD7,
				Aqua = 0xFF00FFFF,
				Aquamarine = 0xFF7FFFD4,
				Azure = 0xFFF0FFFF,
				Beige = 0xFFF5F5DC,
				Bisque = 0xFFFFE4C4,
				Black = 0xFF000000,
				BlanchedAlmond = 0xFFFFEBCD,
				Blue = 0xFF0000FF,
				BlueViolet = 0xFF8A2BE2,
				Brown = 0xFFA52A2A,
				BurlyWood = 0xFFDEB887,
				CadetBlue = 0xFF5F9EA0,
				Chartreuse = 0xFF7FFF00,
				Chocolate = 0xFFD2691E,
				Coral = 0xFFFF7F50,
				CornflowerBlue = 0xFF6495ED,
				Cornsilk = 0xFFFFF8DC,
				Crimson = 0xFFDC143C,
				Cyan = 0xFF00FFFF,
				DarkBlue = 0xFF00008B,
				DarkCyan = 0xFF008B8B,
				DarkGoldenrod = 0xFFB8860B,
				DarkGray = 0xFFA9A9A9,
				DarkGreen = 0xFF006400,
				DarkKhaki = 0xFFBDB76B,
				DarkMagenta = 0xFF8B008B,
				DarkOliveGreen = 0xFF556B2F,
				DarkOrange = 0xFFFF8C00,
				DarkOrchid = 0xFF9932CC,
				DarkRed = 0xFF8B0000,
				DarkSalmon = 0xFFE9967A,
				DarkSeaGreen = 0xFF8FBC8B,
				DarkSlateBlue = 0xFF483D8B,
				DarkSlateGray = 0xFF2F4F4F,
				DarkTurquoise = 0xFF00CED1,
				DarkViolet = 0xFF9400D3,
				DeepPink = 0xFFFF1493,
				DeepSkyBlue = 0xFF00BFFF,
				DimGray = 0xFF696969,
				DodgerBlue = 0xFF1E90FF,
				Firebrick = 0xFFB22222,
				FloralWhite = 0xFFFFFAF0,
				ForestGreen = 0xFF228B22,
				Fuchsia = 0xFFFF00FF,
				Gainsboro = 0xFFDCDCDC,
				GhostWhite = 0xFFF8F8FF,
				Gold = 0xFFFFD700,
				Goldenrod = 0xFFDAA520,
				Gray = 0xFF808080,
				Green = 0xFF008000,
				GreenYellow = 0xFFADFF2F,
				Honeydew = 0xFFF0FFF0,
				HotPink = 0xFFFF69B4,
				IndianRed = 0xFFCD5C5C,
				Indigo = 0xFF4B0082,
				Ivory = 0xFFFFFFF0,
				Khaki = 0xFFF0E68C,
				Lavender = 0xFFE6E6FA,
				LavenderBlush = 0xFFFFF0F5,
				LawnGreen = 0xFF7CFC00,
				LemonChiffon = 0xFFFFFACD,
				LightBlue = 0xFFADD8E6,
				LightCoral = 0xFFF08080,
				LightCyan = 0xFFE0FFFF,
				LightGoldenrodYellow = 0xFFFAFAD2,
				LightGray = 0xFFD3D3D3,
				LightGreen = 0xFF90EE90,
				LightPink = 0xFFFFB6C1,
				LightSalmon = 0xFFFFA07A,
				LightSeaGreen = 0xFF20B2AA,
				LightSkyBlue = 0xFF87CEFA,
				LightSlateGray = 0xFF778899,
				LightSteelBlue = 0xFFB0C4DE,
				LightYellow = 0xFFFFFFE0,
				Lime = 0xFF00FF00,
				LimeGreen = 0xFF32CD32,
				Linen = 0xFFFAF0E6,
				Magenta = 0xFFFF00FF,
				Maroon = 0xFF800000,
				MediumAquamarine = 0xFF66CDAA,
				MediumBlue = 0xFF0000CD,
				MediumOrchid = 0xFFBA55D3,
				MediumPurple = 0xFF9370DB,
				MediumSeaGreen = 0xFF3CB371,
				MediumSlateBlue = 0xFF7B68EE,
				MediumSpringGreen = 0xFF00FA9A,
				MediumTurquoise = 0xFF48D1CC,
				MediumVioletRed = 0xFFC71585,
				MidnightBlue = 0xFF191970,
				MintCream = 0xFFF5FFFA,
				MistyRose = 0xFFFFE4E1,
				Moccasin = 0xFFFFE4B5,
				NavajoWhite = 0xFFFFDEAD,
				Navy = 0xFF000080,
				OldLace = 0xFFFDF5E6,
				Olive = 0xFF808000,
				OliveDrab = 0xFF6B8E23,
				Orange = 0xFFFFA500,
				OrangeRed = 0xFFFF4500,
				Orchid = 0xFFDA70D6,
				PaleGoldenrod = 0xFFEEE8AA,
				PaleGreen = 0xFF98FB98,
				PaleTurquoise = 0xFFAFEEEE,
				PaleVioletRed = 0xFFDB7093,
				PapayaWhip = 0xFFFFEFD5,
				PeachPuff = 0xFFFFDAB9,
				Peru = 0xFFCD853F,
				Pink = 0xFFFFC0CB,
				Plum = 0xFFDDA0DD,
				PowderBlue = 0xFFB0E0E6,
				Purple = 0xFF800080,
				Red = 0xFFFF0000,
				RosyBrown = 0xFFBC8F8F,
				RoyalBlue = 0xFF4169E1,
				SaddleBrown = 0xFF8B4513,
				Salmon = 0xFFFA8072,
				SandyBrown = 0xFFF4A460,
				SeaGreen = 0xFF2E8B57,
				SeaShell = 0xFFFFF5EE,
				Sienna = 0xFFA0522D,
				Silver = 0xFFC0C0C0,
				SkyBlue = 0xFF87CEEB,
				SlateBlue = 0xFF6A5ACD,
				SlateGray = 0xFF708090,
				Snow = 0xFFFFFAFA,
				SpringGreen = 0xFF00FF7F,
				SteelBlue = 0xFF4682B4,
				Tan = 0xFFD2B48C,
				Teal = 0xFF008080,
				Thistle = 0xFFD8BFD8,
				Tomato = 0xFFFF6347,
				Transparent = 0x00FFFFFF,
				Turquoise = 0xFF40E0D0,
				Violet = 0xFFEE82EE,
				Wheat = 0xFFF5DEB3,
				White = 0xFFFFFFFF,
				WhiteSmoke = 0xFFF5F5F5,
				Yellow = 0xFFFFFF00,
				YellowGreen = 0xFF9ACD32
			};

			// Shift count and bit mask for A, R, G, B components

			enum
			{
				AlphaShift = 24,
				RedShift = 16,
				GreenShift = 8,
				BlueShift = 0
			};

			enum
			{
				AlphaMask = 0xff000000,
				RedMask = 0x00ff0000,
				GreenMask = 0x0000ff00,
				BlueMask = 0x000000ff
			};

			// Assemble A, R, G, B values into a 32-bit integer

			static ARGB MakeARGB(IN BYTE a,
				IN BYTE r,
				IN BYTE g,
				IN BYTE b)
			{
				return (((ARGB)(b) << BlueShift) |
					((ARGB)(g) << GreenShift) |
					((ARGB)(r) << RedShift) |
					((ARGB)(a) << AlphaShift));
			}


		};
	};

	typedef struct Distance {
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
		//获取垂直所占空间
		size_t GetVSpace() {
			return Top + Bottom;
		}
		//获取水平所占空间
		size_t GetHSpace() {
			return Left + Right;
		}
	} Margin, Padding;

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
	enum class Align :int
	{
		Top = Align_Top,
		Bottom = Align_Bottom,
		Left = Align_Left,
		Right = Align_Right,
		Mid = Align_Mid,
		Center = Align_Center
	};
	enum  class TextAlign :int {
		// 摘要: 
		   //     内容在垂直方向上顶部对齐，在水平方向上左边对齐。
		TopLeft = Align_Top | Align_Left,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上居中对齐。
		TopCenter = Align_Top | Align_Center,
		//
		// 摘要: 
		//     内容在垂直方向上顶部对齐，在水平方向上右边对齐。
		TopRight = Align_Top | Align_Right,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上左边对齐。
		MiddleLeft = Align_Mid | Align_Left,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上居中对齐。
		MiddleCenter = Align_Mid | Align_Center,
		//
		// 摘要: 
		//     内容在垂直方向上中间对齐，在水平方向上右边对齐。
		MiddleRight = Align_Mid | Align_Right,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上左边对齐。
		BottomLeft = Align_Bottom | Align_Left,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上居中对齐。
		BottomCenter = Align_Bottom | Align_Center,
		//
		// 摘要: 
		//     内容在垂直方向上底边对齐，在水平方向上右边对齐。
		BottomRight = Align_Bottom | Align_Right
	};

	enum class FontStyle :int {
		NORMAL = 0
		/*DWRITE_FONT_STYLE_NORMAL
		字体样式 ：正常。
		DWRITE_FONT_STYLE_OBLIQUE
		字体样式 ：倾斜。
		DWRITE_FONT_STYLE_ITALIC
		字体样式 ：斜体。*/
	};

	class  IImage {
	protected:
		size_t _frameCount = 0;//总帧数
		size_t _framePos = 0;//当前帧率索引
	public:
		EzUI::Padding Padding;// 控件与图片的距离 该数值越大 图片将越小 参考web前端
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
			_framePos++;
			return 0;
		}
	};

	inline RenderType::Rect Transformation(ImageSizeMode imageSizeMode, const RenderType::Rect& rect, const RenderType::Size& imgSize) {

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
				RenderType::SizeF sz{ clientWidth,zoomHeight };
				float y = (clientHeight - sz.Height) / 2 + rect.Y;
				return RenderType::Rect{ rect.X  ,(INT)y, (INT)sz.Width, (INT)sz.Height };
			}
			else {
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;
				RenderType::SizeF sz{ zoomWidth,clientHeight };
				float x = (clientWidth - sz.Width) / 2 + rect.X;
				return RenderType::Rect{ (INT)x  , rect.Y, (INT)sz.Width, (INT)sz.Height };
			}
		}
		if (imageSizeMode == ImageSizeMode::CenterImage) {
			if (clientRate < imgRate) {
				//1000 670 客户端
				//1000 300 图片
				//2233 670     缩放后的图片大小 
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;//图片应该这么宽才对
				float x = (zoomWidth - clientWidth) / 2 + 0.5f;
				return RenderType::Rect{ (INT)(rect.X - x),rect.Y,(INT)zoomWidth,(INT)clientHeight };
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				float zoomHeight = clientWidth / imgWidth * imgHeight + 0.5f;//图片应该这么高才对
				float y = (zoomHeight - clientHeight) / 2 + 0.5f;
				return RenderType::Rect{ rect.X, (INT)(rect.Y - y)  , (INT)clientWidth, (INT)zoomHeight };
			}
		}
		return rect;
	}
#define __Rect RenderType::Rect
#define __RectF RenderType::RectF
#define __Color RenderType::Color
#define __Point RenderType::Point
#define __PointF RenderType::PointF
#define __ARGB RenderType::ARGB
#define __Size RenderType::Size
#define __SizeF RenderType::SizeF
};