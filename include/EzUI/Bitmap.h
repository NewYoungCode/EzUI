#pragma once
#include "EzUI.h"

namespace ezui {
	class UI_EXPORT Bitmap {
	public:
		enum class PixelFormat :int_t {
			BGR = 24,//不透明BGR位图
			BGRA = 32//带透明通道但是RGB数值未预乘A值的位图
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
		int_t Width()const;
		int_t Height()const;
		Bitmap(int_t width, int_t height, PixelFormat piexlFormat = PixelFormat::BGR);
		Bitmap(HDC dc, const Rect& rect, PixelFormat piexlFormat = PixelFormat::BGR);
		void SetPixel(int_t x, int_t y, const Color& color);
		Color GetPixel(int_t x, int_t y)const;
		byte* GetPixel();
		void Earse(const Rect& rect);//抹除矩形内容
		HBITMAP GetHBITMAP();
		HDC GetHDC();
		void Save(const UIString& fileName);
		Bitmap* Clone()const;
		virtual ~Bitmap();
	};
};
