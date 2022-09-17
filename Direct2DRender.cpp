#include "Direct2DRender.h"
#ifdef USED_Direct2D
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Windowscodecs.lib")
namespace EzUI {
	ID2D1Factory* m_pDirect2dFactory = NULL;
	IDWriteFactory* g_write_factory = NULL;
	IWICImagingFactory* g_image_factory = NULL;

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

	void RenderInitialize()
	{
		HRESULT hr = S_OK;
		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
		if (!m_pDirect2dFactory) {
			::MessageBoxW(NULL, L"Failed to create ID2D1Factory", L"Error", 0);
		}
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_write_factory));
		if (!g_write_factory) {
			::MessageBoxW(NULL, L"Failed to create IDWriteFactory", L"Error", 0);
		}
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&g_image_factory);
		if (!g_image_factory) {
			::MessageBoxW(NULL, L"Failed to create IWICImagingFactory", L"Error", 0);
		}
	}
	void RenderUnInitialize()
	{
		if (m_pDirect2dFactory) {
			SafeRelease(&m_pDirect2dFactory);
		}
		if (g_write_factory) {
			SafeRelease(&g_write_factory);
		}
		if (g_image_factory) {
			SafeRelease(&g_image_factory);
		}
	}

	void D2DImage::DecodeOfRender(ID2D1RenderTarget* render) {
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

	UINT D2DImage::GetWidth() {
		return Width;
	}
	UINT D2DImage::GetHeight() {
		return Height;
	}

	D2DImage::D2DImage(const std::wstring& filew)
	{
		if (g_image_factory) {
			g_image_factory->CreateDecoderFromFilename(filew.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			bitmapdecoder->GetFrame(0, &pframe);
			g_image_factory->CreateFormatConverter(&fmtcovter);
			fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			pframe->GetSize(&Width, &Height);
		}
	}

	D2DImage::D2DImage(HBITMAP hBitmap) {
		if (g_image_factory) {
			g_image_factory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUsePremultipliedAlpha, &bitMap);
			bitMap->GetSize(&Width, &Height);
		}
	}

	D2DImage::~D2DImage()
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

	void Direct2DRender::DrawBitmap(ID2D1Bitmap* d2dBitmap, const Rect& rect)
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
		HRESULT	hr = m_pDirect2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&d2dRender);
		RECT rc{ 0,0,Width ,Height };
		((ID2D1DCRenderTarget*)d2dRender)->BindDC(DC, &rc);
		this->BeginDraw();
	}

	Direct2DRender::Direct2DRender(HWND hWnd, int Width, int Height)
	{
		D2D1_SIZE_U size = D2D1::SizeU(Width, Height);
		// Create a Direct2D render target.
		HRESULT hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			(ID2D1HwndRenderTarget**)&d2dRender);
		DC=::GetDC(hWnd);
		this->BeginDraw();
	}

	Direct2DRender::~Direct2DRender()
	{
		d2dRender->EndDraw();
		SafeRelease(&d2dRender);
		if (hWnd && DC) {
			::ReleaseDC(hWnd, DC);
		}
	}
	void Direct2DRender::DrawRectangle(const Rect& _rect, const Color& color, int width, int radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		ID2D1SolidColorBrush* sb;
		auto hr = d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		if (radius > 0) {
			radius = radius / 2.0;
			D2D1_ROUNDED_RECT roundRect{ ToRectF(rect), radius, radius };
			d2dRender->DrawRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->DrawRectangle(ToRectF(rect), sb);
		}
		SafeRelease(&sb);

		SafeRelease(&sb);
	}
	void Direct2DRender::FillRectangle(const Rect& _rect, const Color& color, int radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		ID2D1SolidColorBrush* sb;
		auto hr = d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		if (radius > 0) {
			radius = radius / 2.0;
			D2D1_ROUNDED_RECT roundRect{ ToRectF(rect), radius, radius };
			d2dRender->FillRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->FillRectangle(ToRectF(rect), sb);
		}
		SafeRelease(&sb);
	}
	void Direct2DRender::DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const Color& color, const Rect& _rect, TextAlign textAlign, bool underLine)
	{
		if (text == L"0") {

			int pause0 = 0;
		}

		Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		IDWriteTextFormat* format;
		auto fh =  MulDiv(fontSize, GetDeviceCaps(DC, LOGPIXELSY), 72);
		g_write_factory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fh, L"", &format);

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

		ID2D1SolidColorBrush* sb;
		auto hr = d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		d2dRender->DrawTextW(text.c_str(), text.size(), format, ToRectF(rect), sb);

		SafeRelease(&sb);
		SafeRelease(&format);

	}
	void Direct2DRender::MeasureString(const std::wstring& _text, const std::wstring& fontf, int fontSize, RectF& outBox)
	{
	}
	void Direct2DRender::CreateLayer(const Layer* layer, ClipMode clipMode)
	{
		Layers.push_back((Layer*)layer);
		if (layer->ClipRect) {
			auto& rect = *layer->ClipRect;
			D2D1_RECT_F rectF{ (float)rect.X,(float)rect.Y,(float)rect.GetRight(),(float)rect.GetBottom() };
			d2dRender->PushLayer(D2D1::LayerParameters(rectF), NULL);
		}
		//else {
		//	//::SelectClipRgn(DC, layer->RGN);
		//}
	}
	void Direct2DRender::PopLayer()
	{
		d2dRender->PopLayer();
		if (!Layers.empty()) {
			Layers.pop_back();
			//if (!Layers.empty()) {
			//	Layer& it = *(Layers.back());
			//	if (it.ClipRect) {
			//		Rect& rect = *it.ClipRect;
			//		D2D1_RECT_F rectF = ToRectF(rect);
			//		//d2dRender->PushLayer(D2D1::LayerParameters(rectF), NULL);
			//	}
			//	else
			//	{
			//		//base->SetClip(it.RGN);
			//	}
			//}
		}
	}
	void Direct2DRender::DrawLine(const Color& color, const Point& _A, const Point& _B, int width)
	{
		Point A = _A;
		A.X += OffsetX;
		A.Y += OffsetY;
		Point B = _B;
		B.X += OffsetX;
		B.Y += OffsetY;

		ID2D1SolidColorBrush* sb;
		auto hr = d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		d2dRender->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, sb);
		SafeRelease(&sb);

	}
	void Direct2DRender::DrawImage(IImage* _image, const Rect& destRect, const Rect& srcRect)
	{
		D2DImage* image = (D2DImage*)_image;

	}
	void Direct2DRender::DrawImage(IImage* _image, const Rect& _rect, const ImageSizeMode& imageSizeMode, const Margin& margin)
	{
		if (_image == NULL) return;
		D2DImage* image = (D2DImage*)_image;

		Rect rect = _rect;
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
				Size sz{ clientWidth,(INT)zoomHeight };
				int y = (clientHeight - sz.Height) / 2 + rect.Y;

				this->DrawBitmap(image->d2dBitmap, Rect{ rect.X  ,y, sz.Width, sz.Height });

			}
			else {
				double zoomWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;
				Size sz{ (INT)zoomWidth,clientHeight };
				int x = (clientWidth - sz.Width) / 2 + rect.X;
				this->DrawBitmap(image->d2dBitmap, Rect{ x  , rect.Y, sz.Width, sz.Height });
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
				this->DrawBitmap(image->d2dBitmap, Rect{ rect.X - x,rect.Y,zoomWidth,clientHeight });
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				int zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;//图片应该这么高才对
				int y = (zoomHeight - clientHeight) * 1.0 / 2 + 0.5;
				this->DrawBitmap(image->d2dBitmap, Rect{ rect.X,  rect.Y - y  , clientWidth, zoomHeight });
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
	void Direct2DRender::EndDraw()
	{
		if (beginDraw) {
			d2dRender->EndDraw();
			beginDraw = false;
		}
	}


}
#endif