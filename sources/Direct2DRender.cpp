#include "Direct2DRender.h"
#include <Shlwapi.h>

#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")

#if USED_DIRECT2D
#define _NOREND_IMAGE_ 
//#define _NOREND_IMAGE_ return;
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"Windowscodecs.lib")
#undef min
#undef max
namespace ezui {
	ULONG_PTR g_GdiplusToken = NULL;
	namespace D2D {
		ID2D1Factory* g_Direct2dFactory = NULL;
		IDWriteFactory* g_WriteFactory = NULL;
		IWICImagingFactory* g_ImageFactory = NULL;
	}
#define __COLOR_SCALE 0.003921569f
#define __To_D2D_COLOR_F(color) D2D_COLOR_F{FLOAT(color.GetR() * __COLOR_SCALE), FLOAT(color.GetG() * __COLOR_SCALE), FLOAT(color.GetB() * __COLOR_SCALE),FLOAT(color.GetA() * __COLOR_SCALE)}
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
		D2D::g_WriteFactory->CreateTextFormat(fontFamily.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_fontSize, L"", &m_value);
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
	bool Font::operator==(const Font& _right) {
		if (_right.GetFontFamily() == this->GetFontFamily() && (std::fabs(_right.GetFontSize() - this->GetFontSize()) < EZUI_FLOAT_EPSILON)) {
			return true;
		}
		return false;
	}
	void TextLayout::GetMetrics()
	{
		if (m_textLayout) {
			m_textLayout->GetMetrics(&m_textMetrics);
		}
	}
	//TextLayout
	TextLayout::TextLayout(const std::wstring& text, const Font& font, const SizeF& maxSize, TextAlign textAlign) {
		this->m_unicodeSize = text.size();
		this->m_fontSize = font.GetFontSize();
		this->m_fontFamily = font.GetFontFamily();
		D2D::g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), font.Get(), maxSize.Width, maxSize.Height, &m_textLayout);
		if (m_textLayout == NULL)return;
		SetTextAlign(textAlign);
	}
	IDWriteTextLayout* TextLayout::Get() const {
		return m_textLayout;
	}
	Point TextLayout::HitTestPoint(const Point& pt, int* _textPos, BOOL* _isTrailingHit, int* fontHeight) {
		int& textPos = *_textPos;
		BOOL& isTrailingHit = *_isTrailingHit;
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		BOOL isInside;
		{
			FLOAT x = (FLOAT)pt.X, y = (FLOAT)pt.Y;
			m_textLayout->HitTestPoint(
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
			m_textLayout->HitTestPoint(
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
		if (m_textLayout == NULL)return Point{ 0,0 };
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		FLOAT X, Y;
		m_textLayout->HitTestTextPosition(textPos, isTrailingHit, &X, &Y, &hitTestMetrics);
		return Point((int)(X + 0.5), (int)(Y + 0.5));
	}
	const std::wstring& TextLayout::GetFontFamily()
	{
		return this->m_fontFamily;
	}
	Size TextLayout::GetFontBox() {
		this->GetMetrics();
		FLOAT width = m_textMetrics.widthIncludingTrailingWhitespace;
		FLOAT height = m_textMetrics.height;
		return  Size{ (int)(width + 1) ,(int)(height + 1) };//加1是为了向上取整
	}
	float TextLayout::GetFontSize()
	{
		return this->m_fontSize;
	}
	int TextLayout::Width() {
		this->GetMetrics();
		FLOAT width = m_textMetrics.widthIncludingTrailingWhitespace;
		return (int)(width + 1);
	}
	int TextLayout::Height() {
		this->GetMetrics();
		FLOAT width = m_textMetrics.height;
		return (width + 1);
	}
	int TextLayout::GetFontHeight() {
		this->GetMetrics();
		FLOAT height = m_textMetrics.height;
		return  ((height / m_textMetrics.lineCount) + 0.5);
	}
	int TextLayout::GetLineCount() {
		this->GetMetrics();
		return m_textMetrics.lineCount;
	}

#undef min
#undef max
	const std::vector<RectF>& TextLayout::GetLineRects() {
		//如果unicode个数为0 且获取过m_lineRects 则直接返回不执行获取操作
		if (!m_textLayout || m_unicodeSize == 0 || !m_lineRects.empty()) {
			return m_lineRects;
		}
		UINT lineCount = GetLineCount();
		//获取每行 metrics
		std::vector<DWRITE_LINE_METRICS> lineMetrics(lineCount);
		auto hr = m_textLayout->GetLineMetrics(lineMetrics.data(), lineCount, &lineCount);
		if (FAILED(hr)) {
			return m_lineRects;
		}
		//遍历每一行，计算对应矩形
		UINT32 textPos = 0;
		for (UINT32 i = 0; i < lineCount; i++) {
			auto& lm = lineMetrics[i];
			if (lm.length == 0) { // 空行
				textPos += lm.length;
				continue;
			}
			//给一个足够大的 buffer，存放 hit test 的矩形
			std::vector<DWRITE_HIT_TEST_METRICS> metrics(lm.length);
			UINT32 actualCount = 0;

			hr = m_textLayout->HitTestTextRange(
				textPos, lm.length, // 这一行的字符范围
				0.0f, 0.0f,         // offsetX, offsetY
				metrics.data(), (UINT32)metrics.size(),
				&actualCount
			);

			if (SUCCEEDED(hr) && actualCount > 0) {
				float minX = metrics[0].left;
				float maxX = metrics[0].left + metrics[0].width;
				float top = metrics[0].top;
				float bottom = top + metrics[0].height;

				for (UINT32 j = 1; j < actualCount; j++) {
					auto& m = metrics[j];
					minX = std::min(minX, m.left);
					maxX = std::max(maxX, m.left + m.width);
					top = std::min(top, m.top);
					bottom = std::max(bottom, m.top + m.height);
				}
				m_lineRects.push_back(RectF(minX, top, maxX - minX, bottom - top));
			}
			textPos += lm.length; //下一行的起始字符
		}
		return m_lineRects;
	}

	void TextLayout::SetTextAlign(TextAlign textAlign) {
#define __Top DWRITE_PARAGRAPH_ALIGNMENT_NEAR
#define	__Bottom DWRITE_PARAGRAPH_ALIGNMENT_FAR
#define	__Left DWRITE_TEXT_ALIGNMENT_LEADING
#define	__Right DWRITE_TEXT_ALIGNMENT_TRAILING
#define	__Middle DWRITE_PARAGRAPH_ALIGNMENT_CENTER
#define __Center DWRITE_TEXT_ALIGNMENT_CENTER
		if (m_textLayout == NULL)return;
		//垂直对其方式
		if (((int)textAlign & (int)VAlign::Top) == (int)VAlign::Top) {
			m_textLayout->SetParagraphAlignment(__Top);
		}
		if (((int)textAlign & (int)VAlign::Mid) == (int)VAlign::Mid) {
			m_textLayout->SetParagraphAlignment(__Middle);
		}
		if (((int)textAlign & (int)VAlign::Bottom) == (int)VAlign::Bottom) {
			m_textLayout->SetParagraphAlignment(__Bottom);
		}
		//水平对其方式
		if (((int)textAlign & (int)HAlign::Left) == (int)HAlign::Left) {
			m_textLayout->SetTextAlignment(__Left);
		}
		if (((int)textAlign & (int)HAlign::Center) == (int)HAlign::Center) {
			m_textLayout->SetTextAlignment(__Center);
		}
		if (((int)textAlign & (int)HAlign::Right) == (int)HAlign::Right) {
			m_textLayout->SetTextAlignment(__Right);
		}
#undef __Top 
#undef __Bottom 
#undef __Left 
#undef __Right 
#undef __Middle 
#undef __Center
	}
	void TextLayout::SetUnderline(int pos, int count)
	{
		if (count > 0) {
			m_textLayout->SetUnderline(TRUE, { (UINT32)pos,(UINT32)count });
		}
	}
	TextLayout::~TextLayout() {
		if (m_textLayout) {
			SafeRelease(&m_textLayout);
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
	int DXImage::Width() {
		return m_width;
	}
	int DXImage::Height() {
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

	// 获取 GIF 帧位置
	WICRect GetFrameRect(IWICMetadataQueryReader* metaReader) {
		WICRect rect = { 0,0,0,0 };
		PROPVARIANT pv;
		PropVariantInit(&pv);

		if (SUCCEEDED(metaReader->GetMetadataByName(L"/imgdesc/Left", &pv))) {
			rect.X = pv.uiVal;
		}
		PropVariantClear(&pv);

		if (SUCCEEDED(metaReader->GetMetadataByName(L"/imgdesc/Top", &pv)))
		{
			rect.Y = pv.uiVal;
		}
		PropVariantClear(&pv);

		if (SUCCEEDED(metaReader->GetMetadataByName(L"/imgdesc/Width", &pv)))
		{
			rect.Width = pv.uiVal;
		}
		PropVariantClear(&pv);

		if (SUCCEEDED(metaReader->GetMetadataByName(L"/imgdesc/Height", &pv)))
		{
			rect.Height = pv.uiVal;
		}
		PropVariantClear(&pv);

		return rect;
	}

	// 拷贝整个位图
	void CopyBitmap(IWICBitmap* src, IWICBitmap* dst) {
		if (!src || !dst) return;

		UINT w = 0, h = 0;
		src->GetSize(&w, &h);
		WICRect rect = { 0,0,(INT)w,(INT)h };

		IWICBitmapLock* lockSrc = NULL;
		IWICBitmapLock* lockDst = NULL;

		if (SUCCEEDED(src->Lock(&rect, WICBitmapLockRead, &lockSrc)) &&
			SUCCEEDED(dst->Lock(&rect, WICBitmapLockWrite, &lockDst)))
		{
			UINT cbBufferSrc = 0, cbBufferDst = 0;
			BYTE* pSrc = NULL;
			BYTE* pDst = NULL;
			lockSrc->GetDataPointer(&cbBufferSrc, &pSrc);
			lockDst->GetDataPointer(&cbBufferDst, &pDst);

			memcpy(pDst, pSrc, cbBufferSrc);
		}

		if (lockSrc) SafeRelease(&lockSrc);
		if (lockDst) SafeRelease(&lockDst);
	}

	// 清除矩形区域（透明）
	void ClearRegion(IWICBitmap* bmp, const WICRect& rect) {
		if (!bmp) return;

		IWICBitmapLock* lock = NULL;
		if (SUCCEEDED(bmp->Lock(&rect, WICBitmapLockWrite, &lock)))
		{
			UINT cbBuffer = 0;
			BYTE* pData = NULL;
			lock->GetDataPointer(&cbBuffer, &pData);
			memset(pData, 0, cbBuffer); // 全部置 0（透明）
		}
		SafeRelease(&lock);
	}
	void CopyFrameToCanvas(IWICBitmapSource* frame, IWICBitmap* canvas, UINT CanvasWidth, UINT CanvasHeight, const WICRect& frameRect)
	{
		if (!frame || !canvas) return;

		// 锁定整个画布
		WICRect fullRect{ 0, 0, (INT)CanvasWidth, (INT)CanvasHeight };
		IWICBitmapLock* lock = NULL;
		if (FAILED(canvas->Lock(&fullRect, WICBitmapLockWrite, &lock))) return;

		UINT cbBuffer = 0;
		BYTE* pDst = NULL;
		lock->GetDataPointer(&cbBuffer, &pDst);

		UINT canvasStride = CanvasWidth * 4;
		UINT frameStride = frameRect.Width * 4;
		UINT frameBufferSize = frameStride * frameRect.Height;
		BYTE* tempBuffer = new BYTE[frameBufferSize];

		WICRect copyRect = { 0, 0, frameRect.Width, frameRect.Height };
		HRESULT hr = frame->CopyPixels(&copyRect, frameStride, frameBufferSize, tempBuffer);
		if (FAILED(hr)) {
			delete[] tempBuffer;
			SafeRelease(&lock);
			return;
		}

		// 复制像素到画布的对应偏移位置
		for (UINT y = 0; y < frameRect.Height; y++) {
			BYTE* srcLine = tempBuffer + y * frameStride;
			BYTE* dstLine = pDst + (frameRect.Y + y) * canvasStride + frameRect.X * 4;

			for (UINT x = 0; x < frameRect.Width; x++) {
				BYTE* srcPx = srcLine + x * 4;
				BYTE* dstPx = dstLine + x * 4;

				BYTE srcA = srcPx[3];
				if (srcA != 0) {
					dstPx[0] = srcPx[0];
					dstPx[1] = srcPx[1];
					dstPx[2] = srcPx[2];
					dstPx[3] = srcPx[3];
				}
			}
		}

		//释放
		delete[] tempBuffer;
		SafeRelease(&lock);
	}

	void DXImage::Init()
	{
		m_framePos = 0;
		m_frames.clear();

		if (!m_bitmapdecoder) return;

		UINT fCount = 0;
		m_bitmapdecoder->GetFrameCount(&fCount);
		m_frameCount = fCount;

		if (fCount <= 1) {
			return;
		}
		if (m_fmtcovter) {
			//如果是gif那就不需要m_fmtcovter 因为会把解析完毕的每一帧存储起来直接用
			SafeRelease(&m_fmtcovter);
		}

		UINT width = Width();
		UINT height = Height();

		// 创建全尺寸画布
		IWICBitmap* canvas = NULL;
		D2D::g_ImageFactory->CreateBitmap(
			width, height,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnLoad,
			&canvas);

		// 备份画布（用于 disposal=3）
		IWICBitmap* prevCanvas = NULL;
		D2D::g_ImageFactory->CreateBitmap(
			width, height,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnLoad,
			&prevCanvas);

		for (UINT i = 0; i < fCount; i++)
		{
			IWICBitmapFrameDecode* srcFrame = NULL;
			m_bitmapdecoder->GetFrame(i, &srcFrame);

			// 元数据读取器
			IWICMetadataQueryReader* metaReader = NULL;
			srcFrame->GetMetadataQueryReader(&metaReader);

			// 延时
			UINT delayMs = 100;
			PROPVARIANT pv;
			PropVariantInit(&pv);
			if (SUCCEEDED(metaReader->GetMetadataByName(L"/grctlext/Delay", &pv)))
			{
				delayMs = pv.uiVal * 10; // 单位 1/100 秒
			}
			PropVariantClear(&pv);

			// Disposal
			UINT disposal = 0;
			if (SUCCEEDED(metaReader->GetMetadataByName(L"/grctlext/Disposal", &pv)))
			{
				disposal = pv.uiVal;
			}
			PropVariantClear(&pv);

			// Disposal=3 保存备份
			if (disposal == 3) {
				CopyBitmap(canvas, prevCanvas);
			}

			// Disposal=2 清区域
			if (disposal == 2 && i > 0)
			{
				WICRect rect = GetFrameRect(metaReader);
				ClearRegion(canvas, rect);
			}
			else if (disposal == 3 && i > 0)
			{
				CopyBitmap(prevCanvas, canvas);
			}

			// 转换帧格式
			IWICFormatConverter* converter = NULL;
			D2D::g_ImageFactory->CreateFormatConverter(&converter);
			converter->Initialize(srcFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom);

			// 帧矩形
			WICRect rect = GetFrameRect(metaReader);

			// 绘制到 canvas
			CopyFrameToCanvas(converter, canvas, width, height, rect);

			// 存一份当前画布的完整副本
			IWICBitmap* frameCopy = NULL;
			D2D::g_ImageFactory->CreateBitmapFromSource(canvas, WICBitmapCacheOnLoad, &frameCopy);
			GifFrame gf = { frameCopy, delayMs };
			m_frames.push_back(gf);

			// 释放临时对象
			SafeRelease(&converter);
			SafeRelease(&metaReader);
			SafeRelease(&srcFrame);

		}
		// 释放画布
		SafeRelease(&canvas);
		SafeRelease(&prevCanvas);
		//读取第一帧
		this->NextFrame();
	}

	WORD DXImage::NextFrame() {
		if (m_frames.empty()) {
			return 0; // 没有帧
		}
		if (m_framePos >= m_frames.size()) {
			m_framePos = 0;
		}
		this->m_bitMap = m_frames[m_framePos].wicBitmap;
		UINT delayMs = m_frames[m_framePos].delay;
		// 这里不需要释放和创建格式转换器了，播放时再转 D2D 位图
		// 只移动索引
		m_framePos++;
		return (WORD)delayMs;
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
			//WICBitmapUsePremultipliedAlpha:传入的图像颜色值已经与 Alpha 通道预乘
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
	DXImage::DXImage(int width, int height)
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
		for (auto& it : m_frames) {
			SafeRelease(&it.wicBitmap);
		}
	}

	void Geometry::Combine(Geometry& out, const Geometry& a, const Geometry& b, D2D1_COMBINE_MODE COMBINE_MODE)
	{
		ID2D1PathGeometry* outPathGeometry = NULL;
		D2D::g_Direct2dFactory->CreatePathGeometry(&outPathGeometry);
		ID2D1GeometrySink* geometrySink = NULL;
		outPathGeometry->Open(&geometrySink);
		HRESULT ret = a.m_rgn->CombineWithGeometry(b.m_rgn, COMBINE_MODE, NULL, geometrySink);
		geometrySink->Close();
		if (out.m_rgn) {
			SafeRelease(&out.m_rgn);
		}
		out.m_rgn = outPathGeometry;
		SafeRelease(&geometrySink);
	}

	void RectangleGeometry::Create(float x, float y, float width, float height, float _radius)
	{
		if (_radius > 0) {
			float radius = GetMaxRadius(width, height, _radius);
			D2D1_ROUNDED_RECT rectF{ x,y,(x + width),(y + height) ,radius ,radius };
			D2D::g_Direct2dFactory->CreateRoundedRectangleGeometry(rectF, (ID2D1RoundedRectangleGeometry**)&m_rgn);
		}
		else {
			D2D_RECT_F rectF{ x,y,(x + width),(y + height) };
			D2D::g_Direct2dFactory->CreateRectangleGeometry(rectF, (ID2D1RectangleGeometry**)&m_rgn);
		}
	}
	RectangleGeometry::RectangleGeometry(float x, float y, float width, float height, float _radius) {
		Create(x, y, width, height, _radius);
	}
	RectangleGeometry::RectangleGeometry(const RectF& _rect, float _radius)
	{
		float x = _rect.X, y = _rect.Y, width = _rect.Width, height = _rect.Height;
		Create(x, y, width, height, _radius);
	}
	RectangleGeometry::RectangleGeometry(const RectF& _rect, float topLeftRadius, float topRightRadius, float bottomRightRadius, float bottomLeftRadius)
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

	Geometry::Geometry() {
	}

	void Geometry::AddArc(const PointF& endPoint, float radius) {
		m_pSink->AddArc(
			D2D1::ArcSegment(
				D2D1::Point2F(endPoint.X, endPoint.Y),
				D2D1::SizeF(radius, radius),
				0.0f,
				D2D1_SWEEP_DIRECTION_CLOCKWISE,
				D2D1_ARC_SIZE_SMALL
			)
		);
	}

	void Geometry::AddAcr(const D2D1_ARC_SEGMENT& arc)
	{
		m_pSink->AddArc(arc);
	}

	void Geometry::AddLine(const PointF& endPoint) {
		D2D1_POINT_2F p{ endPoint.X, endPoint.Y };
		m_pSink->AddLine(p);
	}
	void Geometry::BeginFigure(const PointF& startPoint, D2D1_FIGURE_BEGIN figureBegin) {
		ID2D1PathGeometry* path;
		D2D::g_Direct2dFactory->CreatePathGeometry(&path);
		path->Open(&m_pSink);
		m_pSink->BeginFigure(__To_D2D_PointF(startPoint), figureBegin);
		m_rgn = path;
	}
	void Geometry::CloseFigure(D2D1_FIGURE_END figureEnd) {
		m_pSink->EndFigure(figureEnd);
		m_pSink->Close(); // 完成 Path 定义
	}

	Geometry::~Geometry() {
		if (m_pSink) {
			SafeRelease(&m_pSink);
		}
		if (m_rgn) {
			SafeRelease(&m_rgn);

		}
	}
	ID2D1Geometry* Geometry::Get() const {
		return m_rgn;
	}


	PieGeometry::PieGeometry(const RectF& rectF, float startAngle, float endAngle)
	{
		auto rect = __To_D2D_RectF(rectF);
		// 1. 计算圆心和半径
		D2D1_POINT_2F center = {
			(rect.left + rect.right) / 2.0f,
			(rect.top + rect.bottom) / 2.0f
		};

		float radiusX = (rect.right - rect.left) / 2.0f;
		float radiusY = (rect.bottom - rect.top) / 2.0f;

		bool isFullCircle = fabsf(endAngle - startAngle) >= 359.999f;
		if (isFullCircle) {
			// 方法2：直接使用椭圆几何（更高效）
			ID2D1EllipseGeometry* pEllipse = NULL;
			D2D1_ELLIPSE ellipse{ center, radiusX, radiusY };
			D2D::g_Direct2dFactory->CreateEllipseGeometry(&ellipse, &pEllipse);
			//赋值
			this->m_rgn = (ID2D1Geometry*)pEllipse;
		}
		else {

			// 2. 角度处理（转换为弧度）
			float startRad = startAngle * (3.1415926f / 180.0f);
			float endRad = endAngle * (3.1415926f / 180.0f);

			// 3. 创建路径几何
			ID2D1PathGeometry* pGeometry = NULL;
			D2D::g_Direct2dFactory->CreatePathGeometry(&pGeometry);

			ID2D1GeometrySink* pSink = NULL;
			pGeometry->Open(&pSink);
			// 4. 计算弧线起点和终点
			D2D1_POINT_2F startPoint = {
				center.x + radiusX * cosf(startRad),
				center.y + radiusY * sinf(startRad)
			};
			D2D1_POINT_2F endPoint = {
				center.x + radiusX * cosf(endRad),
				center.y + radiusY * sinf(endRad)
			};

			// 5. 构造扇形路径
			pSink->BeginFigure(center, D2D1_FIGURE_BEGIN_FILLED);
			pSink->AddLine(startPoint);

			D2D1_ARC_SEGMENT arc{
				endPoint,
				D2D1::SizeF(radiusX, radiusY),
				0.0f,
				(endRad > startRad) ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE,
				(fabsf(endRad - startRad) <= 3.1415926f) ? D2D1_ARC_SIZE_SMALL : D2D1_ARC_SIZE_LARGE
			};
			pSink->AddArc(arc);
			pSink->AddLine(center);
			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			//释放
			pSink->Close();
			SafeRelease(&pSink);
			//赋值
			this->m_rgn = pGeometry;
		}
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
						D2D1_ALPHA_MODE_PREMULTIPLIED),
					0,
					0,
					D2D1_RENDER_TARGET_USAGE_NONE,
					D2D1_FEATURE_LEVEL_DEFAULT
				);
				//初始化一下d2d让第一次启动窗口快一点
				ID2D1DCRenderTarget* initRender = NULL;
				hr = D2D::g_Direct2dFactory->CreateDCRenderTarget(&defaultOption, (ID2D1DCRenderTarget**)&initRender);
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
				sprintf_s(buf, "Code 0x%08X", hr);
				::MessageBoxA(NULL, buf, "Failed to create IWICImagingFactory", MB_ICONSTOP);
			}
		}

		if (g_GdiplusToken == NULL) {
			//初始化GDI+
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			Gdiplus::GdiplusStartup(&g_GdiplusToken, &gdiplusStartupInput, NULL);
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
		//释放GDI+
		if (g_GdiplusToken) {
			Gdiplus::GdiplusShutdown(g_GdiplusToken);
			g_GdiplusToken = NULL;
		}
	}
	float GetMaxRadius(float width, float height, float _radius)
	{
		float radius = _radius;//半径
		float diameter = radius * 2;//直径
		if (width > height || (std::fabs(width - height) < EZUI_FLOAT_EPSILON)) {
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
				D2D1_ALPHA_MODE_PREMULTIPLIED),//表示三通道的值已经是预乘了a通之后的数值
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
	void DXRender::SetStrokeStyle(StrokeStyle strokeStyle)
	{
		if (m_pStrokeStyle != NULL) {
			SafeRelease(&m_pStrokeStyle);
		}
		if (strokeStyle == StrokeStyle::Dash) {
			// 使用系统默认的虚线样式
			D2D1_DASH_STYLE dashStyle = D2D1_DASH_STYLE_DASH;  // 常规虚线样式
			// 创建虚线样式
			D2D::g_Direct2dFactory->CreateStrokeStyle(
				D2D1::StrokeStyleProperties(
					D2D1_CAP_STYLE_FLAT,                // 线帽样式
					D2D1_CAP_STYLE_FLAT,                // 结束线帽样式
					D2D1_CAP_STYLE_ROUND,               // 虚线部分的线帽样式
					D2D1_LINE_JOIN_ROUND,               // 线段连接样式
					10.0f,                              // 斜接限制
					dashStyle,                          // 设置虚线样式为常规虚线
					0.0f                                 // 自定义虚线的间隔（不需要）
				),
				NULL, 0, &m_pStrokeStyle);
		}
	}
	void DXRender::DrawTextLayout(const TextLayout& textLayout, const PointF& startLacation) {
		m_render->DrawTextLayout(D2D1_POINT_2F{ startLacation.X ,startLacation.Y }, textLayout.Get(), GetBrush());
	}
	void DXRender::DrawString(const std::wstring& text, const  RectF& rect, ezui::TextAlign textAlign) {
		TextLayout textLayout(text, *m_font, { rect.Width, rect.Height }, textAlign);
		this->DrawTextLayout(textLayout, { rect.X,rect.Y });
	}
	void DXRender::DrawLine(const PointF& A, const PointF& B, float width) {
		m_render->DrawLine(D2D1_POINT_2F{ A.X,A.Y }, D2D1_POINT_2F{ B.X,B.Y }, GetBrush(), width, GetStrokeStyle());
	}

	void DXRender::DrawRectangle(const RectF& rect, float _radius, float width)
	{
		if (_radius > 0) {
			float radius = GetMaxRadius(rect.Width, rect.Height, _radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), radius, radius };
			m_render->DrawRoundedRectangle(roundRect, GetBrush(), width, GetStrokeStyle());
		}
		else {
			m_render->DrawRectangle(__To_D2D_RectF(rect), GetBrush(), width, GetStrokeStyle());
		}
	}


	void DXRender::FillRectangle(const RectF& rect, float _radius) {
		if (_radius > 0) {
			float radius = GetMaxRadius(rect.Width, rect.Height, _radius);
			D2D1_ROUNDED_RECT roundRect{ __To_D2D_RectF(rect), radius, radius };
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
		m_render->DrawGeometry(pathGeometry, GetBrush(), width, GetStrokeStyle());
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
		RectF rect(tagRect.X, tagRect.Y, tagRect.Width, tagRect.Height);
		//转换坐标,缩放
		Size imgSize(image->Width(), image->Height());
		if (!sourceRect.IsEmptyArea()) {
			imgSize.Width = sourceRect.Width;
			imgSize.Height = sourceRect.Height;
		}

		//加上在Owner区域的偏移
		rect.X += image->DrawPosition.X;
		rect.Y += image->DrawPosition.Y;

		RectF realRendRect;//最终渲染的矩形位置
		if (!image->DrawSize.Empty()) {
			//限制在Owner中的绘制区域
			realRendRect = RectF(rect.X, rect.Y, image->DrawSize.Width, image->DrawSize.Height);
		}
		else {
			//不限制绘制区域 根据imageSizeMode属性进行坐标转换
			realRendRect = ezui::Transformation(imageSizeMode, rect, imgSize);
		}
		//开始绘制
		D2D_RECT_F drawRectF = __To_D2D_RectF(realRendRect);
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
	void DXRender::DrawEllipse(const RectF& rectF, float strokeWidth)
	{
		PieGeometry pie(rectF, 0, 360);
		DrawGeometry(pie.Get(), strokeWidth);
	}
	void DXRender::FillEllipse(const RectF& rectF)
	{
		PieGeometry pie(rectF, 0, 360);
		FillGeometry(pie.Get());
	}

	void DXRender::DrawPie(const RectF& rectF, float startAngle, float endAngle, float strokeWidth)
	{
		PieGeometry pie(rectF, startAngle, endAngle);
		DrawGeometry(pie.Get(), strokeWidth);
	}
	void DXRender::FillPie(const RectF& rectF, float startAngle, float endAngle)
	{
		PieGeometry pie(rectF, startAngle, endAngle);
		FillGeometry(pie.Get());
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
		m_render->DrawGeometry(geometry, GetBrush(), width, this->GetStrokeStyle());
	}
	void DXRender::FillGeometry(ID2D1Geometry* geometry)
	{
		m_render->FillGeometry(geometry, GetBrush());
	}
	void DXRender::DrawGeometry(Geometry* path, float width)
	{
		DrawGeometry(path->Get(), width);
	}
	void DXRender::FillGeometry(Geometry* path)
	{
		FillGeometry(path->Get());
	}
	void DXRender::Flush()
	{
		m_render->Flush();
	}
	ID2D1DCRenderTarget* DXRender::Get() {
		return m_render;
	}
};

#endif