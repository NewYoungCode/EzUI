#include "EBitmap.h"
namespace EzUI {

	EBitmap::EBitmap(int width, int height, PixelFormat piexlFormat) {
		Create(width, height, piexlFormat);
	}
	EBitmap::EBitmap(HDC dc, const Rect& rect, PixelFormat piexlFormat) {
		Create(rect.Width, rect.Height, piexlFormat);
		::BitBlt(this->GetHDC(), 0, 0, rect.Width, rect.Height, dc, rect.X, rect.Y, SRCCOPY);
	}
	void EBitmap::Create(int width, int height, PixelFormat piexlFormat) {
		(int)Width = width;
		(int)Height = height;
		memset(&_bmpInfo, 0, sizeof(_bmpInfo));
		BITMAPINFOHEADER& bmih = _bmpInfo.bmiHeader;
		bmih.biSize = sizeof(bmih);
		bmih.biBitCount = (byte)piexlFormat;
		bmih.biCompression = BI_RGB;
		bmih.biPlanes = 1;
		bmih.biWidth = width;
		bmih.biHeight = -height;
		bmih.biSizeImage = 0;
		_bmp = ::CreateDIBSection(NULL, &_bmpInfo, DIB_RGB_COLORS, (void**)&point, NULL, 0);
		this->GetHDC();
	}
	void EBitmap::SetPixel(int x, int y, const Color& color) {
		DWORD* point = (DWORD*)this->point + (x + y * this->Width);//��ʼ��ַ+����ƫ��	
		if (_bmpInfo.bmiHeader.biBitCount == 32) { //argb
			((BYTE*)point)[3] = color.GetA();//�޸�Aͨ����ֵ
		}
		((BYTE*)point)[2] = color.GetR();//�޸�Rͨ����ֵ
		((BYTE*)point)[1] = color.GetG();//�޸�Gͨ����ֵ
		((BYTE*)point)[0] = color.GetB();//�޸�Bͨ����ֵ
	}

	Color EBitmap::GetPixel(int x, int y) {
		DWORD* point = (DWORD*)this->point + (x + y * this->Width);//��ʼ��ַ+����ƫ��
		BYTE a = 255, r, g, b;
		if (_bmpInfo.bmiHeader.biBitCount == 32) { //argb
			a = ((BYTE*)point)[3];
		}
		r = ((BYTE*)point)[2];//�޸�Rͨ����ֵ
		g = ((BYTE*)point)[1];//�޸�Gͨ����ֵ
		b = ((BYTE*)point)[0];//�޸�Bͨ����ֵ
		return Color(r, g, b, a);
	}
	byte* EBitmap::GetPixel()
	{
		return (byte*)this->point;
	}
	void EBitmap::Earse(const Rect& _rect) {
		Rect rect = _rect;
		if (rect.X < 0) {
			rect.X = 0;
			rect.Width += rect.X;
		}
		if (rect.Y < 0) {
			rect.Y = 0;
			rect.Height += rect.Y;
		}
		if (rect.GetBottom() > Height) {
			rect.Height = this->Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = this->Width - rect.X;
		}
		for (int y = rect.Y; y < rect.GetBottom(); y++)
		{
			DWORD* point = (DWORD*)this->point + (rect.X + y * this->Width);//��ʼ��ַ+����ƫ��
			::memset(point, 0, rect.Width * 4);//Ĩ��
		}
	}
	HBITMAP EBitmap::GetHBITMAP()
	{
		return this->_bmp;
	}
	HDC& EBitmap::GetHDC() {
		if (!_hdc) {
			_hdc = ::CreateCompatibleDC(NULL);
			::SelectObject(_hdc, _bmp);
		}
		return _hdc;
	}
	void EBitmap::Save(const EString& fileName)
	{
		// ����λͼΪBMP�ļ�
		BITMAPINFOHEADER& bi = _bmpInfo.bmiHeader;
		BITMAPFILEHEADER bmfh;
		bmfh.bfType = 0x4D42; // "BM"�ֽڱ��
		bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + Width * Height * (_bmpInfo.bmiHeader.biBitCount / 8); // �ļ���С
		bmfh.bfReserved1 = 0;
		bmfh.bfReserved2 = 0;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); // λͼ����ƫ����
		//д���ļ�ͷ
		std::ofstream ofs(fileName.utf16(), std::ios::binary);
		ofs.write((char*)&bmfh, sizeof(bmfh));
		ofs.write((char*)&bi, sizeof(bi));
		//��ȡ�������� ������������տɻ�ȡ��ϸ��BITMAPINFO
		auto ret = ::GetDIBits(this->GetHDC(), this->_bmp, 0, this->Height, NULL, &this->_bmpInfo, DIB_RGB_COLORS);
		//д����������
		BYTE* buffer = new BYTE[this->_bmpInfo.bmiHeader.biSizeImage];
		ret = ::GetDIBits(this->GetHDC(), this->_bmp, 0, this->Height, buffer, &this->_bmpInfo, DIB_RGB_COLORS);
		ofs.write((char*)buffer, this->_bmpInfo.bmiHeader.biSizeImage);
		ofs.flush();
		ofs.close();
		delete[] buffer;
	}
	EBitmap::~EBitmap() {
		if (_hdc) {
			::DeleteDC(_hdc);
			::DeleteBitmap(_bmp);
		}
	}
};
