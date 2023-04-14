#include "Direct2DRender.h"
#if USED_Direct2D
#define _NOREND_IMAGE_ 
//#define _NOREND_IMAGE_ return;
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Windowscodecs.lib")
namespace EzUI {
	ID2D1Factory* g_Direct2dFactory = NULL;
	IDWriteFactory* g_WriteFactory = NULL;
	IWICImagingFactory* g_ImageFactory = NULL;
	float Scale = 1.0f;

	D2D_COLOR_F ToColorF(const  __Color& color) {
		FLOAT aF = color.GetA() == 255 ? 1.0f : FLOAT(color.GetA() * 0.003921f);
		FLOAT rF = FLOAT(color.GetR() * 0.003921);
		FLOAT gF = FLOAT(color.GetG() * 0.003921);
		FLOAT bF = FLOAT(color.GetB() * 0.003921);
		return D2D1::ColorF(rF, gF, bF, aF);
	}
	D2D_RECT_F ToRectF(const __Rect& rect) {
		return D2D_RECT_F{ (FLOAT)rect.X,(FLOAT)rect.Y,(FLOAT)rect.GetRight(),(FLOAT)rect.GetBottom() };
	}

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

	template<typename T>
	class Smart_ptr {
		T* value = NULL;
	public:
		Smart_ptr(T* t) {
			value = t;
		}
		~Smart_ptr() {
			if (value) {
				value->Release();
			}
		}
	};

