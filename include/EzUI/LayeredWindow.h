#pragma once
#include "BorderlessWindow.h"
#include "Bitmap.h"
#include "Task.h"
#include "Timer.h"

namespace ezui {
	/// <summary>
	/// //LayeredWindow	  //无边框  带阴影 窗口透明异形 窗口大小发生改变重绘
	/// </summary>
	class  UI_EXPORT LayeredWindow :public BorderlessWindow
	{
	private:
		Timer m_timeOut;
		std::list<Rect> m_invalidateRect;
		Bitmap* m_winBitmap = NULL;
		void UpdateLayeredWindow(HDC hdc);
		void BeginPaint(Rect* rect);
		void EndPaint();
		void Paint();
	protected:
		virtual void OnSize(const Size& sz)override;
		void InvalidateRect(const Rect& rect);
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		//窗口透明度
		float Opacity = 1.0f;
		LayeredWindow(int_t width, int_t height, HWND owner = NULL);
		virtual ~LayeredWindow();
	};
};
