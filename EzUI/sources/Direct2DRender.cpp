#include "Direct2DRender.h"
#if USED_Direct2D
#define _NOREND_IMAGE_ 
//#define _NOREND_IMAGE_ return;
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Windowscodecs.lib")
namespace EzUI {
	namespace D2D {
		ID2D1Factory* g_Direct2dFactory = NULL;
		IDWriteFactory* g_WriteFactory = NULL;
		IWICImagingFactory* g_ImageFactory = NULL;
	}
#define __FloatRate 0.003921568627451
#define __To_D2D_COLOR_F(color) D2D_COLOR_F{FLOAT(color.GetR() * __FloatRate), FLOAT(color.GetG() * __FloatRate), FLOAT(color.GetB() * __FloatRate),FLOAT(color.GetA() * __FloatRate)}
#define __To_D2D_RectF(rect) D2D_RECT_F{(FLOAT)rect.X,(FLOAT)rect.Y,(FLOAT)rect.GetRight(),(FLOAT)rect.GetBottom() }

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

	//SafeSolidColorBrush
	class SafeSolidColorBrush {
		ID2D1SolidColorBrush* value = NULL;
	public:
		SafeSolidColorBrush(ID2D1RenderTarget* render, const __Color& color) {
			render->CreateSolidColorBrush(__To_D2D_COLOR_F(color), &value);
		}
		~SafeSolidColorBrush() {
			SafeRelease(&value);
		}
		operator ID2D1SolidColorBrush* () {
			return value;
		}
	};

