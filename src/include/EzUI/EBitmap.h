#pragma once
#include "EzUI.h"

namespace EzUI {
	class UI_EXPORT EBitmap {
	public:
		enum class PixelFormat :int {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		HBITMAP _bmp = NULL;
		HDC _hdc = NULL;
		byte* point = NULL;
		BITMAPINFO _bmpInfo;
		EBitmap(const EBitmap& hBitmap) = delete;
		void operator=(const EBitmap& hBitmap) = delete;
	protected:
		void Create(int width, int height, PixelFormat piexlFormat);
	public:
		const int Width = 0;
		const int Height = 0;
		EBitmap(int width, int height, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		EBitmap(HDC dc, const Rect& rect, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		void SetPixel(int x, int y, const Color& color);
		Color GetPixel(int x, int y);
		byte* GetPixel();
		void Earse(const Rect& rect);//Ä¨³ý¾ØÐÎÄÚÈÝ
		HBITMAP GetHBITMAP();
		HDC& GetHDC();
		void Save(const EString& fileName);
		virtual ~EBitmap();
	};
};
