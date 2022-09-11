#pragma once
#include "Window.h"
#include "BoxShadow.h"
#include <thread>
#include <mutex>
namespace EzUI {
	//�ޱ߿� ����Ӱ �ֲ㴰�� ������WM_PAINT��Ϣ �볣�洰�ڻ�����Ϣ��ͬ �ŵ�:��Ļ�����봰�岻�Ῠ��,��ʹ��˫���� ����˸ ȱ��:���ڴ�С�����ı�ʱ�����Ҷ���
	class  UI_EXPORT LayeredWindow :public Window
	{
	private:
		int _shadowWidth = SHADOWWIDTH;
		void PushDC(HDC hdc);
		Rect _InvalidateRect;
		bool bRunTask = true;
		std::thread* task = NULL;
	protected:
		void InvalidateRect(const Rect& rect);
		EBitmap* _winBitmap = NULL;
		virtual void OnSize(const Size& sz)override;
		void OnPaint(HDC hdc, const Rect& rePaintRect) override;
		void OnRect(const Rect& rect);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		BoxShadow* _boxShadow = NULL;
		LayeredWindow(int cx, int cy, HWND owner = NULL);
		void UpdateShadow();
		void CloseShadow();
		virtual ~LayeredWindow();
		void Hide();
	};
};
