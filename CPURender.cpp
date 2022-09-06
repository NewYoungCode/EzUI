#include "CPURender.h"

namespace EzUI {
	ULONG_PTR _gdiplusToken = NULL;
#ifdef CreateFont
#undef CreateFont
#endif
	struct SafeClipRegion {
	public:
		HDC DC = NULL;
		HRGN Clip = NULL;
		SafeClipRegion(const HDC& _DC, const Rect& clip) {
			this->DC = _DC;
			if (!clip.IsEmptyArea()) {
				RECT r = clip.WinRECT();
				Clip = ::CreateRectRgn(r.left, r.top, r.right, r.bottom);
				::SelectClipRgn(DC, Clip);
			}
		}
		virtual ~SafeClipRegion() {
			if (Clip) {
				::SelectClipRgn(DC, NULL);
				::DeleteObject(Clip);
			}
		}
	};
	struct SafeFont {
	public:
		HDC DC = NULL;
		HFONT hFont = NULL;
		HGDIOBJ OLDFONT = NULL;
		SafeFont(const HDC& _DC, const EString& fontFamily, int fontSize, const Color& color, bool underLine) {
			this->DC = _DC;
			LOGFONTW lf = { 0 };
			GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
			wcsncpy_s(lf.lfFaceName, fontFamily.utf16().c_str(), LF_FACESIZE);
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfHeight = -MulDiv(fontSize, GetDeviceCaps(DC, LOGPIXELSY), 72);
			//if (0) lf.lfWeight += FW_BOLD;		//����
			lf.lfUnderline = underLine;				//�»���
			//if (0) lf.lfItalic = TRUE;				//б��
			SetTextColor(DC, RGB(color.GetR(), color.GetG(), color.GetB()));
			hFont = CreateFontIndirectW(&lf);
			OLDFONT = SelectObject(DC, hFont);
		}
		virtual ~SafeFont() {
			if (hFont) {
				SelectObject(DC, OLDFONT);
				DeleteFont(hFont);
			}
		}
	};
	///*if (Layer.size() > 0) {
	//			SafeClipRegion clipRgb(DC, (*(Layer.rbegin()))->WinRECT());
	//			SafeFont font(DC, fontFamily, fontSize, color, underLine);
	//			Rect rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
	//			rect.X += OffsetX;
	//			rect.Y += OffsetY;
	//			RECT winRect = rect.WinRECT();
	//			std::wstring wStr = text.utf16();
	//			DrawTextW(DC, wStr.c_str(), wStr.size(), &winRect, DT_SINGLELINE | (int)textAlign);
	//		}*/


	SolidBrush* CreateBrush(const Color& color) {
		SolidBrush* _bufBrush = new SolidBrush(color);
		return _bufBrush;
	}
	Pen* CreatePen(const Color& color, int width) {
		Pen* pen = new Pen(color, (float)width);
		return pen;
	}
	Gdiplus::Font* CreateFont(const EString& fontFamily, int fontSize) {
		Gdiplus::FontFamily ff(fontFamily.utf16().c_str());
		Gdiplus::Font* _bufFont = new Gdiplus::Font(&ff, (float)fontSize);
		return _bufFont;
	}

