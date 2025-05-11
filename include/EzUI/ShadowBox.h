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
		void setA(int x, int y, BYTE a, float radius);
		bool SetShadow(int m_Width, int m_Height, size_t iSize, float radius);
	public:
		ShadowBox(int width, int height, HWND OwnerWnd);//构造函数
		virtual ~ShadowBox();
		//在父窗口发生改变的时候更新阴影区域
		virtual void Update(int _marginWidth, float radius);
		const HWND Hwnd();
	};
};