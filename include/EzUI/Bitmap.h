#pragma once
#include "EzUI.h"

namespace ezui {
	class UI_EXPORT Bitmap {
	public:
		enum class PixelFormat :int_t {
			PixelFormatRGB = 24,
			PixelFormatARGB = 32
		};
	private:
		int_t m_width = 0;
		int_t m_height = 0;
		HBITMAP m_bmp = NULL;
		HDC m_hdc = NULL;
		byte* m_point = NULL;
		BITMAPINFO m_bmpInfo;
		Bitmap(const Bitmap& hBitmap) = delete;
		void operator=(const Bitmap& hBitmap) = delete;
	protected:
		void Create(int_t width, int_t height, PixelFormat piexlFormat);
	public:
		int_t Width();
		int_t Height();
		Bitmap(int_t width, int_t height, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		Bitmap(HDC dc, const Rect& rect, PixelFormat piexlFormat = PixelFormat::PixelFormatRGB);
		void SetPixel(int_t x, int_t y, const Color& color);
		Color GetPixel(int_t x, int_t y);
		byte* GetPixel();
		void Earse(const Rect& rect);//抹除矩形内容
		HBITMAP GetHBITMAP();
		HDC GetHDC();
		void Save(const UIString& fileName);
		virtual ~Bitmap();
	};
};
