#include "CPURender.h"
ULONG_PTR _gdiplusToken = NULL;
#ifdef CreateFont
#undef CreateFont
#endif
SolidBrush* CreateBrush(const Color& color) {
	SolidBrush* _bufBrush = new SolidBrush(color);
	return _bufBrush;
}
Pen* CreatePen(const Color& color, float width) {
	Pen* pen = new Pen(color, width);
	return pen;
}
Gdiplus::Font* CreateFont(const EString& fontFamily, float fontSize) {
	Gdiplus::FontFamily ff(fontFamily.utf16().c_str());
	Gdiplus::Font* _bufFont = new Gdiplus::Font(&ff, fontSize);
	return _bufFont;
}
Image::Image(const EString& filename, int radius) :Gdiplus::Image(filename.utf16().c_str()) {
	if (radius > 0) {
		ClipImage(this, Size{ (int)GetWidth(), (int)GetHeight() }, radius, &BufBitmap);
	}
}
Image* Image::Clone() {
	Gdiplus::GpImage* cloneimage = NULL;
	SetStatus(Gdiplus::DllExports::GdipCloneImage(nativeImage, &cloneimage));
	Image* newImage = new Image(cloneimage, lastResult);
	newImage->Box = Box;
	//newImage->BufBitmap = BufBitmap;
	return newImage;
}
Image::~Image() {
	if (BufBitmap) {
		delete BufBitmap;
	}
}
//
//void HImage::Clone(const Image* img) {
//	if (value || img == NULL) {
//		delete value;
//		value = NULL;
//	}
//	valid = false;
//	if (img) {
//		value = (((Image*)img)->Clone());
//		valid = true;
//	}
//}
//HImage::HImage(const HImage& img) {
//	((HImage*)&img)->ref++;
//	this->value = img.value;
//	this->valid = img.valid;
//}
//HImage& HImage::operator=(const HImage& img) {
//	((HImage*)&img)->ref++;
//	this->value = img.value;
//	this->valid = img.valid;
//	return *this;
//}
//HImage::HImage() {
//}
//void HImage::operator=(const Image* img) {
//	Clone(img);
//}
//HImage::operator Image* () {
//	return value;
//}
//Image* HImage::operator->() {
//	return value;
//}
//HImage::~HImage() {
//	if (ref > 0) {
//		this;
//	}
//	if (value && ref == 0) {
//		delete value;
//	}
//}

void RenderInitialize()
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);//??ʼ??gdi+
}
void RenderUnInitialize()
{
	Gdiplus::GdiplusShutdown(_gdiplusToken); //?ر?gdi+
}

void HighQualityMode(Gdiplus::Graphics* graphics)
{
	graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);//??????
	graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);//????ƫ??ģʽ
	graphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);//????
	graphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);//ͼ??
}
void CreateRectangle(GraphicsPath& path, const Rect& rect, float radius)
{
	if (radius <= 0) {
		path.AddRectangle(rect);
		path.CloseFigure();
		return;
	}
	int diameter = radius;
	RectF arcRect(rect.X, rect.Y, diameter, diameter);// = new Rect(rect.Location, new Size(diameter, diameter));
	//   ???Ͻ?      
	path.AddArc(arcRect, 180, 90);
	//   ???Ͻ?      
	arcRect.X = rect.GetRight() - diameter;
	path.AddArc(arcRect, 270, 90);
	//   ???½?      
	arcRect.Y = rect.GetBottom() - diameter;
	path.AddArc(arcRect, 0, 90);
	//   ???½?      
	arcRect.X = rect.GetLeft();
	path.AddArc(arcRect, 90, 90);
	path.CloseFigure();
}
void ClipImage(Image* img, const Size& sz, int _radius, Bitmap** outBitmap)
{
	int width = img->GetWidth();
	int height = img->GetHeight();

	Bitmap* bitmap = new Bitmap(width, height);
	Gdiplus::Graphics g(bitmap);
	HighQualityMode(&g);
	double rate = width * 1.0 / sz.Width;
	float radius = _radius * rate;

	Gdiplus::GraphicsPath path;
	CreateRectangle(path, Rect{ 0,0,width,height }, radius);
	g.SetClip(&path);
	g.DrawImage(img, 0, 0, sz.Width, sz.Height);
	*outBitmap = bitmap;
}

CPURender::CPURender(Bitmap* image)
{
	base = new  Gdiplus::Graphics(image);
	HighQualityMode(base);
}
CPURender::CPURender(HDC hdc)
{
	DC = hdc;
	base = new  Gdiplus::Graphics(hdc);
	HighQualityMode(base);
}

