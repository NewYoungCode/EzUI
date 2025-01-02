#pragma once
#include "EzUI.h"

namespace EzUI {
	class UI_EXPORT Bitmap {
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
		Bitmap(const Bitmap& hBitmap) = delete;
		void operator=(const Bitmap& hBitmap) = delete;
	protected:
		void Create(int width, int height, PixelFormat piexlFormat);
	public:
		const int Width = 0;
		const int Height = 0;
		Bitmap(int width, int height, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		Bitmap(HDC dc, const Rect& rect, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		void SetPixel(int x, int y, const Color& color);
		Color GetPixel(int x, int y);
		byte* GetPixel();
		void Earse(const Rect& rect);//抹除矩形内容
		HBITMAP GetHBITMAP();
		HDC GetHDC();
		void Save(const EString& fileName);
		virtual ~Bitmap();
	};
};
