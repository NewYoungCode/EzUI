#pragma once
#include "Window.h"
#include "Bitmap.h"

namespace ezui {
	class UI_EXPORT ShadowBox
	{
	private:
		Size m_lastSize;
		int m_lastShadowMargin = 0;
		Bitmap* m_bufBitmap = NULL;
		HWND m_hWnd = NULL;
		HWND m_mainHWnd = NULL;
		WORD m_radius = 0;
		WindowContext* m_publicData = NULL;
		//窗口透明度
		float m_opacity = 0.0f;
	private:
		bool SetShadow(Bitmap* bitmap, int iSize, float radius);
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void OnSize(const Size& sz);
	public:
		ShadowBox(int width, int height, HWND mainHwnd);//构造函数
		virtual ~ShadowBox();
		//在父窗口发生改变的时候更新阴影区域
		virtual void Update(int shadowMargin, int radius);
		//更新透明度
		void Update(float opacity);
		HWND Hwnd();
		HDC GetDC();
	};
};