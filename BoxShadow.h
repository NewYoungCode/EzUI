#pragma once
#include "Window.h"
class UI_EXPORT BoxShadow
{
private:
	HWND _hWnd = NULL;
	Size _bufSize;
	EBitmap*  _bufBitmap = NULL;
	Rect clipRect;
private:
	void setA(int x, int y, BYTE A);
	bool SetShadow(int m_Width, int m_Height, size_t iSize);
public:
	BoxShadow(int cx, int cy, HWND OwnerWnd);//构造函数
	virtual ~BoxShadow();
	Image *BackgroundImage = NULL;//用于做异性窗口
	virtual void Update(int _marginWidth);//在父窗口发生改变的时候更新阴影区域
};
