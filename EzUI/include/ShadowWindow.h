#pragma once
#include "Window.h"

namespace EzUI {
	class UI_EXPORT ShadowWindow
	{
	private:
		Size _bufSize;
		EBitmap* _bufBitmap = NULL;
		Rect clipRect;
	public:
		HWND _hWnd = NULL;
	private:
		void setA(const int& x,const int& y,const BYTE& A);
		bool SetShadow(int m_Width, int m_Height, size_t iSize);
	public:
		ShadowWindow(int width, int height, HWND OwnerWnd);//构造函数
		virtual ~ShadowWindow();
		virtual void Update(int _marginWidth);//在父窗口发生改变的时候更新阴影区域
	};
};