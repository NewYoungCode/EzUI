#include "Direct2DRender.h"
#ifdef USED_Direct2D
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Windowscodecs.lib")
namespace EzUI {
	ID2D1Factory* g_Direct2dFactory = NULL;
	IDWriteFactory* g_WriteFactory = NULL;
	IWICImagingFactory* g_ImageFactory = NULL;

	template<class Interface>
	inline void SafeRelease(
		Interface** ppInterfaceToRelease)
	{
		if (*ppInterfaceToRelease != NULL)
		{
			(*ppInterfaceToRelease)->Release();
			(*ppInterfaceToRelease) = NULL;
		}
	}

	template<typename T>
	class DxSafeObject {
	public:
		T*& value;
		DxSafeObject(T* v) :value(v) {

		}
		operator T* () {
			return value;
		}
		~DxSafeObject() {

		}
	};

	void RenderInitialize()
	{

		int* ii = new int;
		DxSafeObject<int> dx(ii);



		HRESULT hr = S_OK;
		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_Direct2dFactory);
		if (!g_Direct2dFactory) {
			::MessageBoxW(NULL, L"Failed to create ID2D1Factory", L"Error", MB_ICONSTOP);
		}
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_WriteFactory));
		if (!g_WriteFactory) {
			::MessageBoxW(NULL, L"Failed to create IDWriteFactory", L"Error", MB_ICONSTOP);
		}

		_GUID WICImagingFactoryId = CLSID_WICImagingFactory;
	ImagingFactory:
		hr = CoCreateInstance(WICImagingFactoryId, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&g_ImageFactory);
		if (hr != S_OK) {
			//if (hr == 0x800401F0) {//未初始化com 自己在全局初始化一下就好了 (自己控制初始化时机)
			//	::CoInitialize(NULL);
			//	goto ImagingFactory;
			//}
			if (hr == 0x80040154) {//没有注册类 不用win7的sdk生成的程序在下win7系统上运行会出现此错误
				_GUID imageFactoryWin7{ 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa };
				WICImagingFactoryId = imageFactoryWin7;
				goto ImagingFactory;
			}
			CHAR buf[256]{ 0 };
			sprintf_s(buf, "Code 0x%p", hr);
			::MessageBoxA(NULL, "Failed to create IWICImagingFactory", buf, MB_ICONSTOP);
		}

	}

	void RenderUnInitialize()
	{
		if (g_Direct2dFactory) {
			SafeRelease(&g_Direct2dFactory);
		}
		if (g_WriteFactory) {
			SafeRelease(&g_WriteFactory);
		}
		if (g_ImageFactory) {
			SafeRelease(&g_ImageFactory);
		}
	}

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

	DXImage::DXImage(const std::wstring& filew)
	{
		if (g_ImageFactory) {
			g_ImageFactory->CreateDecoderFromFilename(filew.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			bitmapdecoder->GetFrame(0, &pframe);
			g_ImageFactory->CreateFormatConverter(&fmtcovter);
			fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			pframe->GetSize(&Width, &Height);
		}
	}

	DXImage::DXImage(HBITMAP hBitmap) {
		if (g_ImageFactory) {
			g_ImageFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUsePremultipliedAlpha, &bitMap);
			bitMap->GetSize(&Width, &Height);
		}
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

	void Direct2DRender::DrawBitmap(ID2D1Bitmap* d2dBitmap, const  DX_Rect& rect)
	{
		if (d2dBitmap == NULL) return;
		D2D1_RECT_F rectF = ToRectF(rect);
		d2dRender->DrawBitmap(d2dBitmap, &rectF);
	}

	Direct2DRender::Direct2DRender(HDC _dc, int Width, int Height)
	{
		this->DC = _dc;
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
		HRESULT	hr = g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&d2dRender);
		RECT rc{ 0,0,Width ,Height };
		((ID2D1DCRenderTarget*)d2dRender)->BindDC(DC, &rc);
		this->BeginDraw();
	}

	Direct2DRender::Direct2DRender(HWND hWnd, int Width, int Height)
	{
		this->hWnd = hWnd;
		DC = ::GetDC(hWnd);
		D2D1_SIZE_U size = D2D1::SizeU(Width, Height);
		// Create a Direct2D render target.
		HRESULT hr = g_Direct2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			(ID2D1HwndRenderTarget**)&d2dRender);
		this->BeginDraw();
	}

	Direct2DRender::~Direct2DRender()
	{
		for (auto& it : CacheBrush) {
			it.second->Release();
		}
		CacheBrush.clear();
		for (auto& it : CacheTextFormat) {
			it.second->Release();
		}
		CacheTextFormat.clear();

		d2dRender->EndDraw();
		SafeRelease(&d2dRender);
		if (hWnd && DC) {
			::ReleaseDC(hWnd, DC);
		}
	}


	void Direct2DRender::DrawRectangle(const  DX_Rect& _rect, const  DX_Color& color, int width, int radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		DX_Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		ID2D1Brush* sb = CreateSafeSolidBrush(color);
		if (radius > 0) {
			radius = radius / 2.0;
			D2D1_ROUNDED_RECT roundRect{ ToRectF(rect), radius, radius };
			d2dRender->DrawRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->DrawRectangle(ToRectF(rect), sb);
		}

	}
	void Direct2DRender::FillRectangle(const DX_Rect& _rect, const DX_Color& color, int radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		DX_Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		ID2D1Brush* sb = CreateSafeSolidBrush(color);
		if (radius > 0) {
			radius = radius / 2.0;
			D2D1_ROUNDED_RECT roundRect{ ToRectF(rect), radius, radius };
			d2dRender->FillRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->FillRectangle(ToRectF(rect), sb);
		}
	}
	void Direct2DRender::DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const DX_Color& color, const DX_Rect& _rect, EzUI::TextAlign textAlign, bool underLine)
	{
		if (text == L"0") {

			int pause0 = 0;
		}

		DX_Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		IDWriteTextFormat* format = CreateSafeTextFormat(fontFamily, fontSize);

#define __Top DWRITE_PARAGRAPH_ALIGNMENT_NEAR
#define	__Bottom DWRITE_PARAGRAPH_ALIGNMENT_FAR
#define	__Left DWRITE_TEXT_ALIGNMENT_LEADING
#define	__Right DWRITE_TEXT_ALIGNMENT_TRAILING
#define	__Middle DWRITE_PARAGRAPH_ALIGNMENT_CENTER
#define __Center DWRITE_TEXT_ALIGNMENT_CENTER

		do
		{
			if (textAlign == TextAlign::BottomCenter) {
				format->SetParagraphAlignment(__Bottom);
				format->SetTextAlignment(__Center);
				break;
			}
			if (textAlign == TextAlign::BottomLeft) {
				format->SetParagraphAlignment(__Bottom);
				format->SetTextAlignment(__Left);
				break;
			}
			if (textAlign == TextAlign::BottomRight) {
				format->SetParagraphAlignment(__Bottom);
				format->SetTextAlignment(__Right);
				break;
			}
			if (textAlign == TextAlign::MiddleCenter) {
				format->SetParagraphAlignment(__Middle);
				format->SetTextAlignment(__Center);
				break;
			}
			if (textAlign == TextAlign::MiddleLeft) {
				format->SetParagraphAlignment(__Middle);
				format->SetTextAlignment(__Left);
				break;
			}
			if (textAlign == TextAlign::MiddleRight) {
				format->SetParagraphAlignment(__Middle);
				format->SetTextAlignment(__Right);
				break;
			}
			if (textAlign == TextAlign::TopCenter) {
				format->SetParagraphAlignment(__Top);
				format->SetTextAlignment(__Center);
				break;
			}
			if (textAlign == TextAlign::TopLeft) {
				format->SetParagraphAlignment(__Top);
				format->SetTextAlignment(__Left);
				break;
			}
			if (textAlign == TextAlign::TopRight) {
				format->SetParagraphAlignment(__Top);
				format->SetTextAlignment(__Right);
				break;
			}
		} while (0);

		ID2D1Brush* sb = CreateSafeSolidBrush(color);
		d2dRender->DrawTextW(text.c_str(), text.size(), format, ToRectF(rect), sb);
	}
	void Direct2DRender::MeasureString(const std::wstring& _text, const std::wstring& fontf, int fontSize, DX_RectF& outBox)
	{

	}

	void Direct2DRender::PushLayer(const DxGeometry& dxGeometry, EzUI::ClipMode clipMode)
	{
		ID2D1Layer* layer;
		d2dRender->CreateLayer(&layer);
		d2dRender->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), dxGeometry.Geometry), layer);//放入layer
		SafeRelease(&layer);
	}

	void Direct2DRender::PushLayer(const DX_Rect& rectBounds, ClipMode clipMode)
	{
		ID2D1Layer* layer;
		d2dRender->CreateLayer(&layer);
		d2dRender->PushLayer(D2D1::LayerParameters(ToRectF(rectBounds)), NULL);//放入layer
		SafeRelease(&layer);
	}

	void Direct2DRender::PopLayer()
	{
		d2dRender->PopLayer();
	}
	void Direct2DRender::DrawLine(const DX_Color& color, const DX_Point& _A, const DX_Point& _B, int width)
	{
		DX_Point A = _A;
		A.X += OffsetX;
		A.Y += OffsetY;
		DX_Point B = _B;
		B.X += OffsetX;
		B.Y += OffsetY;

		ID2D1Brush* sb = CreateSafeSolidBrush(color);
		d2dRender->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, sb);

	}
	void Direct2DRender::DrawImage(IImage* _image, const DX_Rect& destRect, const DX_Rect& srcRect)
	{
		DXImage* image = (DXImage*)_image;

	}
	void Direct2DRender::DrawImage(IImage* _image, const DX_Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Margin& margin)
	{
		if (_image == NULL) return;
		DXImage* image = (DXImage*)_image;

		DX_Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		rect.X += margin.Left;
		rect.Y += margin.Top;
		rect.Width -= margin.Right * 2;
		rect.Height -= margin.Bottom * 2;

		image->DecodeOfRender(d2dRender);

		if (imageSizeMode == ImageSizeMode::Zoom) {
			//客户端数据
			const int& clientWidth = rect.Width;
			const int& clientHeight = rect.Height;
			double clientRate = clientWidth * 1.0 / clientHeight;
			//图片数据
			int imgWidth = image->GetWidth();
			int imgHeight = image->GetHeight();
			double imgRate = imgWidth * 1.0 / imgHeight;
			if (clientRate < imgRate) {
				double zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;
				DX_Size sz{ clientWidth,(INT)zoomHeight };
				int y = (clientHeight - sz.Height) / 2 + rect.Y;

				this->DrawBitmap(image->d2dBitmap, DX_Rect{ rect.X  ,y, sz.Width, sz.Height });

			}
			else {
				double zoomWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;
				DX_Size sz{ (INT)zoomWidth,clientHeight };
				int x = (clientWidth - sz.Width) / 2 + rect.X;
				this->DrawBitmap(image->d2dBitmap, DX_Rect{ x  , rect.Y, sz.Width, sz.Height });
			}
			return;
		}
		if (imageSizeMode == ImageSizeMode::CenterImage) {
			//客户端数据
			const int& clientWidth = rect.Width;
			const int& clientHeight = rect.Height;
			double clientRate = clientWidth * 1.0 / clientHeight;
			//图片数据
			int imgWidth = image->GetWidth();
			int imgHeight = image->GetHeight();
			double imgRate = imgWidth * 1.0 / imgHeight;
			if (clientRate < imgRate) {
				//1000 670 客户端
				//1000 300 图片
				//2233 670     缩放后的图片大小 
				int zoomWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;//图片应该这么宽才对
				int x = (zoomWidth - clientWidth) * 1.0 / 2 + 0.5;
				this->DrawBitmap(image->d2dBitmap, DX_Rect{ rect.X - x,rect.Y,zoomWidth,clientHeight });
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				int zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;//图片应该这么高才对
				int y = (zoomHeight - clientHeight) * 1.0 / 2 + 0.5;
				this->DrawBitmap(image->d2dBitmap, DX_Rect{ rect.X,  rect.Y - y  , clientWidth, zoomHeight });
			}
			return;
		}

		if (imageSizeMode == ImageSizeMode::StretchImage || true) {
			this->DrawBitmap(image->d2dBitmap, rect);
		}

	}
	void Direct2DRender::BeginDraw()
	{
		if (!beginDraw) {
			d2dRender->BeginDraw();
			beginDraw = true;
		}
	}
	void Direct2DRender::FillGeometry(ID2D1Geometry* geometry, const DX_Color& color)
	{
		ID2D1Brush* sb = CreateSafeSolidBrush(color);
		d2dRender->FillGeometry(geometry, sb);
	}
	ID2D1Brush* Direct2DRender::CreateSafeSolidBrush(const DX_Color& _color)
	{
		auto key = _color.GetValue();
		auto itor = CacheBrush.find(key);
		if (itor != CacheBrush.end()) {
			return (*itor).second;//从缓存中返回ID2D1Brush
		}
		ID2D1SolidColorBrush* sb = NULL;
		d2dRender->CreateSolidColorBrush(ToColorF(_color), &sb);//
		CacheBrush.insert(std::pair<DX_ARGB, ID2D1Brush*>(key, sb));//加入缓存
		return sb;
	}
	IDWriteTextFormat* Direct2DRender::CreateSafeTextFormat(const std::wstring& fontFamily, int fontSize)
	{
		WCHAR key[LF_FACESIZE + 10]{ 0 };
		swprintf_s(key, L"%s_%d", fontFamily.c_str(), fontSize);
		auto itor = CacheTextFormat.find(key);
		if (itor != CacheTextFormat.end()) {
			return (*itor).second;//从缓存中返回TextFormat
		}
		IDWriteTextFormat* format = NULL;
		auto fh = MulDiv(fontSize, GetDeviceCaps(DC, LOGPIXELSY), 72);
		g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fh, L"", &format);
		CacheTextFormat.insert(std::pair<std::wstring, IDWriteTextFormat*>(key, format));//加入缓存
		return format;
	}

	void Direct2DRender::DrawGeometry(ID2D1Geometry* geometry, const DX_Color& color, int width)
	{
		ID2D1Brush* sb = CreateSafeSolidBrush(color);
		d2dRender->DrawGeometry(geometry, sb);
	}
	void Direct2DRender::EndDraw()
	{
		if (beginDraw) {
			d2dRender->EndDraw();
			beginDraw = false;
		}
	}

}


#endif