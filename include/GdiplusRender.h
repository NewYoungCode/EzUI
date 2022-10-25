#pragma once
#include "UIDef.h"
#if USED_GDIPLUS
#include <string>
#include <list>
#include <map>
#include <Windows.h>
#include <Windowsx.h>
#include <comdef.h>
#include <GdiPlus.h>
#include <GdiplusTypes.h>
#include <gdiplusimagecodec.h>
#include <gdiplusimaging.h>
#include <gdiplusimageattributes.h>
#include <shlwapi.h>
#include <usp10.h>
#include "RenderType.h"
namespace EzUI {

	extern int Dpi;

	class TextFormat {
	public:
		std::wstring fontFamilly; int fontSize; TextAlign textAlign;
		HFONT Font = NULL;
	public:
		TextFormat(const std::wstring& fontFamilly, int fontSize, TextAlign textAlign)
		{
			this->fontFamilly = fontFamilly;
			this->fontSize = fontSize;
			this->textAlign = textAlign;

			LOGFONTW lf{ 0 };
			GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
			wcsncpy_s(lf.lfFaceName, fontFamilly.c_str(), LF_FACESIZE);
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfHeight = -MulDiv(fontSize, Dpi, 72);
			//lf.lfWeight += FW_BOLD; //����
			//lf.lfUnderline = lfUnderline; //�»���
			//lf.lfItalic = TRUE; //б��
			Font = CreateFontIndirectW(&lf);
		}
		~TextFormat() {
			if (Font) {
				::DeleteObject(Font);
			}
		}

	};
	class TextLayout {
	public:
		SCRIPT_STRING_ANALYSIS m_Analysis = NULL;	//�ַ����������;
		std::wstring* text;
		__Size maxSize;
		TextFormat* textFormat;
		SCRIPT_STATE ScriptState{ 0 };
		SCRIPT_CONTROL ScriptControl{ 0 };
	public:
		TextLayout(const std::wstring& _text, __Size _maxSize, TextFormat* _textFormat) {
			text = (std::wstring*)&_text;
			maxSize = _maxSize;
			textFormat = _textFormat;
			::ScriptApplyDigitSubstitution(NULL, &ScriptControl, &ScriptState);

			HDC m_hDc = ::CreateCompatibleDC(NULL);
			SelectFont(m_hDc, _textFormat->Font);

			//�ַ������� ����Ϊ1
			int cString = text->length() == 0 ? 1 : text->length();

			HRESULT hr = ::ScriptStringAnalyse(m_hDc,
				text->c_str(),
				cString,
				(INT)(1.5 * cString + 16),
				-1,
				SSA_GLYPHS | SSA_BREAK | SSA_FALLBACK | SSA_LINK,
				0,
				&ScriptControl,
				&ScriptState,
				NULL,
				NULL,
				NULL,
				&m_Analysis);

			::DeleteObject(m_hDc);
		}
		__Point HitTestTextPosition(int textPos, BOOL A_isTrailingHit) {

			__Size box = GetFontSize();
			FLOAT y = (maxSize.Height - box.Height) / 2.0f;//��֤�����Ǿ��е�

			int x = 0;
			HRESULT ret = ::ScriptStringCPtoX(m_Analysis, textPos, A_isTrailingHit, &x);
			if (ret != S_OK || text->empty()) {
				x = box.Width;
			}


			return { x,(INT)y };
		}
		__Point HitTestPoint(__Point point_Start, int& A_TextPos, BOOL& A_isTrailingHit) {

			__Size box = GetFontSize();
			FLOAT y = (maxSize.Height - box.Height) / 2.0f;//��֤�����Ǿ��е�
			if (point_Start.X >= box.Width) {//��������˱߽�
				A_TextPos = text->length() - 1;
				A_isTrailingHit = true;
				return { box.Width,(INT)y };
			}
			::ScriptStringXtoCP(m_Analysis, point_Start.X, &A_TextPos, &A_isTrailingHit);
			int outX = 0;
			::ScriptStringCPtoX(m_Analysis, A_TextPos, A_isTrailingHit, &outX);

			return { outX,(INT)y };
		}
		__Size GetFontSize() {
			const SIZE* sz = ::ScriptString_pSize(m_Analysis);
			int x = sz->cx;
			int y = sz->cy;

			if (text->empty()) {
				x = 0;
			}

			return __Size{ x,y };
		}
		virtual ~TextLayout() {
			if (m_Analysis) {
				::ScriptStringFree(&m_Analysis);
			}
		}
	};