CPURender::CPURender(HWND hWnd)
{
	base = new  Gdiplus::Graphics(hWnd);
	HighQualityMode(base);
}

CPURender::~CPURender()
{
	delete base;
}

void CPURender::CreateFormat(TextAlign textAlign, Gdiplus::StringFormat& outStrFormat) {
	switch (textAlign)
	{
	case TextAlign::TopLeft:
	{
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		break;
	}
	case TextAlign::TopCenter:
	{
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		break;
	}
	case TextAlign::TopRight:
	{
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		break;
	}
	case TextAlign::MiddleLeft:
	{
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		break;
	}
	case TextAlign::MiddleCenter:
	{
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		break;
	}
	case TextAlign::MiddleRight:
	{
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		break;
	}
	case TextAlign::BottomLeft:
	{
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
		break;
	}
	case TextAlign::BottomCenter:
	{
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentCenter);
		break;
	}
	case TextAlign::BottomRight:
	{
		outStrFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
		outStrFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentFar);
		break;
	}
	default:
		break;
	}
}

void CPURender::DrawString(const std::wstring& text, const Gdiplus::Font* font, const Color& color, const RectF& rect, TextAlign textAlign, bool underLine)
{
	Gdiplus::StringFormat sf;
	CreateFormat(textAlign, sf);

	SafeObject<SolidBrush> brush(CreateBrush(color));

	base->DrawString(text.c_str(), text.length(), font, rect, &sf, brush);
	if (underLine) {
		RectF box;
		base->MeasureString(text.c_str(), text.length(), font, rect, &sf, &box);
		PointF p1(box.X, box.GetBottom());
		PointF p2(box.GetRight(), box.GetBottom());
		SafeObject<Pen> pen(CreatePen(color, 1));
		base->DrawLine(pen, p1, p2);
	}
}

void CPURender::DrawRectangle(const Color& color, const Rect& _rect, float width, float radius)
{
	if (color.GetA() == 0) {
		return;
	}
	Rect rect = _rect;
	rect.X += OffsetX;
	rect.Y += OffsetY;

	SafeObject<Pen> pen(CreatePen(color, width));
	if (radius > 0) {
		Gdiplus::GraphicsPath path;
		CreateRectangle(path, rect, radius);
		base->DrawPath(pen, &path);
	}
	else {
		base->DrawRectangle(pen, rect);
	}
}
void CPURender::FillRectangle(const Color& color, const Rect& _rect, int radius)
{
	if (color.GetA() == 0) {
		return;
	}
	Rect rect = _rect;
	rect.X += OffsetX;
	rect.Y += OffsetY;

	SafeObject<SolidBrush> brush(CreateBrush(color));
	if (radius > 0) {
		Gdiplus::GraphicsPath path;
		CreateRectangle(path, rect, radius);
		base->FillPath(brush, &path);
	}
	else {
		base->FillRectangle(brush, rect);
	}
}
void CPURender::DrawString(const EString& text, const EString& fontFamily, float fontSize, const Color& color, const RectF& _rect, TextAlign textAlign, bool underLine)
{
	RectF rect = _rect;
	rect.X += OffsetX;
	rect.Y += OffsetY;
	SafeObject<Gdiplus::Font> font(CreateFont(fontFamily, fontSize));
	this->DrawString(text.utf16(), font, color, rect, textAlign, underLine);
}



void CPURender::MeasureString(const EString& _text, const EString& fontf, float fontSize, RectF& outBox) {
	std::wstring _wtext = _text.utf16();
	SafeObject<Gdiplus::Font> font(CreateFont(fontf, fontSize));
	base->MeasureString(_wtext.c_str(), _wtext.length(), font, { 0,0 }, &outBox);
}