	void RenderInitialize()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);//��ʼ��gdi+
	}
	void RenderUnInitialize()
	{
		Gdiplus::GdiplusShutdown(_gdiplusToken); //�ر�gdi+
	}

	void HighQualityMode(Gdiplus::Graphics* graphics)
	{
		graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);//�����
		graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);//����ƫ��ģʽ
		graphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);//����
		//graphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);//ͼ��
	}
	void CreateRectangle(GraphicsPath& path, const Rect& rect, int radius)
	{
		if (radius <= 0) {
			path.AddRectangle(rect);
			path.CloseFigure();
			return;
		}
		int diameter = radius;
		Rect arcRect(rect.X, rect.Y, diameter, diameter);// = new Rect(rect.Location, new Size(diameter, diameter));
		//   ���Ͻ�      
		path.AddArc(arcRect, 180, 90);
		//   ���Ͻ�      
		arcRect.X = rect.GetRight() - diameter;
		path.AddArc(arcRect, 270, 90);
		//   ���½�      
		arcRect.Y = rect.GetBottom() - diameter;
		path.AddArc(arcRect, 0, 90);
		//   ���½�      
		arcRect.X = rect.GetLeft();
		path.AddArc(arcRect, 90, 90);
		path.CloseFigure();
	}

	CPURender::CPURender(HDC hdc)
	{
		DC = hdc;
		SetBkMode(DC, TRANSPARENT);
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

	void CPURender::DrawRectangle(const Rect& _rect, const Color& color, int width, int radius)
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
	void CPURender::FillRectangle(const Rect& _rect, const Color& color, int radius)
	{
		if (color.GetValue() == 0) {
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


	void CPURender::DrawString(const EString& text, const EString& fontFamily, int fontSize, const Color& color, const Rect& _rect, TextAlign textAlign, bool underLine)
	{
		Rect rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
		rect.X += OffsetX;
		rect.Y += OffsetY;

		HRGN clipRgn = NULL;
		HFONT hFont = NULL;
		HGDIOBJ oldFont = NULL;

		LOGFONTW lf{ 0 };
		GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
		wcsncpy_s(lf.lfFaceName, fontFamily.utf16().c_str(), LF_FACESIZE);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -MulDiv(fontSize, GetDeviceCaps(DC, LOGPIXELSY), 72);
		//if (0) lf.lfWeight += FW_BOLD;		//����
		lf.lfUnderline = underLine;				//�»���
		//if (0) lf.lfItalic = TRUE;				//б��
		hFont = CreateFontIndirectW(&lf);
		oldFont = SelectFont(DC, hFont);
		if (Layer.size() > 0) {
			RECT winClip = (*(Layer.rbegin()))->WinRECT();
			clipRgn = ::CreateRectRgn(winClip.left, winClip.top, winClip.right, winClip.bottom);
			::SelectClipRgn(DC, clipRgn);
		}

		std::wstring wStr = text.utf16();
		RECT winRECT = rect.WinRECT();
		SetTextColor(DC, RGB(color.GetR(), color.GetG(), color.GetB()));
		DrawTextW(DC, wStr.c_str(), wStr.size(), &winRECT, DT_SINGLELINE | (int)textAlign);
		if (clipRgn) {
			::SelectClipRgn(DC, NULL);
			::DeleteRgn(clipRgn);
		}
		if (hFont) {
			SelectFont(DC, oldFont);
			DeleteFont(hFont);
		}

		/*RectF rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
		rect.X += OffsetX;
		rect.Y += OffsetY;
		SafeObject<Gdiplus::Font> font(CreateFont(fontFamily, fontSize));
		this->DrawString(text.utf16(), font, color, rect, textAlign, underLine);*/
	}

	void CPURender::MeasureString(const EString& _text, const EString& fontf, int fontSize, RectF& outBox) {
		std::wstring _wtext = _text.utf16();
		SafeObject<Gdiplus::Font> font(CreateFont(fontf, fontSize));
		base->MeasureString(_wtext.c_str(), _wtext.length(), font, { 0,0 }, &outBox);
	}

	//δ����ƫ�� �ݲ�ʹ��
	//void CPURender::MeasureString(const EString& _text, const EString& fontFamily, int fontSize, const Color& color, const RectF& _rect, TextAlign textAlign, RectF& outBox) {
	//	std::wstring _wtext = _text.utf16();
	//	SafeObject<Gdiplus::Font> font(CreateFont(fontFamily, fontSize));
	//	Gdiplus::StringFormat sf;
	//	CreateFormat(textAlign, sf);
	//	base->MeasureString(_wtext.c_str(), _wtext.length(), font, _rect, &sf, &outBox);
	//}
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
	void CPURender::DrawLine(const Color& color, const Point& _A, const Point& _B, int width)
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

	void CPURender::DrawImage(Gdiplus::Image* image, const Rect& _rect, const ImageSizeMode& imageSizeMode, const Margin& margin)
	{
		if (!image || image->GetLastStatus() != Gdiplus::Status::Ok) return;
		Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		rect.X += margin.Left;
		rect.Y += margin.Top;
		rect.Width -= margin.Right * 2;
		rect.Height -= margin.Bottom * 2;

		if (imageSizeMode == ImageSizeMode::Zoom) {
			//�ͻ�������
			const int& clientWidth = rect.Width;
			const int& clientHeight = rect.Height;
			double clientRate = clientWidth * 1.0 / clientHeight;
			//ͼƬ����
			int imgWidth = image->GetWidth();
			int imgHeight = image->GetHeight();
			double imgRate = imgWidth * 1.0 / imgHeight;
			if (clientRate < imgRate) {
				double zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;
				Size sz{ clientWidth,(INT)zoomHeight };
				int y = (clientHeight - sz.Height) / 2 + rect.Y;
				base->DrawImage(image, Rect{ rect.X  ,y, sz.Width, sz.Height });
			}
			else {
				double zoomWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;
				Size sz{ (INT)zoomWidth,clientHeight };
				int x = (clientWidth - sz.Width) / 2 + rect.X;
				base->DrawImage(image, Rect{ x  , rect.Y, sz.Width, sz.Height });
			}
			return;
		}
		if (imageSizeMode == ImageSizeMode::CenterImage) {
			//�ͻ�������
			const int& clientWidth = rect.Width;
			const int& clientHeight = rect.Height;
			double clientRate = clientWidth * 1.0 / clientHeight;
			//ͼƬ����
			int imgWidth = image->GetWidth();
			int imgHeight = image->GetHeight();
			double imgRate = imgWidth * 1.0 / imgHeight;
			if (clientRate < imgRate) {
				//1000 670 �ͻ���
				//1000 300 ͼƬ
				//2233 670     ���ź��ͼƬ��С 
				int zoomWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;//ͼƬӦ����ô��Ŷ�
				int x = (zoomWidth - clientWidth) * 1.0 / 2 + 0.5;
				base->DrawImage(image, Rect{ rect.X - x,rect.Y,zoomWidth,clientHeight });
			}
			else {
				//1000 600 �ͻ���
				//400  600 ͼƬ
				//1000 1500     ���ź��ͼƬ��С 
				int zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;//ͼƬӦ����ô�߲Ŷ�
				int y = (zoomHeight - clientHeight) * 1.0 / 2 + 0.5;
				base->DrawImage(image, Rect{ rect.X,  rect.Y - y  , clientWidth, zoomHeight });
			}
			return;
		}

		if (imageSizeMode == ImageSizeMode::StretchImage || true) {
			base->DrawImage(image, rect);
		}
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
		//��Ŀ��������ͼ���ڴ�BITMAP
		HDC memDC = CreateCompatibleDC(hDC);
		HBITMAP hBmp = CreateCompatibleBitmap(hDC, nWidth, nHeight);
		SelectObject(memDC, hBmp);
		BitBlt(memDC, lpRect->left, lpRect->top, nWidth, nHeight,
			hDC, 0, 0, SRCCOPY);
		//������ļ�
		{
			//L"image/bmp" L"image/jpeg"  L"image/gif" L"image/tiff" L"image/png"
			CLSID pngClsid;
			GetEncoderClsid(format, &pngClsid);//�˴���BMPΪ����������ʽѡ���Ӧ�����ͣ���JPG��L"image/jpeg" 
			Gdiplus::Bitmap* pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);
			if (pbmSrc->Save(filename, &pngClsid) == Gdiplus::Status::Ok)
			{
				bRet = TRUE;
			}
			delete pbmSrc;
		}
		//������
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

};