#pragma once
#include "Window.h"
#include "Bitmap.h"

namespace ezui {
	class UI_EXPORT ShadowBox
	{
	private:
		Size m_bufSize;
		Bitmap* m_bufBitmap = NULL;
		Rect m_clipRect;
		HWND m_hWnd = NULL;
		WORD m_borderWidth = 0;
	private:
		void setA(int_t x, int_t y, BYTE a, float radius);
		bool SetShadow(int_t m_Width, int_t m_Height, int_t iSize, float radius);
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		//公共数据 请不要改动此变量
		WindowData* PublicData = NULL;
	public:
		ShadowBox(int_t width, int_t height, HWND OwnerWnd);//构造函数
		virtual ~ShadowBox();
		//在父窗口发生改变的时候更新阴影区域
		virtual void Update(int_t _marginWidth, const Border& border);
		const HWND Hwnd();
	};
};