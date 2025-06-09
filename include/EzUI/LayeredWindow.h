#pragma once
#include "BorderlessWindow.h"
#include "Bitmap.h"
#include "Task.h"
namespace EzUI {
	extern std::map<HWND, Rect*> __LayeredInvalidateRect;
	/// <summary>
	/// //LayeredWindow	  //无边框  带阴影 窗口透明异形 窗口大小发生改变重绘
	/// </summary>
	class  UI_EXPORT LayeredWindow :public BorderlessWindow
	{
	private:
		void PushDC(HDC hdc);
		Rect _invalidateRect;
		Bitmap* _winBitmap = NULL;
		void Paint();
	protected:
		virtual void OnSize(const Size& sz)override;
		void InvalidateRect(const Rect& rect);
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		Rect GetUpdateRect();
		LayeredWindow(int_t width, int_t height, HWND owner = NULL);
		virtual ~LayeredWindow();
	};
};