void CPURender::CreateLayer(const Rect& rect, ClipMode clipMode, int radius)
{
	Layer.push_back(&rect);
	base->SetClip(rect, (Gdiplus::CombineMode)clipMode);
}
void CPURender::PopLayer()
{
	base->ResetClip();
	if (!Layer.empty()) {
		Layer.pop_back();
		if (!Layer.empty()) {
			auto it = Layer.back();
			base->SetClip(*(Rect*)it);
		}
	}
}
void CPURender::DrawLine(const Color& color, const Point& _A, const Point& _B, float width)
{
	Point A = _A;
	A.X += OffsetX;
	A.Y += OffsetY;
	Point B = _B;
	B.X += OffsetX;
	B.Y += OffsetY;
	SafeObject<Pen> pen(CreatePen(color, width));
	base->DrawLine(pen, A, B);
}
void CPURender::DrawImage(Image* image, const Rect& _rect, float radius)
{
	if (!image || image->GetLastStatus() != Gdiplus::Status::Ok) return;
	Rect rect = _rect;
	rect.X += OffsetX;
	rect.Y += OffsetY;
	if (image->Box.X != 0) {
		rect.X += image->Box.X;
	}
	if (image->Box.Y != 0) {
		rect.Y += image->Box.Y;
	}
	if (!image->Box.IsEmptyArea()) {
		rect.Width = image->Box.Width;
		rect.Height = image->Box.Height;
	}
	if (image->BufBitmap) {
		base->DrawImage(image->BufBitmap, RectF(rect.X, rect.Y, rect.Width, rect.Height));
		return;
	}
	if (radius > 0) {
		Bitmap* bitmap(0);
		ClipImage(image, { rect.Width,rect.Height }, radius, &bitmap);
		base->DrawImage(bitmap, rect);
		//#if 1 //GDI+????
		//#else //Gdi????
		//		HBITMAP outHMap;
		//		bitmap->GetHBITMAP(Color::Transparent, &outHMap);
		//		HDC hdc = ::CreateCompatibleDC(DC);
		//		HGDIOBJ	_hgdiobj = ::SelectObject(hdc, outHMap);
		//		int srcWidth = bitmap->GetWidth();
		//		int srcHeight = bitmap->GetHeight();
		//		int srcX = Layer.X > rect.X ? srcWidth - Layer.Width : 0;
		//		int srcY = Layer.Y > rect.Y ? srcHeight - Layer.Height : 0;
		//		//::BitBlt(DC, Layer.X, Layer.Y, Layer.Width, Layer.Height, hdc, srcX, srcY, SRCCOPY);
		//		BLENDFUNCTION blend;
		//		blend.BlendOp = AC_SRC_OVER;
		//		blend.BlendFlags = 0;
		//		blend.AlphaFormat = AC_SRC_ALPHA;
		//		blend.SourceConstantAlpha = 255;
		//		//::AlphaBlend(DC, Layer.X, Layer.Y, Layer.Width, Layer.Height, hdc, srcX, srcY, Layer.Width, Layer.Height, blend);
		//		::StretchBlt(DC, Layer.X, Layer.Y, Layer.Width, Layer.Height, hdc, srcX, srcY, srcWidth, srcHeight, SRCCOPY);
		//
		//		::SelectObject(hdc, _hgdiobj);
		//		::DeleteDC(hdc);
		//		::DeleteBitmap(outHMap);
		//#endif
		delete bitmap;
		return;
	}
	//int srcWidth = image->GetWidth();
	//int srcHeight = image->GetHeight();
	//if (srcWidth == rect.Width && srcHeight == rect.Height) {
	//	base->DrawImage(image, rect.X, rect.Y);
	//	return;
	//}
	//if (srcWidth ==48 && srcHeight == 46) {
	//	int pause = 0;
	//}
	//base->DrawImage(image, rect);
	base->DrawImage(image, rect);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}
BOOL SaveHDCToFile(HDC hDC, LPRECT lpRect, const WCHAR* format, const WCHAR* filename)
{
	BOOL bRet = FALSE;
	int nWidth = lpRect->right - lpRect->left;
	int nHeight = lpRect->bottom - lpRect->top;
	//??Ŀ????????ͼ???ڴ?BITMAP
	HDC memDC = CreateCompatibleDC(hDC);
	HBITMAP hBmp = CreateCompatibleBitmap(hDC, nWidth, nHeight);
	SelectObject(memDC, hBmp);
	BitBlt(memDC, lpRect->left, lpRect->top, nWidth, nHeight,
		hDC, 0, 0, SRCCOPY);
	//???????ļ?
	{
		//L"image/bmp" L"image/jpeg"  L"image/gif" L"image/tiff" L"image/png"
		CLSID pngClsid;
		GetEncoderClsid(format, &pngClsid);//?˴???BMPΪ??????????ʽѡ????Ӧ?????ͣ???JPG??L"image/jpeg" 
		Gdiplus::Bitmap* pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);
		if (pbmSrc->Save(filename, &pngClsid) == Gdiplus::Status::Ok)
		{
			bRet = TRUE;
		}
		delete pbmSrc;
	}
	//????????
	SelectObject(memDC, (HBITMAP)NULL);
	DeleteDC(memDC);
	DeleteObject(hBmp);
	return bRet;
}

void CPURender::SaveImage(const WCHAR* format, const WCHAR* fileName, const Size& size)
{
	if (DC) {
		::DeleteFileW(fileName);
		RECT rect{ 0,0,size.Width,size.Height };
		SaveHDCToFile(DC, &rect, format, fileName);
	}
}

