#include "Direct2DRender.h"
#if USED_DIRECT2D
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
	Font::Font(const std::wstring& fontFamily, const float& fontSize) {
		ASSERT(!(fontSize == 0));
		this->fontFamily = fontFamily;
		this->fontSize = fontSize;
		D2D::g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (FLOAT)this->fontSize, L"", &value);
	}
	Font::~Font() {
		if (value && !Ref) {
			SafeRelease(&value);
		}
	}
	const float& Font::GetFontSize()const {
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
	void TextLayout::GetMetrics()
	{
		if (value) {
			value->GetMetrics(&textMetrics);
		}
	}
	//TextLayout
	TextLayout::TextLayout(const std::wstring& text, const Font& font, const SizeF& maxSize, TextAlign textAlign) {
		this->fontSize = font.GetFontSize();
		this->fontFamily = font.GetFontFamily();
		D2D::g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), font.Get(), (FLOAT)maxSize.Width, (FLOAT)maxSize.Height, &value);
		if (value == NULL)return;
		SetTextAlign(textAlign);
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
		if (isTrailingHit) {//�ж�ǰ�໹��β��
			posX += (int)(hitTestMetrics.width + 0.5);
		}
		*fontHeight = (int)(hitTestMetrics.height + 0.5);
		textPos = hitTestMetrics.textPosition;
		return Point{ posX,(int)(hitTestMetrics.top + 0.5) };//���ع�����ڵ�λ��
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
	const std::wstring& TextLayout::GetFontFamily()
	{
		return this->fontFamily;
	}
	Size TextLayout::GetFontBox() {
		this->GetMetrics();
		FLOAT width = textMetrics.widthIncludingTrailingWhitespace;
		FLOAT height = textMetrics.height;
		return  Size{ (int)(width + 1) ,(int)(height + 1) };
	}
	const float& TextLayout::GetFontSize()
	{
		return this->fontSize;
	}
	int TextLayout::Width() {
		this->GetMetrics();
		FLOAT width = textMetrics.widthIncludingTrailingWhitespace;
		return (int)(width + 1);
	}
	int TextLayout::Height() {
		this->GetMetrics();
		FLOAT width = textMetrics.height;
		return (int)(width + 1);
	}
	int TextLayout::GetFontHeight() {
		this->GetMetrics();
		FLOAT height = textMetrics.height;
		return  (int)((height / textMetrics.lineCount) + 0.5);
	}
	int TextLayout::GetLineCount() {
		this->GetMetrics();
		return textMetrics.lineCount;
	}

	Rect TextLayout::GetLineBox(int lineIndex) {
		UINT lineCount = GetLineCount();
		//// ��ȡÿһ�еĿ���
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
		//��ֱ���䷽ʽ
		if (((int)textAlign & (int)VAlign::Top) == (int)VAlign::Top) {
			value->SetParagraphAlignment(__Top);
		}
		if (((int)textAlign & (int)VAlign::Mid) == (int)VAlign::Mid) {
			value->SetParagraphAlignment(__Middle);
		}
		if (((int)textAlign & (int)VAlign::Bottom) == (int)VAlign::Bottom) {
			value->SetParagraphAlignment(__Bottom);
		}
		//ˮƽ���䷽ʽ
		if (((int)textAlign & (int)HAlign::Left) == (int)HAlign::Left) {
			value->SetTextAlignment(__Left);
		}
		if (((int)textAlign & (int)HAlign::Center) == (int)HAlign::Center) {
			value->SetTextAlignment(__Center);
		}
		if (((int)textAlign & (int)HAlign::Right) == (int)HAlign::Right) {
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
	int DXImage::GetWidth() {
		return Width;
	}
	int DXImage::GetHeight() {
		return Height;
	}
	void DXImage::CreateFormStream(IStream* istram) {
		HRESULT imgCreate = S_OK;
		if (D2D::g_ImageFactory) {
			imgCreate = D2D::g_ImageFactory->CreateDecoderFromStream(istram, NULL, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			D2D::g_ImageFactory->CreateFormatConverter(&fmtcovter);
			bitmapdecoder->GetFrame(0, &pframe);
			fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			UINT width, height;
			pframe->GetSize(&width, &height);
			this->Width = width;
			this->Height = height;
			Init();
		}
	}
	void DXImage::CreateFromFile(const std::wstring& filew)
	{
		HRESULT ret = S_OK;
		if (D2D::g_ImageFactory) {
			ret = D2D::g_ImageFactory->CreateDecoderFromFilename(filew.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder);//
		}
		if (bitmapdecoder) {
			ret = D2D::g_ImageFactory->CreateFormatConverter(&fmtcovter);
			ret = bitmapdecoder->GetFrame(0, &pframe);
			ret = fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
			UINT width, height;
			ret = pframe->GetSize(&width, &height);
			this->Width = width;
			this->Height = height;
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
	}
	size_t DXImage::NextFrame() {
		if (_framePos >= _frameCount) {
			_framePos = 0;
		}
		if (pframe) {
			SafeRelease(&pframe);
		}
		if (fmtcovter) {
			SafeRelease(&fmtcovter);
		}
		D2D::g_ImageFactory->CreateFormatConverter(&fmtcovter);
		bitmapdecoder->GetFrame(_framePos, &pframe);
		fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);

		++_framePos;
		return 60;
	}
	DXImage* DXImage::Clone()
	{
		UINT width, height;
		fmtcovter->GetSize(&width, &height); // ��ȡλͼԴ�Ŀ��Ⱥ͸߶�
		//��ʱ����clone����
		return NULL;
	}
	DXImage::DXImage(HBITMAP hBitmap) {
		if (D2D::g_ImageFactory) {
			D2D::g_ImageFactory->CreateBitmapFromHBITMAP(hBitmap, NULL, WICBitmapUsePremultipliedAlpha, &bitMap);
			UINT width, height;
			bitMap->GetSize(&width, &height);
			this->Width = width;
			this->Height = height;
			Init();
		}
	}
	DXImage::DXImage(const std::wstring& file) {
		CreateFromFile(file);
	}
	DXImage::DXImage(int width, int height)
	{
		this->Width = width;
		this->Height = height;
		ASSERT(!(width <= 0 || height <= 0));
		HRESULT hr = D2D::g_ImageFactory->CreateBitmap((UINT)width, (UINT)height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &bitMap);
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
		float radius = GetMaxRadius(width, height, _radius);
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
		// ��·������ͼ�εļ��νӿ�
		ID2D1GeometrySink* pSink;
		pPathGeometry->Open(&pSink);
		// ��ʼ����·��
		pSink->BeginFigure(D2D1::Point2F(rect.left, rect.top + topLeftRadius), D2D1_FIGURE_BEGIN_FILLED);
		// ���ӻ��߶ε����Ͻ�
		pSink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(rect.left + topLeftRadius, rect.top),
				D2D1::SizeF(topLeftRadius, topLeftRadius),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
		// ���ӻ��߶ε����Ͻ�
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
		// ���ӻ��߶ε����½�
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
		// ���ӻ��߶ε����½�
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
		// ����·��
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		pSink->Close();
		SafeRelease(&pSink);
		this->rgn = pPathGeometry;
	}
};

namespace EzUI {
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
				//��ʼ��һ��DX�õ�һ���������ڿ�һ��
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
			_GUID imageFactoryOld{ 0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa };//xp  win7 �ɰ�
			_GUID WICImagingFactoryId = CLSID_WICImagingFactory;//��ǰƽ̨
		ImagingFactoryInit:
			hr = CoCreateInstance(WICImagingFactoryId, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&D2D::g_ImageFactory);
			if (hr != S_OK) {
				//if (hr == 0x800401F0) {//δ��ʼ��com �Լ���ȫ�ֳ�ʼ��һ�¾ͺ��� (�Լ����Ƴ�ʼ��ʱ��)
				//	::CoInitialize(NULL);
				//	goto ImagingFactory;
				//}
				if (hr == 0x80040154) {//û��ע���� ����win7��sdk���ɵĳ�������win7ϵͳ�����л���ִ˴���
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
		float radius = (float)_radius;//�뾶
		float diameter = radius * 2;//ֱ��
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
				D2D1_ALPHA_MODE_PREMULTIPLIED),//����ڷֲ㴰�е���괩͸����
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
			D2D1::PixelFormat(),
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
	void DXRender::SetFont(const std::wstring& fontFamily, const float& fontSize) {
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
	void DXRender::SetColor(const __EzUI__Color& color) {
		if (brush == NULL) {
			render->CreateSolidColorBrush(__To_D2D_COLOR_F(color), &brush);
		}
		else {
			brush->SetColor(__To_D2D_COLOR_F(color));
		}
	}
	void DXRender::SetStrokeStyle(StrokeStyle strokeStyle, float dashWidth)
	{
		if (pStrokeStyle != NULL) {
			SafeRelease(&pStrokeStyle);
		}
		if (strokeStyle == StrokeStyle::Dash) {
			float* dashes = new float[2] { (FLOAT)dashWidth, (FLOAT)dashWidth };
			const int count = 2;
			D2D::g_Direct2dFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
				D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_FLAT, D2D1_CAP_STYLE_ROUND,
				D2D1_LINE_JOIN_ROUND, 10.0f, D2D1_DASH_STYLE_CUSTOM, 0.0f),
				dashes, count, &pStrokeStyle);
			delete[] dashes;
		}
	}
	void DXRender::DrawTextLayout(const TextLayout& textLayout, const PointF& startLacation) {
		render->DrawTextLayout(D2D1_POINT_2F{ (float)(startLacation.X) ,(float)(startLacation.Y) }, textLayout.Get(), GetBrush());
	}
	void DXRender::DrawString(const std::wstring& text, const  RectF& _rect, EzUI::TextAlign textAlign) {
		const auto& rect = _rect;
		TextLayout textLayout(text, *font, { rect.Width, rect.Height }, textAlign);
		this->DrawTextLayout(textLayout, { _rect.X,_rect.Y });
	}
	void DXRender::DrawLine(const PointF& _A, const PointF& _B, float width) {
		const auto& A = _A;
		const auto& B = _B;
		render->DrawLine(D2D1_POINT_2F{ (float)A.X,(float)A.Y }, D2D1_POINT_2F{ (float)B.X,(float)B.Y }, GetBrush(), (FLOAT)width, GetStrokeStyle());
	}

	void DXRender::DrawRectangle(const RectF& _rect, float _radius, float width)
	{
		const auto& rect = _rect;
		if (_radius > 0) {
			float radius = GetMaxRadius(_rect.Width, _rect.Height, (float)_radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), (float)radius, (float)radius };
			render->DrawRoundedRectangle(roundRect, GetBrush(), (FLOAT)width, GetStrokeStyle());
		}
		else {
			render->DrawRectangle(__To_D2D_RectF(rect), GetBrush(), (FLOAT)width, GetStrokeStyle());
		}
	}


	void DXRender::FillRectangle(const RectF& _rect, float _radius) {
		const auto& rect = _rect;
		if (_radius > 0) {
			float radius = GetMaxRadius(_rect.Width, _rect.Height, (float)_radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), (float)radius, (float)radius };
			render->FillRoundedRectangle(roundRect, GetBrush());
		}
		else {
			render->FillRectangle(__To_D2D_RectF(rect), GetBrush());
		}
	}

	void DXRender::SetTransform(float offsetX, float offsetY)
	{
		this->Offset.X = offsetX;
		this->Offset.Y = offsetY;
		this->SetTransform(this->Offset.X, this->Offset.Y, this->RotatePoint.X, this->RotatePoint.Y, this->Angle);
	}

	void DXRender::SetTransform(float startX, float startY, float angle) {
		this->RotatePoint.X = startX;
		this->RotatePoint.Y = startY;
		this->Angle = angle;
		this->SetTransform(this->Offset.X, this->Offset.Y, this->RotatePoint.X, this->RotatePoint.Y, this->Angle);
	}

	void DXRender::SetTransform(float offsetX, float offsetY, float startX, float startY, float angle)
	{
		// ����D2D����ת������ ƽ�Ʊ任
		D2D1::Matrix3x2F transformMatrix = D2D1::Matrix3x2F::Translation(offsetX, offsetY);
		// �ж��Ƿ���Ҫ������ת�任
		if (angle != 0)
		{
			// ��ת�任
			transformMatrix = transformMatrix * D2D1::Matrix3x2F::Rotation(angle, D2D1_POINT_2F{ startX, startY });
		}
		// ��ת������Ӧ���ڻ��ƶ���
		render->SetTransform(transformMatrix);
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
		render->DrawGeometry(pathGeometry, GetBrush(), (FLOAT)width, GetStrokeStyle());
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
		render->DrawGeometry(pathGeometry, GetBrush(), width, GetStrokeStyle());
		SafeRelease(&pathGeometry);
		SafeRelease(&pSink);
	}

	void DXRender::PushLayer(const Geometry& dxGeometry) {
		ID2D1Layer* layer = NULL;
		render->CreateLayer(&layer);
		render->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), dxGeometry.Get()), layer);//����layer
		SafeRelease(&layer);
	}
	void DXRender::PopLayer() {
		render->PopLayer();
	}
	void DXRender::PushAxisAlignedClip(const RectF& rectBounds) {
		render->PushAxisAlignedClip(__To_D2D_RectF(rectBounds), D2D1_ANTIALIAS_MODE::D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	}
	void DXRender::PopAxisAlignedClip() {
		render->PopAxisAlignedClip();
	}

	void DXRender::DrawImage(DXImage* image, const  RectF& tagRect, float opacity) {
		_NOREND_IMAGE_
			if (image->Visible == false) return;
		//����
		image->DecodeOfRender(render);
		ID2D1Bitmap* bitmap = image->Get();
		if (!bitmap) {
			return;
		}
		const ImageSizeMode& imageSizeMode = image->SizeMode;
		const Rect& sourceRect = image->Offset;
		//��������
		Rect rect(tagRect.X + 0.5, tagRect.Y + 0.5, tagRect.Width + 0.5, tagRect.Height + 0.5);
		//ת������,����
		Size imgSize(image->GetWidth(), image->GetHeight());
		if (!sourceRect.IsEmptyArea()) {
			imgSize.Width = sourceRect.Width;
			imgSize.Height = sourceRect.Height;
		}
		Rect drawRect = EzUI::Transformation(imageSizeMode, rect, imgSize);
		//��ʼ����
		D2D_RECT_F drawRectF = __To_D2D_RectF(drawRect);
		if (!sourceRect.IsEmptyArea()) {
			D2D_RECT_F sourceRectF = __To_D2D_RectF(sourceRect);
			render->DrawBitmap(bitmap, drawRectF, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, sourceRectF);
		}
		else {
			render->DrawBitmap(bitmap, drawRectF, opacity);
		}
	}
	void DXRender::DrawEllipse(const PointF& point, float radiusX, float radiusY, float width)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F((FLOAT)point.X, (FLOAT)point.Y), (FLOAT)radiusX, (FLOAT)radiusY);
		render->DrawEllipse(ellipse, GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillEllipse(const PointF& point, float radiusX, float radiusY)
	{
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F((FLOAT)point.X, (FLOAT)point.Y), (FLOAT)radiusX, (FLOAT)radiusY);
		render->FillEllipse(ellipse, GetBrush());
	}
	void DXRender::DrawPoint(const PointF& pt) {
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(__To_D2D_PointF(pt), 0.5, 0.5);
		render->FillEllipse(ellipse, GetBrush());
	}
	void DXRender::DrawArc(const RectF& rect, float startAngle, float sweepAngle, float width) {

	}
	void DXRender::DrawArc(const PointF& point1, const PointF& point2, const PointF& point3, float width)
	{

	}
	void DXRender::DrawGeometry(ID2D1Geometry* geometry, float width)
	{
		render->DrawGeometry(geometry, GetBrush(), (FLOAT)width, this->GetStrokeStyle());
	}
	void DXRender::FillGeometry(ID2D1Geometry* geometry)
	{
		render->FillGeometry(geometry, GetBrush());
	}
	void DXRender::Flush()
	{
		render->Flush();
	}
	void DXRender::DrawPath(const DXPath& path, float width)
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