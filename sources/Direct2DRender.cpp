#include "Direct2DRender.h"
#include <Shlwapi.h>
#if USED_DIRECT2D
#define _NOREND_IMAGE_ 
//#define _NOREND_IMAGE_ return;
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Windowscodecs.lib")
namespace ezui {
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
	Font::Font(const std::wstring& fontFamily, float fontSize) {
		ASSERT(!(fontSize == 0));
		this->m_fontFamily = fontFamily;
		this->m_fontSize = fontSize;
		D2D::g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (FLOAT)this->m_fontSize, L"", &m_value);
	}
	Font::~Font() {
		if (m_value && !m_ref) {
			SafeRelease(&m_value);
		}
	}
	float Font::GetFontSize()const {
		return m_fontSize;
	}
	const std::wstring& Font::GetFontFamily()const {
		return m_fontFamily;
	}
	void Font::Copy(const Font& _copy) {
		((Font&)(_copy)).m_ref = true;
		this->m_value = _copy.Get();
		this->m_fontFamily = _copy.GetFontFamily();
		this->m_fontSize = _copy.GetFontSize();
	}
	IDWriteTextFormat* Font::Get() const {
		return m_value;
	}
	Font::Font(const Font& rightValue) {
		Copy(rightValue);
	}
	Font& Font::operator=(const Font& rightValue) {
		Copy(rightValue);
		return *this;
	}
	bool Font::operator==(const Font& _right) {
		if (_right.GetFontFamily() == this->GetFontFamily() && _right.GetFontSize() == this->GetFontSize()) {
			return true;
		}
		return false;
	}
	void TextLayout::GetMetrics()
	{
		if (value) {
			value->GetMetrics(&m_textMetrics);
		}
	}
	//TextLayout
	TextLayout::TextLayout(const std::wstring& text, const Font& font, const SizeF& maxSize, TextAlign textAlign) {
		this->m_fontSize = font.GetFontSize();
		this->m_fontFamily = font.GetFontFamily();
		D2D::g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), font.Get(), (FLOAT)maxSize.Width, (FLOAT)maxSize.Height, &value);
		if (value == NULL)return;
		SetTextAlign(textAlign);
	}
	IDWriteTextLayout* TextLayout::Get() const {
		return value;
	}
	Point TextLayout::HitTestPoint(const Point& pt, int_t* _textPos, BOOL* _isTrailingHit, int_t* fontHeight) {
		int_t& textPos = *_textPos;
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
		int_t posX = (int_t)(hitTestMetrics.left + 0.5);
		if (isTrailingHit) {//判断前侧还是尾侧
			posX += (int_t)(hitTestMetrics.width + 0.5);
		}
		*fontHeight = (int_t)(hitTestMetrics.height + 0.5);
		textPos = hitTestMetrics.textPosition;
		return Point{ posX,(int_t)(hitTestMetrics.top + 0.5) };//返回光标所在的位置
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

	Point TextLayout::HitTestTextPosition(int_t textPos, BOOL isTrailingHit) {
		if (value == NULL)return Point{ 0,0 };
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		FLOAT X, Y;
		value->HitTestTextPosition(textPos, isTrailingHit, &X, &Y, &hitTestMetrics);
		return Point((int_t)(X + 0.5), (int_t)(Y + 0.5));
	}
	const std::wstring& TextLayout::GetFontFamily()
	{
		return this->m_fontFamily;
	}
	Size TextLayout::GetFontBox() {
		this->GetMetrics();
		FLOAT width = m_textMetrics.widthIncludingTrailingWhitespace;
		FLOAT height = m_textMetrics.height;
		return  Size{ (int_t)(width + 1) ,(int_t)(height + 1) };
	}
	float TextLayout::GetFontSize()
	{
		return this->m_fontSize;
	}
	int_t TextLayout::Width() {
		this->GetMetrics();
		FLOAT width = m_textMetrics.widthIncludingTrailingWhitespace;
		return (int_t)(width + 1);
	}
	int_t TextLayout::Height() {
		this->GetMetrics();
		FLOAT width = m_textMetrics.height;
		return (width + 1);
	}
	int_t TextLayout::GetFontHeight() {
		this->GetMetrics();
		FLOAT height = m_textMetrics.height;
		return  ((height / m_textMetrics.lineCount) + 0.5);
	}
	int_t TextLayout::GetLineCount() {
		this->GetMetrics();
		return m_textMetrics.lineCount;
	}

	Rect TextLayout::GetLineBox(int_t lineIndex) {
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
		if (((int_t)textAlign & (int_t)VAlign::Top) == (int_t)VAlign::Top) {
			value->SetParagraphAlignment(__Top);
		}
		if (((int_t)textAlign & (int_t)VAlign::Mid) == (int_t)VAlign::Mid) {
			value->SetParagraphAlignment(__Middle);
		}
		if (((int_t)textAlign & (int_t)VAlign::Bottom) == (int_t)VAlign::Bottom) {
			value->SetParagraphAlignment(__Bottom);
		}
		//水平对其方式
		if (((int_t)textAlign & (int_t)HAlign::Left) == (int_t)HAlign::Left) {
			value->SetTextAlignment(__Left);
		}
		if (((int_t)textAlign & (int_t)HAlign::Center) == (int_t)HAlign::Center) {
			value->SetTextAlignment(__Center);
		}
		if (((int_t)textAlign & (int_t)HAlign::Right) == (int_t)HAlign::Right) {
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
			value->SetUnderline(TRUE, { (UINT32)pos,(UINT32)count });
		}
	}
	TextLayout::~TextLayout() {
		if (value) {
			SafeRelease(&value);
		}
	}
	//DXImage
	void DXImage::DecodeOfRender(ID2D1RenderTarget* render) {
		HRESULT hr = 0;
		if (m_d2dBitmap != NULL) {
			if (m_d2dBitmap) {
				SafeRelease(&m_d2dBitmap);
			}
		}
		if (m_fmtcovter) {
			hr = render->CreateBitmapFromWicBitmap(m_fmtcovter, 0, &m_d2dBitmap);
		}
		if (m_bitMap) {
			hr = render->CreateBitmapFromWicBitmap(m_bitMap, &m_d2dBitmap);
		}
	}
	int_t DXImage::Width() {
		return m_width;
	}
	int_t DXImage::Height() {
		return m_height;
	}
	void DXImage::CreateFormStream(IStream* istram) {
		HRESULT imgCreate = S_OK;
		if (D2D::g_ImageFactory) {
			imgCreate = D2D::g_ImageFactory->CreateDecoderFromStream(istram, NULL, WICDecodeMetadataCacheOnDemand, &m_bitmapdecoder);//
		}
		if (m_bitmapdecoder) {
			D2D::g_ImageFactory->CreateFormatConverter(&m_fmtcovter);
			m_bitmapdecoder->GetFrame(0, &m_pframe);
			m_fmtcovter->Initialize(m_pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			UINT width, height;
			m_pframe->GetSize(&width, &height);
			this->m_width = width;
			this->m_height = height;
			Init();
		}
	}
	void DXImage::CreateFromFile(const std::wstring& filew)
	{
		HRESULT ret = S_OK;
		if (D2D::g_ImageFactory) {
			ret = D2D::g_ImageFactory->CreateDecoderFromFilename(filew.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &m_bitmapdecoder);//
		}
		if (m_bitmapdecoder) {
			ret = D2D::g_ImageFactory->CreateFormatConverter(&m_fmtcovter);
			ret = m_bitmapdecoder->GetFrame(0, &m_pframe);
			ret = m_fmtcovter->Initialize(m_pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			UINT width, height;
			ret = m_pframe->GetSize(&width, &height);
			this->m_width = width;
			this->m_height = height;
			Init();
		}
	}
	void DXImage::Init() {
		_framePos = 0;
		UINT fCount = 0;
		if (m_bitmapdecoder) {
			m_bitmapdecoder->GetFrameCount(&fCount);
			_frameCount = fCount;
		}
	}
	WORD DXImage::NextFrame() {
		if (_framePos >= _frameCount) {
			_framePos = 0;
		}
		if (m_pframe) {
			SafeRelease(&m_pframe);
		}
		if (m_fmtcovter) {
			SafeRelease(&m_fmtcovter);
		}
		D2D::g_ImageFactory->CreateFormatConverter(&m_fmtcovter);
		m_bitmapdecoder->GetFrame(_framePos, &m_pframe);
		m_fmtcovter->Initialize(m_pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);

		++_framePos;
		return 60;
	}
	DXImage* DXImage::Clone()
	{
		UINT width, height;
		m_fmtcovter->GetSize(&width, &height); // 获取位图源的宽度和高度
		//暂时不做clone函数
		return NULL;
	}
	DXImage::DXImage(HBITMAP hBitmap) {
		if (D2D::g_ImageFactory) {
			D2D::g_ImageFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUsePremultipliedAlpha, &m_bitMap);
			UINT width, height;
			m_bitMap->GetSize(&width, &height);
			this->m_width = width;
			this->m_height = height;
			Init();
		}
	}
	DXImage::DXImage(const std::wstring& file) {
		CreateFromFile(file);
	}
	DXImage::DXImage(int_t width, int_t height)
	{
		this->m_width = width;
		this->m_height = height;
		ASSERT(!(width <= 0 || height <= 0));
		HRESULT hr = D2D::g_ImageFactory->CreateBitmap((UINT)width, (UINT)height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &m_bitMap);
	}
	DXImage::DXImage(const void* data, size_t imgSize)
	{
		ASSERT(data);
		ASSERT(imgSize);
		IStream* stream = SHCreateMemStream((BYTE*)data, imgSize);
		if (stream) {
			this->CreateFormStream(stream);
			SafeRelease(&stream);
		}
	}
	ID2D1Bitmap* DXImage::Get()
	{
		return m_d2dBitmap;
	}
	IWICBitmap* DXImage::GetIWICBitmap()
	{
		return m_bitMap;
	}
	DXImage::DXImage(IStream* istram) {
		CreateFormStream(istram);
	}
	DXImage::~DXImage()
	{
		if (m_d2dBitmap) {
			SafeRelease(&m_d2dBitmap);
		}
		if (m_bitmapdecoder) {
			SafeRelease(&m_bitmapdecoder);
		}
		if (m_pframe) {
			SafeRelease(&m_pframe);
		}
		if (m_fmtcovter) {
			SafeRelease(&m_fmtcovter);
		}
		if (m_bitMap) {
			SafeRelease(&m_bitMap);
		}
	}
	Geometry::Geometry(float x, float y, float width, float height) {
		D2D_RECT_F rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) };
		D2D::g_Direct2dFactory->CreateRectangleGeometry(rectF, (ID2D1RectangleGeometry**)&m_rgn);
	}
	Geometry::Geometry(float x, float y, float width, float height, float _radius) {
		float radius = GetMaxRadius(width, height, _radius);
		D2D1_ROUNDED_RECT rectF{ (FLOAT)x,(FLOAT)y,(FLOAT)(x + width),(FLOAT)(y + height) ,radius ,radius };
		D2D::g_Direct2dFactory->CreateRoundedRectangleGeometry(rectF, (ID2D1RoundedRectangleGeometry**)&m_rgn);
	}
	Geometry::Geometry(const RectF& _rect, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius)
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
		SafeRelease(&pSink);
		this->m_rgn = pPathGeometry;
	}
};

