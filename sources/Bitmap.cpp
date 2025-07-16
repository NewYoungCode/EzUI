#include "Bitmap.h"
namespace ezui {

	Bitmap::Bitmap(int_t width, int_t height, PixelFormat piexlFormat) {
		Create(width, height, piexlFormat);
	}
	Bitmap::Bitmap(HDC dc, const Rect& rect, PixelFormat piexlFormat) {
		Create(rect.Width, rect.Height, piexlFormat);
		::BitBlt(this->GetHDC(), 0, 0, rect.Width, rect.Height, dc, rect.X, rect.Y, SRCCOPY);
	}
	void Bitmap::Create(int_t width, int_t height, PixelFormat piexlFormat) {
		m_width = width;
		m_height = height;
		memset(&m_bmpInfo, 0, sizeof(m_bmpInfo));
		BITMAPINFOHEADER& bmih = m_bmpInfo.bmiHeader;
		bmih.biSize = sizeof(bmih);
		bmih.biBitCount = (byte)piexlFormat;
		bmih.biCompression = BI_RGB;
		bmih.biPlanes = 1;
		bmih.biWidth = width;
		bmih.biHeight = -height;
		bmih.biSizeImage = 0;
		m_bmp = ::CreateDIBSection(NULL, &m_bmpInfo, DIB_RGB_COLORS, (void**)&m_point, NULL, 0);
		this->GetHDC();
	}
	int_t Bitmap::Width() {
		return m_width;
	}
	int_t Bitmap::Height() {
		return m_height;
	}
	void Bitmap::SetPixel(int_t x, int_t y, const Color& color) {
		DWORD* point = (DWORD*)this->m_point + (x + y * this->Width());//起始地址+坐标偏移	
		if (m_bmpInfo.bmiHeader.biBitCount == 32) { //argb
			((BYTE*)point)[3] = color.GetA();//修改A通道数值
		}
		((BYTE*)point)[2] = color.GetR();//修改R通道数值
		((BYTE*)point)[1] = color.GetG();//修改G通道数值
		((BYTE*)point)[0] = color.GetB();//修改B通道数值
	}

	Color Bitmap::GetPixel(int_t x, int_t y) {
		DWORD* point = (DWORD*)this->m_point + (x + y * this->Width());//起始地址+坐标偏移
		BYTE a = 255, r, g, b;
		if (m_bmpInfo.bmiHeader.biBitCount == 32) { //argb
			a = ((BYTE*)point)[3];
		}
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
		for (int_t y = rect.Y; y < rect.GetBottom(); ++y)
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
	void Bitmap::Save(const UIString& fileName)
	{
		// 保存位图为BMP文件
		BITMAPINFOHEADER& bi = m_bmpInfo.bmiHeader;
		BITMAPFILEHEADER bmfh;
		bmfh.bfType = 0x4D42; // "BM"字节标记
		bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + Width() * Height() * (m_bmpInfo.bmiHeader.biBitCount / 8); // 文件大小
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // 位图数据偏移量
		//写入文件头
		std::ofstream ofs(fileName.unicode(), std::ios::binary);
		ofs.write((char*)&bmfh, sizeof(bmfh));
		ofs.write((char*)&bi, sizeof(bi));
		//获取像素数据 第五个参数传空可获取详细的BITMAPINFO
		auto ret = ::GetDIBits(this->GetHDC(), this->m_bmp, 0, this->Height(), NULL, &this->m_bmpInfo, DIB_RGB_COLORS);
		//写入像素数据
		BYTE* buffer = new BYTE[this->m_bmpInfo.bmiHeader.biSizeImage];
		ret = ::GetDIBits(this->GetHDC(), this->m_bmp, 0, this->Height(), buffer, &this->m_bmpInfo, DIB_RGB_COLORS);
		ofs.write((char*)buffer, this->m_bmpInfo.bmiHeader.biSizeImage);
		ofs.flush();
		ofs.close();
		delete[] buffer;
	}
	Bitmap::~Bitmap() {
		if (m_hdc) {
			::DeleteDC(m_hdc);
			::DeleteObject((HGDIOBJ)(HBITMAP)(m_bmp));
		}
	}
};
