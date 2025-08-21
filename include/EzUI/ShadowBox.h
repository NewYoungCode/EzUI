#pragma once
#include "Window.h"
#include "Bitmap.h"

namespace ezui {
	class UI_EXPORT ShadowBox
	{
	private:
		Size m_lastSize;
		Bitmap* m_bufBitmap = NULL;
		HWND m_hWnd = NULL;
		HWND m_mainHWnd = NULL;
		WORD m_radius = 0;
		WindowData* m_publicData = NULL;
	private:
		void SetAplpha(int x, int y, BYTE a, float radius);
		bool SetShadow(int m_Width, int m_Height, int iSize, float radius);
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		ShadowBox(int width, int height, HWND mainHwnd);//构造函数
		virtual ~ShadowBox();
		//在父窗口发生改变的时候更新阴影区域
		virtual void Update(int shadowMargin, int radius);
		const HWND Hwnd();
	};
};