namespace ezui {
	void RenderInitialize()
	{
		HRESULT hr = S_OK;
		// Create a Direct2D factory.
		if (D2D::g_Direct2dFactory == NULL) {
			hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &D2D::g_Direct2dFactory);
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
	float GetMaxRadius(float width, float height, float _radius)
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
	DXRender::DXRender(HDC dc, int_t x, int_t y, int_t width, int_t height) {
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
		HRESULT	hr = D2D::g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&m_render);
		RECT rc{ x,y,x + width ,y + height };
		((ID2D1DCRenderTarget*)m_render)->BindDC(dc, &rc);
		m_render->BeginDraw();
	}

	DXRender::DXRender(DXImage* dxImage) {
		D2D1_RENDER_TARGET_PROPERTIES defaultOption = D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(),
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		);
		HRESULT hr = D2D::g_Direct2dFactory->CreateWicBitmapRenderTarget(dxImage->GetIWICBitmap(), defaultOption, (ID2D1RenderTarget**)&m_render);
		m_render->BeginDraw();
	}


	DXRender::~DXRender() {
		if (m_render) {
			m_render->EndDraw();
		}
		if (m_font) {
			delete m_font;
		}
		SafeRelease(&m_render);
		SafeRelease(&m_brush);
		SafeRelease(&m_pStrokeStyle);
	}
	ID2D1SolidColorBrush* DXRender::GetBrush()
	{
		if (m_brush == NULL) {
			m_render->CreateSolidColorBrush(D2D_COLOR_F{ 0,0,0,1 }, &m_brush);
		}
		return m_brush;
	}
	ID2D1StrokeStyle* DXRender::GetStrokeStyle() {
		return m_pStrokeStyle;
	}
	void DXRender::SetFont(const std::wstring& fontFamily, float fontSize) {
		if (m_font != NULL) {
			if (m_font->GetFontFamily() == fontFamily && m_font->GetFontSize() == fontSize) {
				return;
			}
			delete m_font;
		}
		m_font = new Font(fontFamily, fontSize);
	}
	void DXRender::SetFont(const Font& _copy_font) {
		if (m_font != NULL) {
			if (*m_font == _copy_font) {
				return;
			}
			delete m_font;
		}
		m_font = new Font(_copy_font);
	}
	void DXRender::SetColor(const __EzUI__Color& color) {
		if (m_brush == NULL) {
			m_render->CreateSolidColorBrush(__To_D2D_COLOR_F(color), &m_brush);
		}
		else {
			m_brush->SetColor(__To_D2D_COLOR_F(color));
		}
	}
	void DXRender::SetStrokeStyle(StrokeStyle strokeStyle, float dashWidth)
	{
		if (m_pStrokeStyle != NULL) {
			SafeRelease(&m_pStrokeStyle);
		}
		if (strokeStyle == StrokeStyle::Dash) {
			float* dashes = new float[2] { (FLOAT)dashWidth, (FLOAT)dashWidth };
			const int_t count = 2;
			D2D::g_Direct2dFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_CUSTOM, 0.0f),
				dashes, count, &m_pStrokeStyle);
			delete[] dashes;
		}
	}
	void DXRender::DrawTextLayout(const TextLayout& textLayout, const PointF& startLacation) {
		m_render->DrawTextLayout(D2D1_POINT_2F{ (float)(startLacation.X) ,(float)(startLacation.Y) }, textLayout.Get(), GetBrush());
	}
	void DXRender::DrawString(const std::wstring& text, const  RectF& _rect, ezui::TextAlign textAlign) {
		const auto& rect = _rect;
		TextLayout textLayout(text, *m_font, { rect.Width, rect.Height }, textAlign);
		this->DrawTextLayout(textLayout, { _rect.X,_rect.Y });
	}
	void DXRender::DrawLine(const PointF& _A, const PointF& _B, float width) {
		const auto& A = _A;
		const auto& B = _B;
		m_render->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, GetBrush(), (FLOAT)width, GetStrokeStyle());
	}

	void DXRender::DrawRectangle(const RectF& _rect, float _radius, float width)
	{
		const auto& rect = _rect;
		if (_radius > 0) {
			float radius = GetMaxRadius(_rect.Width, _rect.Height, (float)_radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), (float)radius, (float)radius };
			m_render->DrawRoundedRectangle(roundRect, GetBrush(), (FLOAT)width, GetStrokeStyle());
		}
		else {
			m_render->DrawRectangle(__To_D2D_RectF(rect), GetBrush(), (FLOAT)width, GetStrokeStyle());
		}
	}


	void DXRender::FillRectangle(const RectF& _rect, float _radius) {
		const auto& rect = _rect;
		if (_radius > 0) {
			float radius = GetMaxRadius(_rect.Width, _rect.Height, (float)_radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), (float)radius, (float)radius };
			m_render->FillRoundedRectangle(roundRect, GetBrush());
		}
		else {
			m_render->FillRectangle(__To_D2D_RectF(rect), GetBrush());
		}
	}

	void DXRender::SetTransform(float offsetX, float offsetY)
	{
		this->m_offset.X = offsetX;
		this->m_offset.Y = offsetY;
		this->SetTransform(this->m_offset.X, this->m_offset.Y, this->m_rotatePoint.X, this->m_rotatePoint.Y, this->m_angle);
	}

	void DXRender::SetTransform(float startX, float startY, float angle) {
		this->m_rotatePoint.X = startX;
		this->m_rotatePoint.Y = startY;
		this->m_angle = angle;
		this->SetTransform(this->m_offset.X, this->m_offset.Y, this->m_rotatePoint.X, this->m_rotatePoint.Y, this->m_angle);
	}

	void DXRender::SetTransform(float offsetX, float offsetY, float startX, float startY, float angle)
	{
		// 创建D2D矩阵转换对象 平移变换
		D2D1::Matrix3x2F transformMatrix = D2D1::Matrix3x2F::Translation(offsetX, offsetY);
		// 判断是否需要进行旋转变换
		if (angle != 0)
		{
			// 旋转变换
			transformMatrix = transformMatrix * D2D1::Matrix3x2F::Rotation(angle, D2D1_POINT_2F{ startX, startY });
		}
		// 将转换矩阵应用于绘制对象
		m_render->SetTransform(transformMatrix);
	}

	void DXRender::DrawBezier(const PointF& startPoint, const Bezier& points, float width) {
		ID2D1GeometrySink* pSink = NULL;
		ID2D1PathGeometry* pathGeometry = NULL;
		D2D::g_Direct2dFactory->CreatePathGeometry(&pathGeometry);
		pathGeometry->Open(&pSink);
		pSink->BeginFigure(__To_D2D_PointF(startPoint), D2D1_FIGURE_BEGIN_FILLED);
		D2D1_BEZIER_SEGMENT bzr{ __To_D2D_PointF(points.point1) ,__To_D2D_PointF(points.point2) ,__To_D2D_PointF(points.point3) };
		pSink->AddBezier(bzr);
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		pSink->Close();
		m_render->DrawGeometry(pathGeometry, GetBrush(), (FLOAT)width, GetStrokeStyle());
		SafeRelease(&pathGeometry);
		SafeRelease(&pSink);
	}
	void DXRender::DrawBezier(const PointF& startPoint, std::list<Bezier>& beziers, float width)
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
		m_render->DrawGeometry(pathGeometry, GetBrush(), width, GetStrokeStyle());
		SafeRelease(&pathGeometry);
		SafeRelease(&pSink);
	}

	void DXRender::PushLayer(const Geometry& dxGeometry) {
		ID2D1Layer* layer = NULL;
		m_render->CreateLayer(&layer);
		m_render->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), dxGeometry.Get()), layer);//放入layer
		SafeRelease(&layer);
	}
	void DXRender::PopLayer() {
		m_render->PopLayer();
	}
	void DXRender::PushAxisAlignedClip(const RectF& rectBounds) {
		m_render->PushAxisAlignedClip(__To_D2D_RectF(rectBounds), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
	void DXRender::PopAxisAlignedClip() {
		m_render->PopAxisAlignedClip();
	}

	void DXRender::DrawImage(DXImage* image, const  RectF& tagRect, float opacity) {
		_NOREND_IMAGE_
			if (image->Visible == false) return;
		//解码
		image->DecodeOfRender(m_render);
		ID2D1Bitmap* bitmap = image->Get();
		if (!bitmap) {
			//解码失败
			return;
		}
		ImageSizeMode imageSizeMode = image->SizeMode;
		const Rect& sourceRect = image->Clip;
		//计算坐标
		Rect rect(tagRect.X + 0.5, tagRect.Y + 0.5, tagRect.Width + 0.5, tagRect.Height + 0.5);
		//转换坐标,缩放
		Size imgSize(image->Width(), image->Height());
		if (!sourceRect.IsEmptyArea()) {
			imgSize.Width = sourceRect.Width;
			imgSize.Height = sourceRect.Height;
		}

		//加上在Owner区域的偏移
		rect.X += image->DrawPosition.X;
		rect.Y += image->DrawPosition.Y;

		Rect drawRect;
		if (!image->DrawSize.Empty()) {
			//限制在Owner中的绘制区域
			drawRect = Rect(rect.X, rect.Y, image->DrawSize.Width, image->DrawSize.Height);
		}
		else {
			//不限制绘制区域 根据imageSizeMode属性进行坐标转换
			drawRect = ezui::Transformation(imageSizeMode, rect, imgSize);
		}
		//开始绘制
		D2D_RECT_F drawRectF = __To_D2D_RectF(drawRect);
		if (!sourceRect.IsEmptyArea()) {
			//裁剪图片部分内容进行绘制
			D2D_RECT_F imageClipRectF = __To_D2D_RectF(sourceRect);
			m_render->DrawBitmap(bitmap, drawRectF, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, imageClipRectF);
		}
		else {
			//不裁剪图片的方式进行绘制
			m_render->DrawBitmap(bitmap, drawRectF, opacity);
		}
	}
	void DXRender::DrawEllipse(const PointF& point, float radiusX, float radiusY, float width)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F((FLOAT)point.X, (FLOAT)point.Y), (FLOAT)radiusX, (FLOAT)radiusY);
		m_render->DrawEllipse(ellipse, GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillEllipse(const PointF& point, float radiusX, float radiusY)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F((FLOAT)point.X, (FLOAT)point.Y), (FLOAT)radiusX, (FLOAT)radiusY);
		m_render->FillEllipse(ellipse, GetBrush());
	}
	void DXRender::DrawPoint(const PointF& pt) {
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(__To_D2D_PointF(pt), 0.5, 0.5);
		m_render->FillEllipse(ellipse, GetBrush());
	}
	void DXRender::DrawArc(const RectF& rect, float startAngle, float sweepAngle, float width) {

	}
	void DXRender::DrawArc(const PointF& point1, const PointF& point2, const PointF& point3, float width)
	{

	}
	void DXRender::DrawGeometry(ID2D1Geometry* geometry, float width)
	{
		m_render->DrawGeometry(geometry, GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillGeometry(ID2D1Geometry* geometry)
	{
		m_render->FillGeometry(geometry, GetBrush());
	}
	void DXRender::Flush()
	{
		m_render->Flush();
	}
	void DXRender::DrawPath(const DXPath& path, float width)
	{
		m_render->DrawGeometry(path.Get(), GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillPath(const DXPath& path)
	{
		m_render->FillGeometry(path.Get(), GetBrush());
	}
	ID2D1DCRenderTarget* DXRender::Get() {
		return m_render;
	}
};

#endif