#pragma once
#include "BorderlessWindow.h"
#include "EBitmap.h"
#include "Task.h"
namespace EzUI {
	/// <summary>
	/// //LayeredWindow	  //无边框  带阴影 窗口透明异形 窗口大小发生改变重绘
	/// </summary>
	class  UI_EXPORT LayeredWindow :public BorderlessWindow
	{
	private:
		void PushDC(HDC hdc);
		Rect _InvalidateRect;
		EBitmap* _winBitmap = NULL;
		bool _bStop = false;
		Task* _paintTask = NULL;
		std::mutex mtx;
		std::condition_variable condv;
		void Paint();
	protected:
		virtual void OnSize(const Size& sz)override;
		void InvalidateRect(const Rect& rect);
	public:
		LayeredWindow(int width, int height, HWND owner = NULL);
		virtual ~LayeredWindow();
	};
};
