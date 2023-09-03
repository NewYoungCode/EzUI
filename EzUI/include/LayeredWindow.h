#pragma once
#include "BorderlessWindow.h"
namespace EzUI {
	/// <summary>
	/// //LayeredWindow	  //�ޱ߿�  ����Ӱ ����͸������ ���ڴ�С�����ı��ػ�
	/// </summary>
	class  UI_EXPORT LayeredWindow :public BorderlessWindow
	{
	private:
		void PushDC(HDC hdc);
		Rect _InvalidateRect;
		EBitmap* _winBitmap = NULL;
	protected:
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnSize(const Size& sz)override;
		virtual void DoPaint(HDC winHDC, const Rect& rePaintRect)override;
		void InvalidateRect(const Rect& rect);
	public:
		LayeredWindow(int width, int height, HWND owner = NULL);
		virtual ~LayeredWindow();
	};
};
