#pragma once
#include "EzUI.h"
#include "CPURender.h"
namespace EzUI {


	struct EBitmap {
	private:
		HDC _hdc = NULL;
		HGDIOBJ _hgdiobj = NULL;
		EBitmap(const EBitmap& hBitmap) {}
		EBitmap operator=(const EBitmap& hBitmap) {}
	public:
		HBITMAP _bitmap = NULL;
		WORD Width;
		WORD Height;
		void* point = NULL;
		EBitmap(WORD width, WORD height, BYTE bitCount = 24) {//默认24位不透明位图
			this->Width = width;
			this->Height = height;
			BITMAPINFO bmi;
			memset(&bmi, 0, sizeof(BITMAPINFO));
			BITMAPINFOHEADER& bmih = bmi.bmiHeader;
			bmih.biSize = sizeof(BITMAPINFOHEADER);
			bmih.biBitCount = bitCount;
			bmih.biCompression = BI_RGB;
			bmih.biPlanes = 1;
			bmih.biWidth = width;
			bmih.biHeight = height;
			bmih.biSizeImage = width * height * bitCount;
			_bitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &point, NULL, 0);
		}
		HDC& GetHDC() {
			if (!_hdc) {
				_hdc = ::CreateCompatibleDC(NULL);
				_hgdiobj = ::SelectObject(_hdc, _bitmap);
			}
			return _hdc;
		}
		virtual ~EBitmap() {
			if (_hdc) {
				::SelectObject(_hdc, _hgdiobj);
				::DeleteDC(_hdc);
				::DeleteBitmap(_bitmap);
			}
		}
	};



	using Painter = CPURender;

};