	//TextFormat
	TextFormat::TextFormat(const std::wstring& fontFamily, int fontSize, TextAlign textAlign) {
		D2D::g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (FLOAT)fontSize, L"", &value);
#define __Top DWRITE_PARAGRAPH_ALIGNMENT_NEAR
#define	__Bottom DWRITE_PARAGRAPH_ALIGNMENT_FAR
#define	__Left DWRITE_TEXT_ALIGNMENT_LEADING
#define	__Right DWRITE_TEXT_ALIGNMENT_TRAILING
#define	__Middle DWRITE_PARAGRAPH_ALIGNMENT_CENTER
#define __Center DWRITE_TEXT_ALIGNMENT_CENTER
		do
		{
			if (textAlign == TextAlign::BottomCenter) {
				value->SetParagraphAlignment(__Bottom);
				value->SetTextAlignment(__Center);
				break;
			}
			if (textAlign == TextAlign::BottomLeft) {
				value->SetParagraphAlignment(__Bottom);
				value->SetTextAlignment(__Left);
				break;
			}
			if (textAlign == TextAlign::BottomRight) {
				value->SetParagraphAlignment(__Bottom);
				value->SetTextAlignment(__Right);
				break;
			}
			if (textAlign == TextAlign::MiddleCenter) {
				value->SetParagraphAlignment(__Middle);
				value->SetTextAlignment(__Center);
				break;
			}
			if (textAlign == TextAlign::MiddleLeft) {
				value->SetParagraphAlignment(__Middle);
				value->SetTextAlignment(__Left);
				break;
			}
			if (textAlign == TextAlign::MiddleRight) {
				value->SetParagraphAlignment(__Middle);
				value->SetTextAlignment(__Right);
				break;
			}
			if (textAlign == TextAlign::TopCenter) {
				value->SetParagraphAlignment(__Top);
				value->SetTextAlignment(__Center);
				break;
			}
			if (textAlign == TextAlign::TopLeft) {
				value->SetParagraphAlignment(__Top);
				value->SetTextAlignment(__Left);
				break;
			}
			if (textAlign == TextAlign::TopRight) {
				value->SetParagraphAlignment(__Top);
				value->SetTextAlignment(__Right);
				break;
			}
		} while (0);
#undef __Top 
#undef __Bottom 
#undef __Left 
#undef __Right 
#undef __Middle 
#undef __Center 
	}
	IDWriteTextFormat* TextFormat::operator->() {
		return value;
	}
	TextFormat::operator IDWriteTextFormat* () {
		return value;
	}
	TextFormat::~TextFormat() {
		if (value) {
			value->Release();
		}
	}
	//TextLayout
	TextLayout::TextLayout(const std::wstring& text, __Size maxSize, TextFormat* pTextFormat) {
		D2D::g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), pTextFormat->value, (FLOAT)maxSize.Width, (FLOAT)maxSize.Height, &value);
	}
	__Point TextLayout::HitTestPoint(const __Point& pt, int& textPos, BOOL& isTrailingHit) {
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		//BOOL isTrailingHit;
		BOOL isInside;
		{
			FLOAT x = (FLOAT)pt.X, y = (FLOAT)pt.Y;
			value->HitTestPoint(
				(FLOAT)x,
				(FLOAT)y,
				&isTrailingHit,
				&isInside,
				&hitTestMetrics
			);
		}
		int posX = (int)(hitTestMetrics.left + 0.5);
		if (isTrailingHit) {//判断前侧还是尾侧
			posX += (int)(hitTestMetrics.width + 0.5);
		}
		textPos = hitTestMetrics.textPosition;
		return __Point{ posX,(int)(hitTestMetrics.top + 0.5) };//返回光标所在的位置
	}
	__Point TextLayout::HitTestTextPosition(int textPos, BOOL isTrailingHit) {
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		FLOAT X, Y;
		value->HitTestTextPosition(textPos, isTrailingHit, &X, &Y, &hitTestMetrics);
		return __Point((int)(X + 0.5), (int)(Y + 0.5));
	}
	__Size TextLayout::GetFontSize() {
		DWRITE_TEXT_METRICS textMetrics;
		value->GetMetrics(&textMetrics);
		D2D1_SIZE_F size = D2D1::SizeF(ceil(textMetrics.widthIncludingTrailingWhitespace), ceil(textMetrics.height));
		return  __Size{ (int)(size.width + 0.5) ,(int)((size.height / textMetrics.lineCount) + 0.5) };
	}
	IDWriteTextLayout* TextLayout::operator->() {
		return value;
	}
	TextLayout::operator IDWriteTextLayout* () {
		return value;
	}
	TextLayout::~TextLayout() {
		if (value) {
			value->Release();
		}
	}
	//DXImage
	void DXImage::DecodeOfRender(ID2D1RenderTarget* render) {
		if (d2dBitmap != NULL) {
			if (d2dBitmap) {
				SafeRelease(&d2dBitmap);
			}
		}
		if (fmtcovter) {
			render->CreateBitmapFromWicBitmap(fmtcovter, 0, &d2dBitmap);
		}
		if (bitMap) {
			render->CreateBitmapFromWicBitmap(bitMap, &d2dBitmap);
		}
	}
	UINT DXImage::GetWidth() {
		return Width;
	}
	UINT DXImage::GetHeight() {
		return Height;
	}
	void DXImage::CreateFormStream(IStream* istram) {
		if (D2D::g_ImageFactory) {
			D2D::g_ImageFactory->CreateDecoderFromStream(istram, NULL, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			D2D::g_ImageFactory->CreateFormatConverter(&fmtcovter);
			bitmapdecoder->GetFrame(0, &pframe);
			fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			pframe->GetSize(&Width, &Height);
			Init();
		}
	}
	void DXImage::CreateFromFile(const std::wstring& filew)
	{
		if (D2D::g_ImageFactory) {
			D2D::g_ImageFactory->CreateDecoderFromFilename(filew.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			D2D::g_ImageFactory->CreateFormatConverter(&fmtcovter);
			bitmapdecoder->GetFrame(0, &pframe);
			fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			pframe->GetSize(&Width, &Height);
			Init();
		}
	}
	void DXImage::Init() {
		_framePos = 0;
		UINT fCount = 0;
		if (bitmapdecoder) {
			bitmapdecoder->GetFrameCount(&fCount);
			_frameCount = fCount;
		}
		if (fCount > 1) {
			int a = 0;
		}
	}
	size_t DXImage::NextFrame() {
		if (_framePos >= _frameCount) {
			_framePos = 0;
		}
		if (pframe) {
			pframe->Release();
			pframe = NULL;
		}

		if (fmtcovter) {
			fmtcovter->Release();
			fmtcovter = NULL;
		}

		D2D::g_ImageFactory->CreateFormatConverter(&fmtcovter);
		bitmapdecoder->GetFrame(_framePos, &pframe);
		fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);

		_framePos++;
		return 60;
	}
	DXImage::DXImage(HBITMAP hBitmap) {
		if (D2D::g_ImageFactory) {
			D2D::g_ImageFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUsePremultipliedAlpha, &bitMap);
			bitMap->GetSize(&Width, &Height);
			Init();
		}
	}
	DXImage::DXImage(const std::wstring& file) {
		CreateFromFile(file);
	}
	DXImage::DXImage(IStream* istram) {
		CreateFormStream(istram);
	}
	DXImage::~DXImage()
	{
		if (d2dBitmap) {
			SafeRelease(&d2dBitmap);
		}
		if (bitmapdecoder) {
			SafeRelease(&bitmapdecoder);
		}
		if (pframe) {
			SafeRelease(&pframe);
		}
		if (fmtcovter) {
			SafeRelease(&fmtcovter);
		}
		if (bitMap) {
			SafeRelease(&bitMap);
		}
	}
};

