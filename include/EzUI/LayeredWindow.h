#pragma once
#include "BorderlessWindow.h"
#include "Bitmap.h"
#include "Task.h"
#include "Timer.h"

namespace ezui {
	namespace detail {
		class LayeredWindowPaintTimer;
	}
	/// <summary>
	/// //LayeredWindow	  //无边框  带阴影 窗口透明异形 
	/// </summary>
	class UI_EXPORT LayeredWindow :public BorderlessWindow
	{
		friend class detail::LayeredWindowPaintTimer;
	private:
		Bitmap* m_winBitmap = NULL;
		std::list<Rect> m_invalidateRect;
		//窗口透明度
		float m_opacity = 1.0f;
		void UpdateLayeredWindow(HDC hdc, const Rect& rePaintRect);
		void Merge(std::vector<Rect>* outRect);
		bool Paint();
	protected:
		virtual void OnSize(const Size& sz)override;
		void InvalidateRect(const Rect& rect);
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		void SetOpacity(float opacity);
		float GetOpacity();
		LayeredWindow(int width = 0, int height = 0, HWND owner = NULL, DWORD dwStyle = NULL, DWORD dwExStyle = NULL);
		virtual ~LayeredWindow();
	};
};
