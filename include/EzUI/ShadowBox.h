#pragma once
#include "Window.h"
#include "Bitmap.h"

namespace EzUI {
	class UI_EXPORT ShadowBox
	{
	private:
		Size _bufSize;
		Bitmap* _bufBitmap = NULL;
		Rect _clipRect;
		HWND _hWnd = NULL;
	private:
		void setA(int_t x, int_t y, BYTE a, float radius);
		bool SetShadow(int_t m_Width, int_t m_Height, int_t iSize, float radius);
	public:
		ShadowBox(int_t width, int_t height, HWND OwnerWnd);//构造函数
		virtual ~ShadowBox();
		//在父窗口发生改变的时候更新阴影区域
		virtual void Update(int_t _marginWidth, const Border& border);
		const HWND Hwnd();
	};
};