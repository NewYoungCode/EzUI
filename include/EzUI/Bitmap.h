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
		uint8_t* m_point = NULL;
		BITMAPINFO& m_bmpInfo;
	private:
		Bitmap(const Bitmap&) = delete;            // 禁止拷贝构造
		Bitmap& operator=(const Bitmap&) = delete; // 禁止拷贝赋值
		Bitmap(Bitmap&&) = delete;                 // 禁止移动构造
		Bitmap& operator=(Bitmap&&) = delete;      // 禁止移动赋值
	protected:
		void Create(int width, int height, bool zeroFill = false);
	public:
		int Width()const;
		int Height()const;
		/// <summary>
		/// //BGRA 32位图
		/// </summary>
		/// <param name="width">位图的宽</param>
		/// <param name="height">位图的高</param>
		Bitmap(int width, int height);
		Bitmap(HDC dc, const Rect& rect);
		void SetPixel(int x, int y, const Color& color);
		Color GetPixel(int x, int y)const;
		uint8_t* GetPixel();
		void Earse(const Rect& rect);//抹除矩形内容
		HBITMAP GetHBITMAP();
		HDC GetDC();
		bool Save(const UIString& fileName);
		Bitmap* Clone()const;
		virtual ~Bitmap();
	};
};
