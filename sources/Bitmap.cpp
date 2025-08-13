#include "Bitmap.h"
#include <gdiplus.h>
namespace ezui {
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0) {
			return -1;  // Failure
		}

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
		if (pImageCodecInfo == NULL) {
			return -1;  // Failure
		}

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

	Bitmap::Bitmap(int width, int height) {
		Create(width, height);
	}
	Bitmap::Bitmap(HDC dc, const Rect& rect) {
		Create(rect.Width, rect.Height);
		::BitBlt(this->GetHDC(), 0, 0, rect.Width, rect.Height, dc, rect.X, rect.Y, SRCCOPY);
	}
	void Bitmap::Create(int width, int height) {
		m_width = width;
		m_height = height;
		memset(&m_bmpInfo, 0, sizeof(m_bmpInfo));
		BITMAPINFOHEADER& bmih = m_bmpInfo.bmiHeader;
		bmih.biSize = sizeof(bmih);
		bmih.biBitCount = 32;
		bmih.biCompression = BI_RGB;
		bmih.biPlanes = 1;
		bmih.biWidth = width;
		bmih.biHeight = -height;
		bmih.biSizeImage = width * height * 4;
		m_bmp = ::CreateDIBSection(NULL, &m_bmpInfo, DIB_RGB_COLORS, (void**)&m_point, NULL, 0);
		::memset(m_point, 0, bmih.biSizeImage);
		this->GetHDC();
	}
	int Bitmap::Width()const {
		return m_width;
	}
	int Bitmap::Height() const {
		return m_height;
	}
	void Bitmap::SetPixel(int x, int y, const Color& color) {
		DWORD* point = (DWORD*)this->m_point + (x + y * this->Width());//起始地址+坐标偏移	

		((BYTE*)point)[3] = color.GetA();//修改A通道数值
		((BYTE*)point)[2] = color.GetR();//修改R通道数值
		((BYTE*)point)[1] = color.GetG();//修改G通道数值
		((BYTE*)point)[0] = color.GetB();//修改B通道数值
	}

	Color Bitmap::GetPixel(int x, int y) const {
		DWORD* point = (DWORD*)this->m_point + (x + y * this->Width());//起始地址+坐标偏移
		BYTE a, r, g, b;
		a = ((BYTE*)point)[3];
		r = ((BYTE*)point)[2];//修改R通道数值
		g = ((BYTE*)point)[1];//修改G通道数值
		b = ((BYTE*)point)[0];//修改B通道数值
		return Color(r, g, b, a);
	}
	byte* Bitmap::GetPixel()
	{
		return (byte*)this->m_point;
	}
	void Bitmap::Earse(const Rect& _rect) {
		Rect rect = _rect;
		if (rect.X < 0) {
			rect.X = 0;
			rect.Width += rect.X;
		}
		if (rect.Y < 0) {
			rect.Y = 0;
			rect.Height += rect.Y;
		}
		if (rect.GetBottom() > Height()) {
			rect.Height = this->Height() - rect.Y;
		}
		if (rect.GetRight() > Width()) {
			rect.Width = this->Width() - rect.X;
		}
		for (int y = rect.Y; y < rect.GetBottom(); ++y)
		{
			DWORD* point = (DWORD*)this->m_point + (rect.X + y * this->Width());//起始地址+坐标偏移
			::memset(point, 0, rect.Width * 4);//抹除
		}
	}
	HBITMAP Bitmap::GetHBITMAP()
	{
		return this->m_bmp;
	}
	HDC Bitmap::GetHDC() {
		if (!m_hdc) {
			m_hdc = ::CreateCompatibleDC(NULL);
			::SelectObject(m_hdc, m_bmp);
		}
		return m_hdc;
	}
	bool Bitmap::Save(const UIString& fileName)
	{
		size_t pos = fileName.rfind(".");
		UIString ext;
		if (pos != size_t(-1)) {
			ext = fileName.substr(pos);
			ext = ext.toLower();
		}

		bool ret = false;
		Gdiplus::Bitmap* pbmSrc = NULL;
		do
		{
			CLSID pngClsid;
			int code = -1;
			if (ext == ".png") {
				code = GetEncoderClsid(L"image/png", &pngClsid);
			}
			else if (ext == ".jpg" || ext == ".jpeg") {
				code = GetEncoderClsid(L"image/jpeg", &pngClsid);
			}
			else if (ext == ".tiff") {
				code = GetEncoderClsid(L"image/tiff", &pngClsid);
			}
			else {
				code = GetEncoderClsid(L"image/bmp", &pngClsid);
			}
			if (code == -1) {
				break;
			}
			pbmSrc = new Gdiplus::Bitmap(Width(), Height(), Width() * 4, PixelFormat32bppARGB, (BYTE*)GetPixel());
			if (pbmSrc && pbmSrc->Save(fileName.unicode().c_str(), &pngClsid) == Gdiplus::Status::Ok)
			{
				ret = true;
				break;
			}
		} while (false);
		if (pbmSrc) {
			delete pbmSrc;
		}
		return ret;
	}

	Bitmap* Bitmap::Clone() const {
		if (m_width <= 0 || m_height <= 0) {
			return NULL;
		}
		// 创建新 Bitmap 对象（保留格式）
		Bitmap* clone = new Bitmap(m_width, m_height);
		//拷贝像素
		memcpy(clone->m_point, m_point, m_width * m_height * 4);
		return clone;
	}

	Bitmap::~Bitmap() {
		if (m_hdc) {
			::DeleteDC(m_hdc);
			::DeleteObject((HGDIOBJ)(HBITMAP)(m_bmp));
		}
	}
};
