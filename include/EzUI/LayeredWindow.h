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
		std::list<Rect> m_invalidateRect;
		Bitmap* m_winBitmap = NULL;
		void UpdateLayeredWindow(HDC hdc, const Rect& rePaintRect);
		void BeginPaint(std::vector<Rect>* outRect);
		void EndPaint();
		bool Paint();
	protected:
		virtual void OnSize(const Size& sz)override;
		void InvalidateRect(const Rect& rect);
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		//窗口透明度
		float Opacity = 1.0f;
		LayeredWindow(int width, int height, HWND owner = NULL, DWORD dwStyle = NULL, DWORD dwExStyle = NULL);
		virtual ~LayeredWindow();
	};
};