	void RenderInitialize()
	{
		HRESULT hr = S_OK;
		// Create a Direct2D factory.
		if (g_Direct2dFactory == NULL) {
			hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &g_Direct2dFactory);
			if (!g_Direct2dFactory) {
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
				HRESULT	hr = g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&initRender);
				SafeRelease(&initRender);
			}
		}
		if (g_WriteFactory == NULL) {
			hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&g_WriteFactory));
			if (!g_WriteFactory) {
				::MessageBoxW(NULL, L"Failed to create IDWriteFactory", L"Error", MB_ICONSTOP);
			}
		}
		if (g_ImageFactory == NULL) {
			_GUID imageFactoryOld{ 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa };//xp  win7 旧版
			_GUID WICImagingFactoryId = CLSID_WICImagingFactory;//当前平台
		ImagingFactoryInit:
			hr = CoCreateInstance(WICImagingFactoryId, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&g_ImageFactory);
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

	void DXImage::CreateFormStream(IStream* istram) {
		if (g_ImageFactory) {
			g_ImageFactory->CreateDecoderFromStream(istram, NULL, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//

		}
		if (bitmapdecoder) {
			g_ImageFactory->CreateFormatConverter(&fmtcovter);


			bitmapdecoder->GetFrame(0, &pframe);
			fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			pframe->GetSize(&Width, &Height);
			Init();
		}
	}

	void DXImage::CreateFromFile(const std::wstring& filew)
	{
		if (g_ImageFactory) {
			g_ImageFactory->CreateDecoderFromFilename(filew.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			g_ImageFactory->CreateFormatConverter(&fmtcovter);

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
	//跳转到下一帧 并且获取下一帧的延迟
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

		g_ImageFactory->CreateFormatConverter(&fmtcovter);
		bitmapdecoder->GetFrame(_framePos, &pframe);
		fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);

		_framePos++;
		return 60;
	}

	DXImage::DXImage(HBITMAP hBitmap) {
		if (g_ImageFactory) {
			g_ImageFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUsePremultipliedAlpha, &bitMap);
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
	ID2D1DCRenderTarget* CreateRender(HDC _dc, int Width, int Height)
	{
		ID2D1DCRenderTarget* d2dRender = NULL;
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
		((ID2D1DCRenderTarget*)d2dRender)->BindDC(_dc, &rc);
		return d2dRender;
	}
	void ReleaseRender(ID2D1DCRenderTarget* d2dRender) {
		d2dRender->Release();
	}
	void FillRectangle(ID2D1RenderTarget* d2dRender, const __Rect& _rect, const __Color& color, int _radius) {

		ID2D1SolidColorBrush* sb = NULL;
		d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		Smart_ptr<ID2D1SolidColorBrush> smptr(sb);
		if (color.GetValue() == 0) {
			return;
		}
		const __Rect& rect = _rect;
		if (_radius > 0) {
			float radius = _radius / 2.0f;
			D2D1_ROUNDED_RECT roundRect{ ToRectF(rect), radius, radius };
			d2dRender->FillRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->FillRectangle(ToRectF(rect), sb);
		}
	}
	void DrawRectangle(ID2D1RenderTarget* d2dRender, const  __Rect& _rect, const  __Color& color, int width, int _radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		const __Rect& rect = _rect;
		ID2D1SolidColorBrush* sb = NULL;
		d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		Smart_ptr<ID2D1SolidColorBrush> smptr(sb);
		if (_radius > 0) {
			float radius = _radius / 2.0f;
			D2D1_ROUNDED_RECT roundRect{ ToRectF(rect), radius, radius };
			d2dRender->DrawRoundedRectangle(roundRect, sb);
		}
		else {
			d2dRender->DrawRectangle(ToRectF(rect), sb);
		}
	}
	void SetTransform(ID2D1RenderTarget* d2dRender, int xOffset, int yOffset)
	{
		// 设置x和y方向的偏移
		D2D1_MATRIX_3X2_F transformMatrix = D2D1::Matrix3x2F::Translation((FLOAT)xOffset, (FLOAT)yOffset);
		d2dRender->SetTransform(transformMatrix);
	}
	void DrawLine(ID2D1RenderTarget* d2dRender, const __Color& color, const __Point& _A, const __Point& _B, int width)
	{
		const __Point& A = _A;
		const __Point& B = _B;

		ID2D1SolidColorBrush* sb = NULL;
		d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		Smart_ptr<ID2D1SolidColorBrush> smptr(sb);
		d2dRender->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, sb, (FLOAT)width);
	}
	void PushAxisAlignedClip(ID2D1RenderTarget* d2dRender, const __Rect& rectBounds) {
		d2dRender->PushAxisAlignedClip(ToRectF(rectBounds), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
	void PopAxisAlignedClip(ID2D1RenderTarget* d2dRender) {//弹出最后一个裁剪
		d2dRender->PopAxisAlignedClip();
	}
	void PushLayer(ID2D1RenderTarget* d2dRender, const Geometry& dxGeometry)
	{
		ID2D1Layer* layer;
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
		D2D1_RECT_F rectF = ToRectF(drawRect);
		d2dRender->DrawBitmap(image->d2dBitmap, &rectF);
	}
	void DrawTextLayout(ID2D1RenderTarget* d2dRender, const __Point& startLacation, IDWriteTextLayout* textLayout, const __Color& color)
	{
		ID2D1SolidColorBrush* sb = NULL;
		d2dRender->CreateSolidColorBrush(ToColorF(color), &sb);
		Smart_ptr<ID2D1SolidColorBrush> smptr(sb);
		d2dRender->DrawTextLayout(D2D1_POINT_2F{ (FLOAT)(startLacation.X) ,(FLOAT)(startLacation.Y) }, textLayout, sb);
	}
	void DrawString(ID2D1RenderTarget* d2dRender, const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  __Color& color, const  __Rect& _rect, EzUI::TextAlign textAlign, bool underLine)
	{
		__Rect rect = _rect;
		TextFormat textFormat(fontFamily, fontSize, textAlign);
		TextLayout textLayout(text, __Size{ rect.Width, rect.Height }, &textFormat);
		if (underLine) {
			textLayout->SetUnderline(TRUE, { 0,(UINT32)text.size() });
		}
		DrawTextLayout(d2dRender, { _rect.X,_rect.Y }, textLayout, color);
	}
};

#endif