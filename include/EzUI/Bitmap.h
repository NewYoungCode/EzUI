#pragma once
#include "EzUI.h"

namespace ezui {
	//BGRA 32位图
	class UI_EXPORT Bitmap {
	private:
		int m_width = 0;
		int m_height = 0;
		HBITMAP m_bmp = NULL;
		HDC m_hdc = NULL;
		byte* m_point = NULL;
		BITMAPINFO m_bmpInfo;
		Bitmap(const Bitmap& hBitmap) = delete;
		void operator=(const Bitmap& hBitmap) = delete;
	protected:
		void Create(int width, int height);
	public:
		int Width()const;
		int Height()const;
		//BGRA 32位图
		Bitmap(int width, int height);
		Bitmap(HDC dc, const Rect& rect);
		void SetPixel(int x, int y, const Color& color);
		Color GetPixel(int x, int y)const;
		byte* GetPixel();
		void Earse(const Rect& rect);//抹除矩形内容
		HBITMAP GetHBITMAP();
		HDC GetDC();
		bool Save(const UIString& fileName);
		Bitmap* Clone()const;
		virtual ~Bitmap();
	};
};
