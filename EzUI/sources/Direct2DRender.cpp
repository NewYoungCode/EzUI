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
#define __To_D2D_PointF(pt)  D2D1_POINT_2F{(FLOAT)pt.X,(FLOAT)pt.Y}

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

	//TextFormat
	Font::Font(const std::wstring& fontFamily, int fontSize) {
		this->fontFamily = fontFamily;
		this->fontSize = fontSize;// *1.25f;//缩放字体
		D2D::g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (FLOAT)this->fontSize, L"", &value);
	}
	Font::~Font() {
		if (value && !Ref) {
			value->Release();
		}
	}
	const int& Font::GetFontSize()const {
		return fontSize;
	}
	const std::wstring& Font::GetFontFamily()const {
		return fontFamily;
	}
	void Font::Copy(const Font& _copy) {
		((Font&)(_copy)).Ref = true;
		this->value = _copy.Get();
		this->fontFamily = _copy.GetFontFamily();
		this->fontSize = _copy.GetFontSize();
	}
	IDWriteTextFormat* Font::Get() const {
		return value;
	}
	Font::Font(const Font& _copy) {
		Copy(_copy);
	}
	Font& Font::operator=(const Font& _copy) {
		Copy(_copy);
		return *this;
	}
	bool Font::operator==(const Font& _right) {
		if (_right.GetFontFamily() == this->GetFontFamily() && _right.GetFontSize() == this->GetFontSize()) {
			return true;
		}
		return false;
	}
	//TextLayout
	TextLayout::TextLayout(const std::wstring& text, const Font& font, Size maxSize, TextAlign textAlign) {
		D2D::g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), font.Get(), (FLOAT)maxSize.Width, (FLOAT)maxSize.Height, &value);
		if (value == NULL)return;
		SetTextAlign(textAlign);
		value->GetMetrics(&textMetrics);
	}
	IDWriteTextLayout* TextLayout::Get() const {
		return value;
	}
	Point TextLayout::HitTestPoint(const Point& pt, int* _textPos, BOOL* _isTrailingHit, int* fontHeight) {
		int& textPos = *_textPos;
		BOOL& isTrailingHit = *_isTrailingHit;
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
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
		*fontHeight = (int)(hitTestMetrics.height + 0.5);
		textPos = hitTestMetrics.textPosition;
		return Point{ posX,(int)(hitTestMetrics.top + 0.5) };//返回光标所在的位置
	}

	void TextLayout::HitTestPoint(const Point& pt, HitTestMetrics* outHitTestMetrics) {
		BOOL isTrailingHit;
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
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
		outHitTestMetrics->Length = hitTestMetrics.length;
		outHitTestMetrics->TextPos = hitTestMetrics.textPosition;
		outHitTestMetrics->IsTrailingHit = isTrailingHit;
		outHitTestMetrics->FontBox.X = hitTestMetrics.left;
		outHitTestMetrics->FontBox.Y = hitTestMetrics.top;
		outHitTestMetrics->FontBox.Width = hitTestMetrics.width;
		outHitTestMetrics->FontBox.Height = hitTestMetrics.height;
	}

	Point TextLayout::HitTestTextPosition(int textPos, BOOL isTrailingHit) {
		if (value == NULL)return Point{ 0,0 };
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		FLOAT X, Y;
		value->HitTestTextPosition(textPos, isTrailingHit, &X, &Y, &hitTestMetrics);
		return Point((int)(X + 0.5), (int)(Y + 0.5));
	}
	Size TextLayout::GetFontBox() {
		FLOAT width = textMetrics.widthIncludingTrailingWhitespace;
		FLOAT height = textMetrics.height;
		return  Size{ (int)(width + 0.5) ,(int)(height + 0.5) };
	}
	int TextLayout::Width() {
		FLOAT width = textMetrics.widthIncludingTrailingWhitespace;
		return (int)(width + 0.5);
	}
	int TextLayout::Height() {
		FLOAT width = textMetrics.height;
		return (int)(width + 0.5);
	}

	int TextLayout::GetFontHeight() {
		FLOAT height = textMetrics.height;
		return  (int)((height / textMetrics.lineCount) + 0.5);
	}
	int TextLayout::GetLineCount() {
		return textMetrics.lineCount;
	}

	Rect TextLayout::GetLineBox(int lineIndex) {
		UINT lineCount = GetLineCount();
		//// 获取每一行的宽高
		DWRITE_LINE_METRICS* hitTestMetrics = new DWRITE_LINE_METRICS[lineCount]{ 0 };
		FLOAT x = 0;
		FLOAT y = 0;
		value->GetLineMetrics(hitTestMetrics, lineCount, &lineCount);
		float count = 0;
		for (UINT32 i = 0; i < lineCount; i++)
		{
			auto it = hitTestMetrics[i];
			int pause = 0;
		}
		delete[] hitTestMetrics;
		return Rect();
	}

	void TextLayout::SetTextAlign(TextAlign textAlign) {
#define __Top DWRITE_PARAGRAPH_ALIGNMENT_NEAR
#define	__Bottom DWRITE_PARAGRAPH_ALIGNMENT_FAR
#define	__Left DWRITE_TEXT_ALIGNMENT_LEADING
#define	__Right DWRITE_TEXT_ALIGNMENT_TRAILING
#define	__Middle DWRITE_PARAGRAPH_ALIGNMENT_CENTER
#define __Center DWRITE_TEXT_ALIGNMENT_CENTER
		if (value == NULL)return;
		//垂直对其方式
		if (((int)textAlign & (int)Align::Top) == (int)Align::Top) {
			value->SetParagraphAlignment(__Top);
		}
		if (((int)textAlign & (int)Align::Mid) == (int)Align::Mid) {
			value->SetParagraphAlignment(__Middle);
		}
		if (((int)textAlign & (int)Align::Bottom) == (int)Align::Bottom) {
			value->SetParagraphAlignment(__Bottom);
		}
		//水平对其方式
		if (((int)textAlign & (int)Align::Left) == (int)Align::Left) {
			value->SetTextAlignment(__Left);
		}
		if (((int)textAlign & (int)Align::Center) == (int)Align::Center) {
			value->SetTextAlignment(__Center);
		}
		if (((int)textAlign & (int)Align::Right) == (int)Align::Right) {
			value->SetTextAlignment(__Right);
		}
#undef __Top 
#undef __Bottom 
#undef __Left 
#undef __Right 
#undef __Middle 
#undef __Center
	}
	void TextLayout::SetUnderline(size_t pos, size_t count)
	{
		if (count > 0) {
			value->SetUnderline(TRUE, { pos,count });
		}
	}
	TextLayout::~TextLayout() {
		if (value) {
			value->Release();
		}
	}
	//DXImage
	void DXImage::DecodeOfRender(ID2D1RenderTarget* render) {
		HRESULT hr = 0;
		if (d2dBitmap != NULL) {
			if (d2dBitmap) {
				SafeRelease(&d2dBitmap);
			}
		}
		if (fmtcovter) {
			hr = render->CreateBitmapFromWicBitmap(fmtcovter, 0, &d2dBitmap);
		}
		if (bitMap) {
			hr = render->CreateBitmapFromWicBitmap(bitMap, &d2dBitmap);
		}
	}
	UINT DXImage::GetWidth() {
		return Width;
	}
	UINT DXImage::GetHeight() {
		return Height;
	}
	void DXImage::CreateFormStream(IStream* istram) {
		HRESULT ret = S_OK;
		if (D2D::g_ImageFactory) {
			ret = D2D::g_ImageFactory->CreateDecoderFromStream(istram, NULL, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
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
	DXImage* DXImage::Clone()
	{
		UINT width, height;
		fmtcovter->GetSize(&width, &height); // 获取位图源的宽度和高度
		//暂时不做clone函数
		return NULL;
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
	DXImage::DXImage(UINT width, UINT height)
	{
		HRESULT hr = D2D::g_ImageFactory->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &bitMap);
	}
	DXImage::DXImage(const void* data, size_t count)
	{
		IStream* stream = SHCreateMemStream((BYTE*)data, count);
		if (stream) {
			this->CreateFormStream(stream);
			stream->Release();
		}
	}
	ID2D1Bitmap* DXImage::Get()
	{
		return d2dBitmap;
	}
	IWICBitmap* DXImage::GetIWICBitmap()
	{
		return bitMap;
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
	Geometry::Geometry(int x, int y, int width, int height) {
		D2D_RECT_F rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) };
		D2D::g_Direct2dFactory->CreateRectangleGeometry(rectF, (ID2D1RectangleGeometry**)&rgn);
	}
	Geometry::Geometry(int x, int y, int width, int height, int _radius) {
		float radius = GetMaxRadius(width, height, (float)_radius);
		D2D1_ROUNDED_RECT rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) ,radius ,radius };
		D2D::g_Direct2dFactory->CreateRoundedRectangleGeometry(rectF, (ID2D1RoundedRectangleGeometry**)&rgn);
	}
	Geometry::Geometry(const Rect& _rect, int topLeftRadius, int topRightRadius, int bottomRightRadius, int bottomLeftRadius)
	{
		ID2D1PathGeometry* pPathGeometry = NULL;
		D2D1_RECT_F rect = __To_D2D_RectF(_rect);
		D2D::g_Direct2dFactory->CreatePathGeometry(&pPathGeometry);
		topLeftRadius = GetMaxRadius(_rect.Width, _rect.Height, topLeftRadius);
		topRightRadius = GetMaxRadius(_rect.Width, _rect.Height, topRightRadius);
		bottomRightRadius = GetMaxRadius(_rect.Width, _rect.Height, bottomRightRadius);
		bottomLeftRadius = GetMaxRadius(_rect.Width, _rect.Height, bottomLeftRadius);
		// 打开路径几何图形的几何接口
		ID2D1GeometrySink* pSink;
		pPathGeometry->Open(&pSink);
		// 开始绘制路径
		pSink->BeginFigure(D2D1::Point2F(rect.left, rect.top + topLeftRadius), D2D1_FIGURE_BEGIN_FILLED);
		// 添加弧线段到左上角
		pSink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect.left + topLeftRadius, rect.top),
				D2D1::SizeF(topLeftRadius, topLeftRadius),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		// 添加弧线段到右上角
		pSink->AddLine(D2D1::Point2F(rect.right - topRightRadius, rect.top));
		pSink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect.right, rect.top + topRightRadius),
				D2D1::SizeF(topRightRadius, topRightRadius),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		// 添加弧线段到右下角
		pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom - bottomRightRadius));
		pSink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect.right - bottomRightRadius, rect.bottom),
				D2D1::SizeF(bottomRightRadius, bottomRightRadius),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		// 添加弧线段到左下角
		pSink->AddLine(D2D1::Point2F(rect.left + bottomLeftRadius, rect.bottom));
		pSink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect.left, rect.bottom - bottomLeftRadius),
				D2D1::SizeF(bottomLeftRadius, bottomLeftRadius),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		// 结束路径
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		pSink->Close();
		pSink->Release();
		this->rgn = pPathGeometry;
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
	float GetMaxRadius(int width, int height, float _radius)
	{
		float radius = (float)_radius;//半径
		float diameter = radius * 2;//直径
		if (width > height || width == height) {
			if (diameter > height) {
				radius = height / 2.0f;
			}
		}
		else if (height > width) {
			if (diameter > width) {
				radius = width / 2.0f;
			}
		}
		return  radius;
	}
	DXRender::DXRender(HDC dc, int x, int y, int width, int height) {
		D2D1_RENDER_TARGET_PROPERTIES defaultOption = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED),//解决在分层窗中的鼠标穿透问题
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);
		HRESULT	hr = D2D::g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&render);
		RECT rc{ x,y,x + width ,y + height };
		((ID2D1DCRenderTarget*)render)->BindDC(dc, &rc);
		render->BeginDraw();
	}

	DXRender::DXRender(DXImage* dxImage) {
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
		HRESULT hr = D2D::g_Direct2dFactory->CreateWicBitmapRenderTarget(dxImage->GetIWICBitmap(), defaultOption, (ID2D1RenderTarget**)&render);
		render->BeginDraw();
	}


	DXRender::~DXRender() {
		if (render) {
			render->EndDraw();
		}
		if (font) {
			delete font;
		}
		SafeRelease(&render);
		SafeRelease(&brush);
		SafeRelease(&pStrokeStyle);
	}
	ID2D1SolidColorBrush* DXRender::GetBrush()
	{
		if (brush == NULL) {
			render->CreateSolidColorBrush(D2D_COLOR_F{ 0,0,0,1 }, &brush);
		}
		return brush;
	}
	ID2D1StrokeStyle* DXRender::GetStrokeStyle() {
		return pStrokeStyle;
	}
	void DXRender::SetFont(const std::wstring& fontFamily, int fontSize) {
		if (font != NULL) {
			if (font->GetFontFamily() == fontFamily && font->GetFontSize() == fontSize) {
				return;
			}
			delete font;
		}
		font = new Font(fontFamily, fontSize);
	}
	void DXRender::SetFont(const Font& _copy_font) {
		if (font != NULL) {
			if (*font == _copy_font) {
				return;
			}
			delete font;
		}
		font = new Font(_copy_font);
	}
	void DXRender::SetColor(const Color& color) {
		if (brush == NULL) {
			render->CreateSolidColorBrush(__To_D2D_COLOR_F(color), &brush);
		}
		else {
			brush->SetColor(__To_D2D_COLOR_F(color));
		}
	}
	void DXRender::SetStrokeStyle(StrokeStyle strokeStyle, int dashWidth)
	{
		if (pStrokeStyle != NULL) {
			SafeRelease(&pStrokeStyle);
		}
		if (strokeStyle == StrokeStyle::Dash) {
			float* dashes = new float[2]{ (FLOAT)dashWidth, (FLOAT)dashWidth };
			const int count = 2;
			D2D::g_Direct2dFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_CUSTOM, 0.0f),
				dashes, count, &pStrokeStyle);
			delete[] dashes;
		}
	}
	void DXRender::DrawString(const TextLayout& textLayout, Point startLacation) {
		render->DrawTextLayout(D2D1_POINT_2F{ (FLOAT)(startLacation.X) ,(FLOAT)(startLacation.Y) }, textLayout.Get(), GetBrush());
	}
	void DXRender::DrawString(const std::wstring& text, const  Rect& _rect, EzUI::TextAlign textAlign) {
		const Rect& rect = _rect;
		TextLayout textLayout(text, *font, Size{ rect.Width, rect.Height }, textAlign);
		this->DrawString(textLayout, { _rect.X,_rect.Y });
	}
	void DXRender::DrawLine(const Point& _A, const Point& _B, int width) {
		const Point& A = _A;
		const Point& B = _B;
		render->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, GetBrush(), (FLOAT)width, GetStrokeStyle());
	}
	void DXRender::DrawRectangle(const  Rect& _rect, int _radius, int width) {
		const Rect& rect = _rect;
		if (_radius > 0) {
			float radius = GetMaxRadius(_rect.Width, _rect.Height, (float)_radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), (float)radius, (float)radius };
			render->DrawRoundedRectangle(roundRect, GetBrush(), (FLOAT)width, GetStrokeStyle());
		}
		else {
			render->DrawRectangle(__To_D2D_RectF(rect), GetBrush(), (FLOAT)width, GetStrokeStyle());
		}
	}
	void DXRender::FillRectangle(const Rect& _rect, int _radius) {
		const Rect& rect = _rect;
		if (_radius > 0) {
			float radius = GetMaxRadius(_rect.Width, _rect.Height, (float)_radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), (float)radius, (float)radius };
			render->FillRoundedRectangle(roundRect, GetBrush());
		}
		else {
			render->FillRectangle(__To_D2D_RectF(rect), GetBrush());
		}
	}
	void DXRender::SetTransform(int xOffset, int yOffset, int angle)
	{
		if (xOffset == 0 && yOffset == 0 && angle == 0) {
			render->SetTransform(D2D1::Matrix3x2F::Identity());
		}
		else if (angle != 0) {
			render->SetTransform(D2D1::Matrix3x2F::Rotation((FLOAT)angle, D2D1::Point2F((FLOAT)xOffset, (FLOAT)yOffset)));
		}
		else if (angle == 0) {
			// 设置x和y方向的偏移
			render->SetTransform(D2D1::Matrix3x2F::Translation((FLOAT)xOffset, (FLOAT)yOffset));
		}
	}
	void DXRender::DrawBezier(const Point& startPoint, const Bezier& points, int width) {
		ID2D1GeometrySink* pSink = NULL;
		ID2D1PathGeometry* pathGeometry = NULL;
		D2D::g_Direct2dFactory->CreatePathGeometry(&pathGeometry);
		pathGeometry->Open(&pSink);
		pSink->BeginFigure(__To_D2D_PointF(startPoint), D2D1_FIGURE_BEGIN_FILLED);
		D2D1_BEZIER_SEGMENT bzr{ __To_D2D_PointF(points.point1) ,__To_D2D_PointF(points.point2) ,__To_D2D_PointF(points.point3) };
		pSink->AddBezier(bzr);
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		pSink->Close();
		render->DrawGeometry(pathGeometry, GetBrush(), (FLOAT)width, GetStrokeStyle());
		SafeRelease(&pathGeometry);
		SafeRelease(&pSink);
	}
	void DXRender::DrawBezier(const Point& startPoint, std::list<Bezier>& beziers, int width)
	{
		ID2D1GeometrySink* pSink = NULL;
		ID2D1PathGeometry* pathGeometry = NULL;
		D2D::g_Direct2dFactory->CreatePathGeometry(&pathGeometry);
		pathGeometry->Open(&pSink);
		pSink->BeginFigure(__To_D2D_PointF(startPoint), D2D1_FIGURE_BEGIN_FILLED);
		for (auto& it : beziers) {
			D2D1_BEZIER_SEGMENT bzr{ __To_D2D_PointF(it.point1) ,__To_D2D_PointF(it.point2) ,__To_D2D_PointF(it.point3) };
			pSink->AddBezier(bzr);
		}
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		pSink->Close();
		render->DrawGeometry(pathGeometry, GetBrush(), (FLOAT)width, GetStrokeStyle());
		SafeRelease(&pathGeometry);
		SafeRelease(&pSink);
	}
	void DXRender::PushLayer(const Geometry& dxGeometry) {
		ID2D1Layer* layer = NULL;
		render->CreateLayer(&layer);
		render->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), dxGeometry.Get()), layer);//放入layer
		layer->Release();
		layers.push_back(false);
	}
	void DXRender::PushLayer(const Rect& rectBounds) {
		render->PushAxisAlignedClip(__To_D2D_RectF(rectBounds), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		layers.push_back(true);
	}
	void DXRender::PopLayer() {
		if (layers.size() > 0) {
			if (*layers.rbegin() == true) {
				render->PopAxisAlignedClip();
			}
			else {
				render->PopLayer();
			}
			layers.pop_back();
		}
	}
	void DXRender::DrawImage(DXImage* image, const Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Padding& padding) {
		_NOREND_IMAGE_
			if (image == NULL || image->Visible == false) return;
		//计算坐标
		Rect rect = _rect;
		rect.X += padding.Left;
		rect.Y += padding.Top;
		rect.Width -= padding.Right * 2;
		rect.Height -= padding.Bottom * 2;
		//解码
		image->DecodeOfRender(render);
		//转换坐标,缩放
		Size imgSize(image->GetWidth(), image->GetHeight());
		Rect drawRect = EzUI::Transformation(imageSizeMode, rect, imgSize);
		//开始绘制
		if (image->Get() == NULL) return;
		render->DrawBitmap(image->Get(), __To_D2D_RectF(drawRect));
	}
	void DXRender::DrawEllipse(const Point& point, int radiusX, int radiusY, int width)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F((FLOAT)point.X, (FLOAT)point.Y), (FLOAT)radiusX, (FLOAT)radiusY);
		render->DrawEllipse(ellipse, GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillEllipse(const Point& point, int radiusX, int radiusY)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F((FLOAT)point.X, (FLOAT)point.Y), (FLOAT)radiusX, (FLOAT)radiusY);
		render->FillEllipse(ellipse, GetBrush());
	}
	void DXRender::DrawPoint(const Point& pt) {
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(__To_D2D_PointF(pt), 0.5, 0.5);
		render->FillEllipse(ellipse, GetBrush());
	}
	void DXRender::DrawArc(const Rect& rect, int startAngle, int sweepAngle, int width) {

	}
	void DXRender::DrawArc(const Point& point1, const Point& point2, const Point& point3, int width)
	{

	}
	void DXRender::DrawGeometry(ID2D1Geometry* geometry, int width)
	{
		render->DrawGeometry(geometry, GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillGeometry(ID2D1Geometry* geometry, int width)
	{
		render->FillGeometry(geometry, GetBrush());
	}

	void DXRender::DrawPath(const DXPath& path, int width)
	{
		render->DrawGeometry(path.Get(), GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillPath(const DXPath& path)
	{
		render->FillGeometry(path.Get(), GetBrush());
	}
	ID2D1DCRenderTarget* DXRender::Get() {
		return render;
	}
};

#endif