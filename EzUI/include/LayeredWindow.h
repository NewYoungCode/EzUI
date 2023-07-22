#pragma once
#include "Window.h"
#include "ShadowWindow.h"
namespace EzUI {
	/// <summary>
	/// //LayeredWindow	  //�ޱ߿�  ����Ӱ ����͸������ ���ڴ�С�����ı��ػ� (��ʱ�����Ч����) ���������Ӵ��� 
	/// </summary>
	class  UI_EXPORT LayeredWindow :public Window
	{
	private:
		int _shadowWidth = 12;
		void PushDC(HDC hdc);
		Rect _InvalidateRect;
		bool bRunTask = true;
		std::thread* task = NULL;
	protected:
		void InvalidateRect(const Rect& rect);
		EBitmap* _winBitmap = NULL;
		virtual void OnSize(const Size& sz)override;
		virtual void Rending(HDC winHDC, const Rect& rePaintRect)override;
		virtual void OnPaint(PaintEventArgs& args) override;
		void OnRect(const Rect& rect);
		void SetShadow(int width);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		ShadowWindow* _boxShadow = NULL;
		LayeredWindow(int width, int height, HWND owner = NULL);
		void UpdateShadow();
		void CloseShadow();
		virtual ~LayeredWindow();
		void Hide();
	};
};
