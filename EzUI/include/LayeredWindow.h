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
		EBitmap* _winBitmap = NULL;
		ShadowWindow* _boxShadow = NULL;
	protected:
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnSize(const Size& sz)override;
		virtual void DoPaint(HDC winHDC, const Rect& rePaintRect)override;
		virtual void OnPaint(PaintEventArgs& args) override;
		virtual void OnRect(const Rect& rect)override;
		void UpdateShadow();
		void InvalidateRect(const Rect& rect);
	public:
		LayeredWindow(int width, int height, HWND owner = NULL);
		ShadowWindow* GetShadowWindow();
		void SetShadow(int padding);
		void CloseShadow();
		virtual ~LayeredWindow();
		void Hide();
	};
};