namespace EzUI {
	void RenderInitialize()
	{
		HRESULT hr = S_OK;
		// Create a Direct2D factory.
		if (D2D::g_Direct2dFactory == NULL) {
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &D2D::g_Direct2dFactory);
			if (!D2D::g_Direct2dFactory) {
				::MessageBoxW(NULL, L"Failed to create ID2D1Factory", L"Error", MB_ICONSTOP);
			}
			else {
				D2D1_RENDER_TARGET_PROPERTIES defaultOption = D2D1::RenderTargetProperties(
					D2D1_RENDER_TARGET_TYPE_DEFAULT,
					D2D1::PixelFormat(
						DXGI_FORMAT_B8G8R8A8_UNORM,
						D2D1_ALPHA_MODE_IGNORE),
					0,
					0,
					D2D1_RENDER_TARGET_USAGE_NONE,
					D2D1_FEATURE_LEVEL_DEFAULT
				);
				//初始化一下DX让第一次启动窗口快一点
				ID2D1DCRenderTarget* initRender = NULL;
				HRESULT	hr = D2D::g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&initRender);
				SafeRelease(&initRender);
			}
		}
		if (D2D::g_WriteFactory == NULL) {
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&D2D::g_WriteFactory));
			if (!D2D::g_WriteFactory) {
				::MessageBoxW(NULL, L"Failed to create IDWriteFactory", L"Error", MB_ICONSTOP);
			}
		}
		if (D2D::g_ImageFactory == NULL) {
			_GUID imageFactoryOld{ 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa };//xp  win7 旧版
			_GUID WICImagingFactoryId = CLSID_WICImagingFactory;//当前平台
		ImagingFactoryInit:
			hr = CoCreateInstance(WICImagingFactoryId, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&D2D::g_ImageFactory);
			if (hr != S_OK) {
				//if (hr == 0x800401F0) {//未初始化com 自己在全局初始化一下就好了 (自己控制初始化时机)
				//	::CoInitialize(NULL);
				//	goto ImagingFactory;
				//}
				if (hr == 0x80040154) {//没有注册类 不用win7的sdk生成的程序在下win7系统上运行会出现此错误
					WICImagingFactoryId = imageFactoryOld;
					goto ImagingFactoryInit;
				}
				CHAR buf[256]{ 0 };
				sprintf_s(buf, "Code 0x%p", (void*)hr);
				::MessageBoxA(NULL, "Failed to create IWICImagingFactory", buf, MB_ICONSTOP);
			}
		}
	}
	void RenderUnInitialize()
	{
		if (D2D::g_Direct2dFactory) {
			SafeRelease(&D2D::g_Direct2dFactory);
		}
		if (D2D::g_WriteFactory) {
			SafeRelease(&D2D::g_WriteFactory);
		}
		if (D2D::g_ImageFactory) {
			SafeRelease(&D2D::g_ImageFactory);
		}
	}

	ID2D1DCRenderTarget* CreateRender(HDC _dc, int x, int y, int Width, int Height)
	{
		D2D1_RENDER_TARGET_PROPERTIES defaultOption = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);
		ID2D1DCRenderTarget* d2dRender = NULL;
		HRESULT	hr = D2D::g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&d2dRender);
		RECT rc{ x,y,x + Width ,y + Height };
		((ID2D1DCRenderTarget*)d2dRender)->BindDC(_dc, &rc);
		return d2dRender;
	}
	void ReleaseRender(ID2D1DCRenderTarget* d2dRender) {
		SafeRelease(&d2dRender);
	}
	void FillRectangle(ID2D1RenderTarget* d2dRender, const __Rect& _rect, const __Color& color, int _radius) {
		if (color.GetValue() == 0) {
			return;
		}
		SafeSolidColorBrush sb(d2dRender, color);
		const __Rect& rect = _rect;
		if (_radius > 0) {
			float radius = _radius / 2.0f;
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), radius, radius };
			d2dRender->FillRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->FillRectangle(__To_D2D_RectF(rect), sb);
		}
	}
	void DrawRectangle(ID2D1RenderTarget* d2dRender, const  __Rect& _rect, const  __Color& color, int width, int _radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		const __Rect& rect = _rect;
		SafeSolidColorBrush sb(d2dRender, color);
		if (_radius > 0) {
			float radius = _radius / 2.0f;
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), radius, radius };
			d2dRender->DrawRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->DrawRectangle(__To_D2D_RectF(rect), sb);
		}
	}
	void SetTransform(ID2D1RenderTarget* d2dRender, int xOffset, int yOffset)
	{
		// 设置x和y方向的偏移
		d2dRender->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)xOffset, (FLOAT)yOffset));
	}
	void DrawLine(ID2D1RenderTarget* d2dRender, const __Color& color, const __Point& _A, const __Point& _B, int width)
	{
		const __Point& A = _A;
		const __Point& B = _B;

		SafeSolidColorBrush sb(d2dRender, color);
		d2dRender->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, sb, (FLOAT)width);
	}
	void PushAxisAlignedClip(ID2D1RenderTarget* d2dRender, const __Rect& rectBounds) {
		d2dRender->PushAxisAlignedClip(__To_D2D_RectF(rectBounds), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
	void PopAxisAlignedClip(ID2D1RenderTarget* d2dRender) {//弹出最后一个裁剪
		d2dRender->PopAxisAlignedClip();
	}
	void PushLayer(ID2D1RenderTarget* d2dRender, const Geometry& dxGeometry)
	{
		ID2D1Layer* layer = NULL;
		d2dRender->CreateLayer(&layer);
		d2dRender->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), dxGeometry.rgn), layer);//放入layer
		layer->Release();
	}
	void PopLayer(ID2D1RenderTarget* d2dRender)//弹出最后一个裁剪
	{
		d2dRender->PopLayer();
	}
	void DrawImage(ID2D1RenderTarget* d2dRender, IImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Margin& margin)
	{
		_NOREND_IMAGE_
			if (_image == NULL) return;
		DXImage* image = (DXImage*)_image;
		//计算坐标
		__Rect rect = _rect;
		rect.X += margin.Left;
		rect.Y += margin.Top;
		rect.Width -= margin.Right * 2;
		rect.Height -= margin.Bottom * 2;
		//解码
		image->DecodeOfRender(d2dRender);
		//转换坐标,缩放
		__Size imgSize(image->GetWidth(), image->GetHeight());
		__Rect drawRect = EzUI::Transformation(imageSizeMode, rect, imgSize);
		//开始绘制
		if (image->d2dBitmap == NULL) return;
		d2dRender->DrawBitmap(image->d2dBitmap, __To_D2D_RectF(drawRect));
	}
	void DrawTextLayout(ID2D1RenderTarget* d2dRender, const __Point& startLacation, IDWriteTextLayout* textLayout, const __Color& color)
	{
		SafeSolidColorBrush sb(d2dRender, color);
		d2dRender->DrawTextLayout(D2D1_POINT_2F{ (FLOAT)(startLacation.X) ,(FLOAT)(startLacation.Y) }, textLayout, sb);
	}
	void DrawString(ID2D1RenderTarget* d2dRender, const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  __Color& color, const  __Rect& _rect, EzUI::TextAlign textAlign, bool underLine)
	{
		const __Rect& rect = _rect;
		TextFormat textFormat(fontFamily, fontSize, textAlign);
		TextLayout textLayout(text, __Size{ rect.Width, rect.Height }, &textFormat);
		if (underLine) {
			textLayout->SetUnderline(TRUE, { 0,(UINT32)text.size() });
		}
		DrawTextLayout(d2dRender, { _rect.X,_rect.Y }, textLayout, color);
	}
};

#endif