	class UI_EXPORT GdiplusImage :public Gdiplus::Bitmap, public IImage {
	protected:
		Gdiplus::PropertyItem* m_pPropertyItem = NULL;
		GUID* pDimensionIDs = NULL;
		void CreateFromFile(const std::wstring& file) {
			Gdiplus::GpBitmap* bitmap = NULL;
			lastResult = Gdiplus::DllExports::GdipCreateBitmapFromFile(file.c_str(), &bitmap);
			SetNativeImage(bitmap);
			Init();
		}
		void CreateFormStream(IStream* iStream) {
			Gdiplus::GpBitmap* bitmap = NULL;
			lastResult = Gdiplus::DllExports::GdipCreateBitmapFromStream(iStream, &bitmap);
			SetNativeImage(bitmap);
			Init();
		}
		void Init();
	public:
		virtual ~GdiplusImage();
		GdiplusImage(IStream* iStream) :Gdiplus::Bitmap(iStream) { Init(); }
		GdiplusImage(const std::wstring& fileName) :Gdiplus::Bitmap(fileName.c_str()) { Init(); }
		GdiplusImage(HBITMAP hBitmap) :Gdiplus::Bitmap(hBitmap, NULL) { Init(); }
		GdiplusImage(BITMAPINFO* gdiBitmapInfo, void* gdiBitmapData) :Gdiplus::Bitmap(gdiBitmapInfo, gdiBitmapData) { Init(); }
		GdiplusImage(INT width, INT height, Gdiplus::PixelFormat pixelFormat = PixelFormat32bppARGB) :Gdiplus::Bitmap(width, height, pixelFormat) { Init(); }
		//��ת����һ֡ ���һ�ȡ��һ֡���ӳ�
		virtual size_t NextFrame();
		void Save(const  std::wstring& fileName);
	};

	class Geometry {
	public:
		bool Ref = false;
		Gdiplus::Region* rgn = NULL;
		Gdiplus::GraphicsPath Path;
	protected:
		void Copy(const Geometry& _copy) {
			((Geometry&)(_copy)).Ref = true;
		}
	public:
		Geometry() {}
		Geometry(int x, int y, int w, int h) {
			rgn = new Gdiplus::Region(Gdiplus::Rect{ x,y,w,h });
		}
		Geometry(int x, int y, int w, int h, int radius) {
			Gdiplus::Rect Rect{ x,y,w,h };
			if (radius <= 0) {
				Path.AddRectangle(Rect);
				Path.CloseFigure();
				return;
			}
			int diameter = radius;
			Gdiplus::Rect arcRect(Rect.X, Rect.Y, diameter, diameter);// = new Rect(rect.Location, new Size(diameter, diameter));
			//   ���Ͻ�      
			Path.AddArc(arcRect, 180, 90);
			//   ���Ͻ�      
			arcRect.X = Rect.GetRight() - diameter;
			Path.AddArc(arcRect, 270, 90);
			//   ���½�      
			arcRect.Y = Rect.GetBottom() - diameter;
			Path.AddArc(arcRect, 0, 90);
			//   ���½�      
			arcRect.X = Rect.GetLeft();
			Path.AddArc(arcRect, 90, 90);
			Path.CloseFigure();
			rgn = new Gdiplus::Region(&Path);
		}

		Geometry(const Geometry& _copy) {
			Copy(_copy);
		}
		Geometry& operator =(const Geometry& _right) {
			Copy(_right);
			return *this;
		}
		virtual ~Geometry() {
			if (rgn && !Ref) {
				delete rgn;
			}
		}
	public:
		//��������ȡ���߽�
		static void Union(Geometry& out, const Geometry& a, const Geometry& b) {
			if (out.rgn) {
				delete out.rgn;
			}
			out.rgn = a.rgn->Clone();
			out.rgn->Union(b.rgn);
		}
		//���������н����Ĳ���
		static void Intersect(Geometry& out, const Geometry& a, const Geometry& b) {
			if (out.rgn) {
				delete out.rgn;
			}
			out.rgn = a.rgn->Clone();
			out.rgn->Intersect(b.rgn);
		}
		static void Xor(Geometry& out, const Geometry& a, const Geometry& b) {
			if (out.rgn) {
				delete out.rgn;
			}
			out.rgn = a.rgn->Clone();
			out.rgn->Xor(b.rgn);
		}
		static void Exclude(Geometry& out, const Geometry& a, const Geometry& b) {
			if (out.rgn) {
				delete out.rgn;
			}
			out.rgn = a.rgn->Clone();
			out.rgn->Exclude(b.rgn);
		}
	};

