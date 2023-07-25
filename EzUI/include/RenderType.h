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

		template<typename T>
		class __Size
		{
		public:
			T Width;
			T Height;
		public:
			__Size()
			{
				Width = Height = 0;
			}
			__Size(const __Size& __Size)
			{
				Width = __Size.Width;
				Height = __Size.Height;
			}
			__Size(T width,
				T height)
			{
				Width = width;
				Height = height;
			}
			__Size operator+(const __Size& sz) const
			{
				return __Size(Width + sz.Width,
					Height + sz.Height);
			}
			__Size operator-(const __Size& sz) const
			{
				return __Size(Width - sz.Width,
					Height - sz.Height);
			}

			bool operator==(const __Size& _right) const
			{
				return (Width == _right.Width && Height == _right.Height);
			}
			bool operator!=(const __Size& _right) const
			{
				return !(Width == _right.Width && Height == _right.Height);
			}

			bool Equals(const __Size& sz) const
			{
				return (Width == sz.Width) && (Height == sz.Height);
			}

			bool Empty() const
			{
				return (Width == 0 && Height == 0);
			}
		};

		template<typename T>
		class __Point
		{
		public:
			T X;
			T Y;
		public:
			__Point()
			{
				X = Y = 0;
			}

			__Point(const __Point& __Point)
			{
				X = __Point.X;
				Y = __Point.Y;
			}

			__Point(const __Size<T>& __Size)
			{
				X = __Size.Width;
				Y = __Size.Height;
			}

			__Point(T x,
				T y)
			{
				X = x;
				Y = y;
			}

			__Point operator+(const __Point& __Point) const
			{
				return __Point(X + __Point.X,
					Y + __Point.Y);
			}

			__Point operator-(const __Point& __Point) const
			{
				return __Point(X - __Point.X,
					Y - __Point.Y);
			}

			bool Equals(const __Point& __Point)
			{
				return (X == __Point.X) && (Y == __Point.Y);
			}

		};

		template<typename T>
		class __Rect
		{
		public:
			T X;
			T Y;
			T Width;
			T Height;
		public:

			__Rect()
			{
				X = Y = Width = Height = 0;
			}

			__Rect(T x,
				T y,
				T width,
				T height)
			{
				X = x;
				Y = y;
				Width = width;
				Height = height;
			}

			__Rect(const __Point<T>& location, const __Size<T>& size) {
				X = location.X;
				Y = location.Y;
				Width = size.Width;
				Height = size.Height;
			}

			__Point<T> GetLocation() const
			{
				return __Point<T>{ X, Y };
			}

			__Size<T> GetSize() const
			{
				return __Size<T>(Width, Height);
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

			bool Equals(const __Rect& __Rect) const
			{
				return X == __Rect.X &&
					Y == __Rect.Y &&
					Width == __Rect.Width &&
					Height == __Rect.Height;
			}

			bool  operator == (const __Rect& right) {
				return Equals(right);
			}

			bool Contains(T x,
				T y) const
			{
				return x >= X && x < X + Width &&
					y >= Y && y < Y + Height;
			}

			bool Contains(const __Point<T>& pt) const
			{
				return Contains(pt.X, pt.Y);
			}

			bool Contains(const __Rect& __Rect) const
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

			void Inflate(const __Point<T>& point)
			{
				Inflate(point.X, point.Y);
			}

			bool Intersect(const __Rect& __Rect)
			{
				return Intersect(*this, *this, __Rect);
			}
			__Rect WinRECT() const {
				return __Rect{ X,Y,GetRight(),GetBottom() };
			}

			static bool Intersect(__Rect& c,
				const __Rect& a,
				const __Rect& b)
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

			bool IntersectsWith(const __Rect& __Rect) const
			{
				return (GetLeft() < __Rect.GetRight() &&
					GetTop() < __Rect.GetBottom() &&
					GetRight() > __Rect.GetLeft() &&
					GetBottom() > __Rect.GetTop());
			}

			static bool Union(__Rect& c,
				const __Rect& a,
				const __Rect& b)
			{
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
			void Offset(const __Point<T>& point)
			{
				Offset(point.X, point.Y);
			}
			void Offset(T dx,
				T dy)
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
				Argb = 0;
			}

			// Construct an opaque Color object with
			// the specified Red, Green, Blue values.
			//
			// Color values are not premultiplied.

			Color(BYTE r,
				BYTE g,
				BYTE b)
			{
				Argb = MakeARGB(255, r, g, b);
			}

			Color(BYTE a,
				BYTE r,
				BYTE g,
				BYTE b)
			{
				Argb = MakeARGB(a, r, g, b);
			}

			Color(ARGB argb)
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

			void SetValue(ARGB argb)
			{
				Argb = argb;
			}
		public:

			// Common color constants

			enum
			{
				Red = 0xFFFF0000,
				Green = 0xFF008000,
				Blue = 0xFF0000FF,
				Black = 0xFF000000,
				Gray = 0xFF808080,
				Orange = 0xFFFFA500,
				Pink = 0xFFFFC0CB,
				White = 0xFFFFFFFF,
				Yellow = 0xFFFFFF00
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

			static ARGB MakeARGB(BYTE a,
				BYTE r,
				BYTE g,
				BYTE b)
			{
				return (((ARGB)(b) << BlueShift) |
					((ARGB)(g) << GreenShift) |
					((ARGB)(r) << RedShift) |
					((ARGB)(a) << AlphaShift));
			}


		};

		template<typename T>
		class __Line {
		public:
			__Point<T> pointA;
			__Point<T> pointB;
		public:
			__Line() {
				pointA.X = 0;
				pointA.Y = 0;
				pointB.X = 0;
				pointB.Y = 0;
			}
			__Line(const __Point<T>& _pointA, const __Point<T>& _pointB) {
				this->pointA = _pointA;
				this->pointB = _pointB;
			}
		};
	};

	typedef  RenderType::__Point<INT> Point;
	typedef  RenderType::__Point<FLOAT> PointF;
	typedef  RenderType::__Line<INT> Line;
	typedef  RenderType::__Line<FLOAT> LineF;
	typedef  RenderType::__Size<INT> Size;
	typedef  RenderType::__Size<FLOAT> SizeF;
	typedef  RenderType::__Rect<INT> Rect;
	typedef  RenderType::__Rect<FLOAT> RectF;
	typedef  RenderType::Color Color;

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
		//��ȡ��ֱ��ռ�ռ�
		size_t GetVSpace() {
			return Top + Bottom;
		}
		//��ȡˮƽ��ռ�ռ�
		size_t GetHSpace() {
			return Left + Right;
		}
	} Margin, Padding;

	enum class ImageSizeMode {
		//
		// ժҪ:
		//     Owner�ؼ� �е�ͼ����������������ʺ� Owner�ؼ�
		//     �Ĵ�С��
		StretchImage = 1,
		// ժҪ:
		//     ��� Owner�ؼ� ��ͼ�����ͼ�񽫾�����ʾ�����ͼ��� Owner�ؼ�
		//     ����ͼƬ������ Owner�ؼ� ���ģ������Ե�������õ���
		CenterImage = 3,
		//
		// ժҪ:
		//     ͼ���С����ԭ�еĴ�С���������ӻ��С��
		Zoom = 4
	};
	/// <summary>
	/// �����߿��һЩ��Ϣ
	/// </summary>
	class  Border {
	public:
		int Left = 0;//��߱߿��С
		int Top = 0;//�����߿��С
		int Right = 0;//�ұ߱߿��С
		int Bottom = 0;//�ײ��߿��С
		int TopLeftRadius = 0;
		int TopRightRadius = 0;
		int BottomRightRadius = 0;
		int BottomLeftRadius = 0;
		Color Color;
	public:
		class Radius {
			Border& Border;
		public:
			Radius(EzUI::Border& bd) :Border(bd) {}
			//���ĸ��Ƕ�ͬʱ���ð뾶��С
			Radius& operator=(int radius) {
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
		//���ĸ������ô�С
		Border& operator=(int borderWidth) {
			Left = borderWidth;
			Top = borderWidth;
			Right = borderWidth;
			Bottom = borderWidth;
			return *this;
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
	enum class Align :int
	{
		Top = Align_Top,
		Bottom = Align_Bottom,
		Left = Align_Left,
		Right = Align_Right,
		Mid = Align_Mid,
		Center = Align_Center
	};
	enum class __Align :int {
		//
		// ժҪ: 
		//     ��������
		Normal = 0,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����϶������룬��ˮƽ��������߶��롣
		TopLeft = Align_Top | Align_Left,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����϶������룬��ˮƽ�����Ͼ��ж��롣
		TopCenter = Align_Top | Align_Center,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����϶������룬��ˮƽ�������ұ߶��롣
		TopRight = Align_Top | Align_Right,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�������м���룬��ˮƽ��������߶��롣
		MiddleLeft = Align_Mid | Align_Left,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�������м���룬��ˮƽ�����Ͼ��ж��롣
		MiddleCenter = Align_Mid | Align_Center,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�������м���룬��ˮƽ�������ұ߶��롣
		MiddleRight = Align_Mid | Align_Right,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����ϵױ߶��룬��ˮƽ��������߶��롣
		BottomLeft = Align_Bottom | Align_Left,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����ϵױ߶��룬��ˮƽ�����Ͼ��ж��롣
		BottomCenter = Align_Bottom | Align_Center,
		//
		// ժҪ: 
		//     �����ڴ�ֱ�����ϵױ߶��룬��ˮƽ�������ұ߶��롣
		BottomRight = Align_Bottom | Align_Right
	};

	typedef __Align TextAlign;
	typedef __Align ControlAlign;


	enum class FontStyle :int {
		NORMAL = 0
		/*DWRITE_FONT_STYLE_NORMAL
		������ʽ ��������
		DWRITE_FONT_STYLE_OBLIQUE
		������ʽ ����б��
		DWRITE_FONT_STYLE_ITALIC
		������ʽ ��б�塣*/
	};

	class  IImage {
	protected:
		size_t _frameCount = 0;//��֡��
		size_t _framePos = 0;//��ǰ֡������
	public:
		EzUI::Padding Padding;// �ؼ���ͼƬ�ľ��� ����ֵԽ�� ͼƬ��ԽС �ο�webǰ��
		ImageSizeMode SizeMode = ImageSizeMode::Zoom;// ͼ����ʾģʽ
	public:
		virtual ~IImage() {}
		size_t FrameCount() {
			return _frameCount;
		}
		//��ת����һ֡ ���һ�ȡ��һ֡���ӳ�
		virtual size_t NextFrame() {
			if (_framePos >= _frameCount) {
				_framePos = 0;
			}
			_framePos++;
			return 0;
		}
	};

	inline Rect Transformation(ImageSizeMode imageSizeMode, const Rect& rect, const Size& imgSize) {

		if (imageSizeMode == ImageSizeMode::StretchImage) {
			return rect;
		}

		//�ͻ�������
		float clientWidth = (float)rect.Width;
		float clientHeight = (float)rect.Height;
		float clientRate = clientWidth / clientHeight;
		//ͼƬ����
		float imgWidth = (float)imgSize.Width;
		float imgHeight = (float)imgSize.Height;
		float imgRate = imgWidth / imgHeight;

		if (imageSizeMode == ImageSizeMode::Zoom) {
			if (clientRate < imgRate) {
				float zoomHeight = clientWidth / imgWidth * imgHeight + 0.5f;
				float y = (clientHeight - zoomHeight) / 2 + rect.Y;
				return Rect(rect.X, (INT)y, (INT)clientWidth, (INT)zoomHeight);
			}
			else {
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;
				float x = (clientWidth - zoomWidth) / 2 + rect.X;
				return Rect((INT)x, rect.Y, (INT)zoomWidth, (INT)clientHeight);
			}
		}
		if (imageSizeMode == ImageSizeMode::CenterImage) {
			if (clientRate < imgRate) {
				//1000 670 �ͻ���
				//1000 300 ͼƬ
				//2233 670     ���ź��ͼƬ��С 
				float zoomWidth = clientHeight / imgHeight * imgWidth + 0.5f;//ͼƬӦ����ô��Ŷ�
				float x = (zoomWidth - clientWidth) / 2 + 0.5f;
				return Rect((INT)(rect.X - x), rect.Y, (INT)zoomWidth, (INT)clientHeight);
			}
			else {
				//1000 600 �ͻ���
				//400  600 ͼƬ
				//1000 1500     ���ź��ͼƬ��С 
				float zoomHeight = clientWidth / imgWidth * imgHeight + 0.5f;//ͼƬӦ����ô�߲Ŷ�
				float y = (zoomHeight - clientHeight) / 2 + 0.5f;
				return Rect(rect.X, (INT)(rect.Y - y), (INT)clientWidth, (INT)zoomHeight);
			}
		}
		return rect;
	}
};