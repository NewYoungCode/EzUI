#pragma once
#include "Window.h"
#include "ShadowWindow.h"
namespace EzUI {
	//�ޱ߿����Ӱ�Ĵ��� �������WM_PAINT��Ϣ
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWidth = 12;
	protected:
		void OnRect(const Rect& rect) override;
		LRESULT  WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		ShadowWindow* _boxShadow = NULL;
		void SetShadow(int width);
		BorderlessWindow(int width, int height, HWND owner = NULL);
		virtual ~BorderlessWindow();
		void UpdateShadow();
		void CloseShadow();
		void Hide();
	};
};