	UI_EXPORT void RenderInitialize();
	UI_EXPORT void RenderUnInitialize();
	UI_EXPORT void HighQualityMode(Gdiplus::Graphics* graphics);
	UI_EXPORT void CreateRectangle(Gdiplus::GraphicsPath& path, const __Rect& rect, int radius);//����
	UI_EXPORT int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	UI_EXPORT BOOL SaveHDCToFile(HDC hDC, LPRECT lpRect, const WCHAR* format, const WCHAR* filename);
	UI_EXPORT BOOL SaveHDCToFile(HDC hDC, const __Rect& rect, const std::wstring& fileName);
	UI_EXPORT GdiplusImage* ClipImage(GdiplusImage* img, const __Size& sz, int _radius);

	class Layer {
	public:
		__Rect Bound;
		HBITMAP Bitmap = NULL;
		HDC DC = NULL;//��ʱDC
		HDC SRCDC = NULL;//ԴDC
		int Radius = 0;
		Gdiplus::Graphics* render = NULL;
	public:
		Layer(const __Rect& rect, int radius, HDC srcDC, Gdiplus::Graphics* render) {
			this->render = render;
			this->Bound = rect;
			this->Radius = radius;
			this->SRCDC = srcDC;
			this->Bitmap = ::CreateCompatibleBitmap(SRCDC, rect.Width, rect.Height);
			this->DC = ::CreateCompatibleDC(NULL);
			::SelectObject(DC, Bitmap);
			::BitBlt(this->DC, 0, 0, Bound.Width, Bound.Height, SRCDC, Bound.X, Bound.Y, SRCCOPY);//���ݱ���
		}
		void PopLayer() {
			//�ѻ��ƺõı������� ׼�����ü�
			HBITMAP bitmap = ::CreateCompatibleBitmap(this->DC, Bound.Width, Bound.Height);
			HDC  hdc = ::CreateCompatibleDC(NULL);
			::SelectObject(hdc, bitmap);
			::BitBlt(hdc, 0, 0, Bound.Width, Bound.Height, SRCDC, Bound.X, Bound.Y, SRCCOPY);
			//�ѱ�������ȥ
			::BitBlt(SRCDC, Bound.X, Bound.Y, Bound.Width, Bound.Height, this->DC, 0, 0, SRCCOPY);

			//����ü��ٻ�����ȥ
			Gdiplus::Bitmap img(bitmap, NULL);
			Gdiplus::TextureBrush tb(&img);
			Gdiplus::GraphicsPath path;
			CreateRectangle(path, Bound, Radius);//����
			tb.TranslateTransform((FLOAT)Bound.X, (FLOAT)Bound.Y);//����ƫ��
			render->FillPath(&tb, &path);

			//������
			::DeleteDC(hdc);
			::DeleteObject(bitmap);
		}
		virtual ~Layer() {
			if (Bitmap || DC) {
				::DeleteDC(DC);
				::DeleteObject(Bitmap);
			}
		}
	};
	class UI_EXPORT GdiplusRender
	{
	protected:
		Gdiplus::SolidBrush* SolidBrush = NULL;
	public:
		int Count = 0;
		std::list<__Rect*> Layers;
		HDC DC = NULL;
		int OffsetX = 0;
		int OffsetY = 0;
		Gdiplus::Graphics* base = NULL;
	public:
		GdiplusRender(HDC hdc, int Width = 0, int Height = 0);
		GdiplusRender(HWND hWnd);
		GdiplusRender(Gdiplus::Image* image);
		virtual ~GdiplusRender();
		void DrawRectangle(const __Rect& rect, const __Color& color, int width = 1, int radius = 0);
		void FillRectangle(const __Rect& rect, const __Color& color, int radius = 0);
		void DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const __Color& color, const __Rect& rect, TextAlign textAlign, bool underLine = false, HFONT font = NULL);
		void DrawTextLayout(const __Point& pt, TextLayout* textLayout, const __Color& color);
		Gdiplus::SolidBrush* GetSolidBrush(const __Color& color);
		void PushAxisAlignedClip(const __Rect& rect);
		void PopAxisAlignedClip();
		void Flush();
		HFONT CreateHFont(const std::wstring& fontFamily, int fontSize, HDC DC, bool lfUnderline = false);
		void DrawLine(const __Color& color, const __Point& A, const __Point& B, int width = 1);
		void DrawImage(IImage* image, const __Rect& destRect, const __Rect& srcRect);
		void DrawImage(IImage* image, const __Rect& rect, const ImageSizeMode& imageSizeMode = ImageSizeMode::Zoom, const Margin& margin = 0);
		void SaveImage(const WCHAR* format, const WCHAR* fileName, const __Size& size);
		void FillGeometry(const Geometry& geometry, const  __Color& color);
		void BeginDraw();
		void EndDraw();
	};
	using Painter = GdiplusRender;

};
#endif