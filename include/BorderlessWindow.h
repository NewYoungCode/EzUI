#pragma once
#include "Window.h"
#include "BoxShadow.h"
namespace EzUI {
	//�ޱ߿����Ӱ�Ĵ��� �������WM_PAINT��Ϣ
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWidth = SHADOWWIDTH;
	protected:
		void OnRect(const Rect& rect) override;
		LRESULT  WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		BoxShadow* _boxShadow = NULL;
		void SetShadow(int width);
		BorderlessWindow(int width, int height, HWND owner = NULL);
		virtual ~BorderlessWindow();
		void UpdateShadow();
		void CloseShadow();
		void Hide();
	};
};
