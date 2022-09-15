#include "CPURender.h"

namespace EzUI {
	ULONG_PTR _gdiplusToken = NULL;
#ifdef CreateFont
#undef CreateFont
#endif

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
		Gdiplus::GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);//初始化gdi+
	}
	void RenderUnInitialize()
	{
		Gdiplus::GdiplusShutdown(_gdiplusToken); //关闭gdi+
	}

	void HighQualityMode(Gdiplus::Graphics* graphics)
	{
		graphics->SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);//抗锯齿
		graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);//像素偏移模式
		graphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);//文字
		//graphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);//图像
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
		//   左上角      
		path.AddArc(arcRect, 180, 90);
		//   右上角      
		arcRect.X = rect.GetRight() - diameter;
		path.AddArc(arcRect, 270, 90);
		//   右下角      
		arcRect.Y = rect.GetBottom() - diameter;
		path.AddArc(arcRect, 0, 90);
		//   左下角      
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
		_hwnd = hWnd;
		DC = ::GetDC(hWnd);
		base = new  Gdiplus::Graphics(DC);
		SetBkMode(DC, TRANSPARENT);
		HighQualityMode(base);
	}

	CPURender::CPURender(Gdiplus::Image* image)
	{
		base = new  Gdiplus::Graphics(image);
		HighQualityMode(base);
	}

	CPURender::~CPURender()
	{
		if (_hwnd) {
			::ReleaseDC(_hwnd, DC);
		}
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
		if (DC != NULL) {//使用GDI绘制文字
			Rect rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
			rect.X += OffsetX;
			rect.Y += OffsetY;
			HFONT hFont = NULL;
			HGDIOBJ oldFont = NULL;
			LOGFONTW lf{ 0 };
			GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
			wcsncpy_s(lf.lfFaceName, fontFamily.utf16().c_str(), LF_FACESIZE);
			lf.lfCharSet = DEFAULT_CHARSET;
			lf.lfHeight = -MulDiv(fontSize, GetDeviceCaps(DC, LOGPIXELSY), 72);
			//lf.lfWeight += FW_BOLD;		//粗体
			lf.lfUnderline = underLine;				//下划线
			//lf.lfItalic = TRUE;				//斜体
			hFont = CreateFontIndirectW(&lf);
			oldFont = SelectFont(DC, hFont);
			std::wstring wStr = text.utf16();
			RECT winRECT = rect.WinRECT();
			SetTextColor(DC, RGB(color.GetR(), color.GetG(), color.GetB()));
			DrawTextW(DC, wStr.c_str(), wStr.size(), &winRECT, DT_SINGLELINE | (int)textAlign);
			if (hFont) {
				SelectFont(DC, oldFont);
				DeleteFont(hFont);
			}
		}
		else
		{
			//使用GDI+绘制文字
			RectF rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
			rect.X += OffsetX;
			rect.Y += OffsetY;
			SafeObject<Gdiplus::Font> font(CreateFont(fontFamily, fontSize));
			this->DrawString(text.utf16(), font, color, rect, textAlign, underLine);
		}
	}

	void CPURender::MeasureString(const EString& _text, const EString& fontf, int fontSize, RectF& outBox) {
		std::wstring _wtext = _text.utf16();
		SafeObject<Gdiplus::Font> font(CreateFont(fontf, fontSize));
		base->MeasureString(_wtext.c_str(), _wtext.length(), font, { 0,0 }, &outBox);
	}

	void CPURender::CreateLayer(const Rect& rect, ClipMode clipMode, int radius)
	{
		Layer.push_back(&rect);

		if (DC) {
			HRGN _rgn = ::CreateRectRgn(rect.X, rect.Y, rect.GetRight(), rect.GetBottom());
			::SelectClipRgn(DC, _rgn);
			DeleteRgn(_rgn);
		}
		base->SetClip(rect, (Gdiplus::CombineMode)clipMode);
	}
	void CPURender::PopLayer()
	{
		if (DC) {
			::SelectClipRgn(DC, NULL);
		}
		base->ResetClip();
		if (!Layer.empty()) {
			Layer.pop_back();
			if (!Layer.empty()) {
				auto it = Layer.back();
				if (DC) {
					auto& rect = *(Rect*)it;
					HRGN _rgn = ::CreateRectRgn(rect.X, rect.Y, rect.GetRight(), rect.GetBottom());
					::SelectClipRgn(DC, _rgn);
					DeleteRgn(_rgn);
				}
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
	void CPURender::DrawImage(Gdiplus::Image* image, const Rect& destRect, const Rect& srcRect)
	{
		base->DrawImage(image, destRect, srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, Gdiplus::Unit::UnitPixel);
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
				base->DrawImage(image, Rect{ rect.X - x,rect.Y,zoomWidth,clientHeight });
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				int zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;//图片应该这么高才对
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
		//将目标区域贴图到内存BITMAP
		HDC memDC = CreateCompatibleDC(hDC);
		HBITMAP hBmp = CreateCompatibleBitmap(hDC, nWidth, nHeight);
		SelectObject(memDC, hBmp);
		BitBlt(memDC, lpRect->left, lpRect->top, nWidth, nHeight,
			hDC, 0, 0, SRCCOPY);
		//保存成文件
		{
			//L"image/bmp" L"image/jpeg"  L"image/gif" L"image/tiff" L"image/png"
			CLSID pngClsid;
			GetEncoderClsid(format, &pngClsid);//此处以BMP为例，其它格式选择对应的类型，如JPG用L"image/jpeg" 
			Gdiplus::Bitmap* pbmSrc = Gdiplus::Bitmap::FromHBITMAP(hBmp, NULL);
			if (pbmSrc->Save(filename, &pngClsid) == Gdiplus::Status::Ok)
			{
				bRet = TRUE;
			}
			delete pbmSrc;
		}
		//清理工作
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

	void _Bitmap::Save(const EString& fileName)
	{
		size_t pos = fileName.rfind(".");
		WCHAR format[15]{ L"image/bmp" };
		if (pos != EString::npos) {
			EString ext2 = fileName.substr(pos);
			ext2 = ext2.Tolower();
			do
			{
				if (ext2 == ".jpg") {
					StrCpyW(format, L"image/jpeg");
					break;
				}
				if (ext2 == ".png") {
					StrCpyW(format, L"image/png");
					break;
				}
				if (ext2 == ".gif") {
					StrCpyW(format, L"image/gif");
					break;
				}
				if (ext2 == ".tiff") {
					StrCpyW(format, L"image/tiff");
					break;
				}

			} while (false);
		}

		CLSID pngClsid;
		GetEncoderClsid(format, &pngClsid);
		__super::Save(fileName.utf16().c_str(), &pngClsid);
	}

};