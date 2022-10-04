#include "GdiplusRender.h"
#if USED_GDIPLUS
#pragma comment (lib, "usp10.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib, "gdiplus.lib")
namespace EzUI {

	int Dpi = 0;
	ULONG_PTR _gdiplusToken = NULL;

	Gdiplus::SolidBrush* CreateBrush(const __Color& color) {
		Gdiplus::SolidBrush* _bufBrush = new Gdiplus::SolidBrush(color.GetValue());
		return _bufBrush;
	}
	Gdiplus::Pen* CreatePen(const __Color& color, int width) {
		Gdiplus::Pen* pen = new Gdiplus::Pen(color.GetValue(), (float)width);
		return pen;
	}

	void RenderInitialize()
	{
		Dpi = ::GetDpiForSystem();
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
		//graphics->SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);//文字
		//graphics->SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);//图像
	}
	void CreateRectangle(Gdiplus::GraphicsPath& path, const  __Rect& rect, int radius)
	{

		if (radius <= 0) {
			path.AddRectangle(Gdiplus::Rect{ rect.X,rect.Y,rect.Width,rect.Height });
			path.CloseFigure();
			return;
		}
		int diameter = radius;
		Gdiplus::Rect arcRect(rect.X, rect.Y, diameter, diameter);// = new Rect(rect.Location, new Size(diameter, diameter));
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
	GdiplusRender::GdiplusRender(HDC hdc, int Width, int Height)
	{
		this->DC = hdc;
		base = new  Gdiplus::Graphics(hdc);
		HighQualityMode(base);
		
	}
	GdiplusRender::GdiplusRender(HWND hWnd)
	{
		base = new  Gdiplus::Graphics(hWnd);
		HighQualityMode(base);
	}
	GdiplusRender::GdiplusRender(Gdiplus::Image* image)
	{
		base = new  Gdiplus::Graphics(image);
		HighQualityMode(base);
	}
	GdiplusRender::~GdiplusRender()
	{
		for (auto& it : CacheFont) {
			DeleteObject(it.second);
		}
		CacheFont.clear();

		if (this->SolidBrush) {
			delete this->SolidBrush;
		}

		delete base;
	}
	void GdiplusRender::DrawTextLayout(const __Point& pt, TextLayout* textLayout, const __Color& color) {
		__Rect rect{ pt.X,pt.Y,textLayout->maxSize.Width,textLayout->maxSize.Height };
		this->DrawString(*textLayout->text, textLayout->textFormat->fontFamilly, textLayout->textFormat->fontSize, color, rect, textLayout->textFormat->textAlign);
	}
	void GdiplusRender::DrawRectangle(const __Rect& _rect, const __Color& color, int width, int radius)
	{
		if (color.GetA() == 0) {
			return;
		}
		__Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		Gdiplus::Pen pen(GetSolidBrush(color), (float)width);
		if (radius > 0) {
			Gdiplus::GraphicsPath path;
			CreateRectangle(path, rect, radius);
			base->DrawPath(&pen, &path);
		}
		else {
			base->DrawRectangle(&pen, ToRect(rect));
		}
	}

	Gdiplus::SolidBrush* GdiplusRender::GetSolidBrush(const __Color& color) {
		if (this->SolidBrush == NULL) {
			this->SolidBrush = new Gdiplus::SolidBrush(ToColor(color));
		}
		else {
			this->SolidBrush->SetColor(ToColor(color));
		}
		return this->SolidBrush;
	}

	void GdiplusRender::FillRectangle(const __Rect& _rect, const __Color& color, int radius)
	{
		if (color.GetValue() == 0) {
			return;
		}
		__Rect rect = _rect;
		rect.X += OffsetX;
		rect.Y += OffsetY;

		auto brush = GetSolidBrush(color);
		if (radius > 0) {
			Gdiplus::GraphicsPath path;
			CreateRectangle(path, rect, radius);
			base->FillPath(brush, &path);
		}
		else {
			base->FillRectangle(brush, ToRect(rect));
		}

	}
	void GdiplusRender::DrawString(const std::wstring& text, const std::wstring& fontFamily, int fontSize, const __Color& color, const __Rect& _rect, TextAlign textAlign, bool underLine)
	{
		__Rect rect(_rect.X, _rect.Y, _rect.Width, _rect.Height);
		rect.X += OffsetX;
		rect.Y += OffsetY;

		HDC DC = base->GetHDC();
		//设定裁剪区域
		HRGN clip = NULL;
		if (!Layers.empty()) {
			if (!Layers.empty()) {
				__Rect& it = *(Layers.back());
				clip = ::CreateRectRgn(it.X, it.Y, it.GetRight(), it.GetBottom());
				SelectClipRgn(DC, clip);
			}
		}
		//设定基本参数
		int lastMode=::SetBkMode(DC, TRANSPARENT);
		HGDIOBJ oldFont = SelectFont(DC, CreateSafeFont(fontFamily, fontSize, DC, underLine));
		//绘制文字
		RECT winRECT = rect.WinRECT();
		SetTextColor(DC, RGB(color.GetR(), color.GetG(), color.GetB()));

#define __Top  DT_TOP
#define __Bottom  DT_BOTTOM
#define __Left  DT_LEFT
#define __Right  DT_RIGHT
#define __Middle   DT_VCENTER
#define __Center   DT_CENTER

		int textFormat = 0;
		do
		{
			if (textAlign == TextAlign::BottomCenter) {
				textFormat = __Bottom | __Center;
				break;
			}
			if (textAlign == TextAlign::BottomLeft) {
				textFormat = __Bottom | __Left;
				break;
			}
			if (textAlign == TextAlign::BottomRight) {
				textFormat = __Bottom | __Right;
				break;
			}
			if (textAlign == TextAlign::MiddleCenter) {
				textFormat = __Middle | __Center;
				break;
			}
			if (textAlign == TextAlign::MiddleLeft) {
				textFormat = __Middle | __Left;
				break;
			}
			if (textAlign == TextAlign::MiddleRight) {
				textFormat = __Middle | __Right;
				break;
			}
			if (textAlign == TextAlign::TopCenter) {
				textFormat = __Top | __Center;
				break;
			}
			if (textAlign == TextAlign::TopLeft) {
				textFormat = __Top | __Left;
				break;
			}
			if (textAlign == TextAlign::TopRight) {
				textFormat = __Top | __Right;
				break;
			}
		} while (0);

		DrawTextW(DC, text.c_str(), text.size(), &winRECT, DT_SINGLELINE | textFormat);
		//清理工作
		SelectFont(DC, oldFont);
		::SetBkMode(DC, lastMode);
		if (clip) {
			SelectClipRgn(DC, NULL);
			DeleteObject(clip);
		}
		base->ReleaseHDC(DC);
	}
	void GdiplusRender::PushAxisAlignedClip(const __Rect& rect, ClipMode clipMode)
	{
		Layers.push_back((__Rect*)&rect);
		base->SetClip(ToRect(rect), (Gdiplus::CombineMode)clipMode);
	}
	void GdiplusRender::PopAxisAlignedClip()
	{
		base->ResetClip();
		if (!Layers.empty()) {
			Layers.pop_back();
			if (!Layers.empty()) {
				__Rect& it = *(Layers.back());
				base->SetClip(ToRect(it), (Gdiplus::CombineMode)ClipMode::Valid);
			}
		}
	}
	void GdiplusRender::FillGeometry(const Geometry& geometry, const  __Color& color) {
		auto brush = GetSolidBrush(color);
		base->FillRegion(brush, geometry.rgn);
	}
	HFONT GdiplusRender::CreateSafeFont(const std::wstring& fontFamily, int fontSize, HDC DC, bool lfUnderline)
	{
		WCHAR key[LF_FACESIZE + 10]{ 0 };
		swprintf_s(key, L"%s_%d_%d", fontFamily.c_str(), fontSize, lfUnderline);
		auto itor = CacheFont.find(key);
		if (itor != CacheFont.end()) {
			return (*itor).second;//从缓存中返回TextFormat
		}
		LOGFONTW lf{ 0 };
		GetObjectW(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONTW), &lf);
		wcsncpy_s(lf.lfFaceName, fontFamily.c_str(), LF_FACESIZE);
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfHeight = -MulDiv(fontSize, Dpi, 72);
		//lf.lfWeight += FW_BOLD; //粗体
		lf.lfUnderline = lfUnderline; //下划线
		//lf.lfItalic = TRUE; //斜体
		HFONT hFont = CreateFontIndirectW(&lf);
		CacheFont.insert(std::pair<std::wstring, HFONT>(key, hFont));//加入缓存
		return hFont;
	}
	void GdiplusRender::DrawLine(const __Color& color, const __Point& _A, const  __Point& _B, int width)
	{
		__Point A = _A;
		A.X += OffsetX;
		A.Y += OffsetY;
		__Point B = _B;
		B.X += OffsetX;
		B.Y += OffsetY;

		Gdiplus::Pen pen(GetSolidBrush(color), (float)width);
		base->DrawLine(&pen, A.X, A.Y, B.X, B.Y);
	}
	void GdiplusRender::DrawImage(IImage* _image, const __Rect& destRect, const __Rect& srcRect)
	{
		GdiplusImage* image = (GdiplusImage*)_image;
		base->DrawImage(image, ToRect(destRect), srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height, Gdiplus::Unit::UnitPixel);
	}
	void GdiplusRender::DrawImage(IImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode, const Margin& margin)
	{
		GdiplusImage* image = (GdiplusImage*)_image;

		if (!image || image->GetLastStatus() != Gdiplus::Status::Ok) return;
		__Rect rect = _rect;
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
				__Size sz{ clientWidth,(INT)zoomHeight };
				int y = (clientHeight - sz.Height) / 2 + rect.Y;
				base->DrawImage(image, Gdiplus::Rect{ rect.X  ,y, sz.Width, sz.Height });
			}
			else {
				double zoomWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;
				__Size sz{ (INT)zoomWidth,clientHeight };
				int x = (clientWidth - sz.Width) / 2 + rect.X;
				base->DrawImage(image, Gdiplus::Rect{ x  , rect.Y, sz.Width, sz.Height });
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
				base->DrawImage(image, Gdiplus::Rect{ rect.X - x,rect.Y,zoomWidth,clientHeight });
			}
			else {
				//1000 600 客户端
				//400  600 图片
				//1000 1500     缩放后的图片大小 
				int zoomHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;//图片应该这么高才对
				int y = (zoomHeight - clientHeight) * 1.0 / 2 + 0.5;
				base->DrawImage(image, Gdiplus::Rect{ rect.X,  rect.Y - y  , clientWidth, zoomHeight });
			}
			return;
		}

		if (imageSizeMode == ImageSizeMode::StretchImage || true) {


			base->DrawImage(image, ToRect(rect));
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
	BOOL SaveHDCToFile(HDC hDC, const __Rect& rect, const std::wstring& fileName) {
		size_t pos = fileName.rfind(L".");
		WCHAR format[15]{ L"image/bmp" };
		if (pos != size_t(-1)) {
			std::wstring ext2 = fileName.substr(pos);
			do
			{
				if (ext2 == L".jpg") {
					StrCpyW(format, L"image/jpeg");
					break;
				}
				if (ext2 == L".png") {
					StrCpyW(format, L"image/png");
					break;
				}
				if (ext2 == L".gif") {
					StrCpyW(format, L"image/gif");
					break;
				}
				if (ext2 == L".tiff") {
					StrCpyW(format, L"image/tiff");
					break;
				}
			} while (false);
		}
		CLSID pngClsid;
		GetEncoderClsid(format, &pngClsid);
		RECT r = rect.WinRECT();
		return SaveHDCToFile(hDC, &r, format, fileName.c_str());
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
	GdiplusImage* ClipImage(GdiplusImage* img, const __Size& sz, int _radius)
	{
		GdiplusImage* bitmap = new  GdiplusImage(sz.Width, sz.Height);
		Gdiplus::Graphics g(bitmap);
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);//抗锯齿
		g.SetPixelOffsetMode(Gdiplus::PixelOffsetMode::PixelOffsetModeHalf);//像素偏移模式
		Gdiplus::TextureBrush tb(img);
		Gdiplus::GraphicsPath path;
		CreateRectangle(path, __Rect{ 0,0,sz.Width, sz.Height }, _radius);
		g.FillPath(&tb, &path);
		return bitmap;
	}
	void GdiplusRender::SaveImage(const WCHAR* format, const WCHAR* fileName, const __Size& size)
	{
		/*	if (DC) {
				::DeleteFileW(fileName);
				RECT rect{ 0,0,size.Width,size.Height };
				SaveHDCToFile(DC, &rect, format, fileName);
			}*/
	}
	void GdiplusRender::BeginDraw()
	{
		//base->BeginContainer();
	}
	void GdiplusRender::EndDraw()
	{
		base->Flush();
	}
	void GdiplusImage::Save(const std::wstring& fileName)
	{
		size_t pos = fileName.rfind(L".");
		WCHAR format[15]{ L"image/bmp" };
		if (pos != size_t(-1)) {
			std::wstring ext2 = fileName.substr(pos);
			//std::wstring = ext2.Tolower();
			do
			{
				if (ext2 == L".jpg") {
					StrCpyW(format, L"image/jpeg");
					break;
				}
				if (ext2 == L".png") {
					StrCpyW(format, L"image/png");
					break;
				}
				if (ext2 == L".gif") {
					StrCpyW(format, L"image/gif");
					break;
				}
				if (ext2 == L".tiff") {
					StrCpyW(format, L"image/tiff");
					break;
				}

			} while (false);
		}

		CLSID pngClsid;
		GetEncoderClsid(format, &pngClsid);
		__super::Save(fileName.c_str(), &pngClsid);
	}
};
#endif