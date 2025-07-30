#pragma once
#include "EzUI.h"

namespace ezui {
	//BGRA 32位图
	class UI_EXPORT Bitmap {
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
		void Create(int_t width, int_t height);
	public:
		int_t Width()const;
		int_t Height()const;
		//BGRA 32位图
		Bitmap(int_t width, int_t height);
		Bitmap(HDC dc, const Rect& rect);
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
