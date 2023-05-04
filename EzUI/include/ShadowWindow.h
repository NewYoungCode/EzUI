#pragma once
#include "Window.h"
#define SHADOWWIDTH 11

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
		ShadowWindow(int cx, int cy, HWND OwnerWnd);//���캯��
		virtual ~ShadowWindow();
		virtual void Update(int _marginWidth);//�ڸ����ڷ����ı��ʱ�������Ӱ����
	};
};