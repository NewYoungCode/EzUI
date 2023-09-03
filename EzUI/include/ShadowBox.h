#pragma once
#include "Window.h"

namespace EzUI {
	class UI_EXPORT ShadowBox
	{
	private:
		Size _bufSize;
		EBitmap* _bufBitmap = NULL;
		Rect clipRect;
		HWND _hWnd = NULL;
	private:
		void setA(const int& x,const int& y,const BYTE& A);
		bool SetShadow(int m_Width, int m_Height, size_t iSize);
	public:
		ShadowBox(int width, int height, HWND OwnerWnd);//���캯��
		virtual ~ShadowBox();
		virtual void Update(int _marginWidth);//�ڸ����ڷ����ı��ʱ�������Ӱ����
		const HWND& Hwnd();
	};
};