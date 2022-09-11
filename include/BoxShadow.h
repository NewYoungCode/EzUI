#pragma once
#include "Window.h"
#define SHADOWWIDTH 15
namespace EzUI {
	class UI_EXPORT BoxShadow
	{
	private:
		Size _bufSize;
		EBitmap* _bufBitmap = NULL;
		Rect clipRect;
	public:
		HWND _hWnd = NULL;
	private:
		void setA(int x, int y, BYTE A);
		bool SetShadow(int m_Width, int m_Height, size_t iSize);
	public:
		BoxShadow(int cx, int cy, HWND OwnerWnd);//���캯��
		virtual ~BoxShadow();
		Image* BackgroundImage = NULL;//���������Դ���
		virtual void Update(int _marginWidth);//�ڸ����ڷ����ı��ʱ�������Ӱ����
	};
};