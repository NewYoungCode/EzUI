#pragma once
#include "Window.h"
#include "BoxShadow.h"
namespace EzUI {
#define SHADOWWIDTH 15
	//�ޱ߿����Ӱ�Ĵ��� �������WM_PAINT��Ϣ
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWidth = SHADOWWIDTH;
	protected:
		void OnRect(const Rect& rect) override;
		LRESULT  WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		void OnPaint(HDC hdc, const Rect& _rect) override;
	public:
		BoxShadow* _boxShadow = NULL;
		void SetShadow(int width);
		BorderlessWindow(int width, int height, HWND owner = NULL);
		virtual ~BorderlessWindow();
		void CloseShadow();
		void Hide();
	};
	//�ޱ߿� ����Ӱ �ֲ㴰�� ������WM_PAINT��Ϣ �볣�洰�ڻ�����Ϣ��ͬ �ŵ�:��Ļ�����봰�岻�Ῠ��,��ʹ��˫���� ����˸ ȱ��:���ڴ�С�����ı�ʱ�����Ҷ���
	class  UI_EXPORT LayeredWindow :public Window
	{
	private:
		int _shadowWidth = SHADOWWIDTH;
		void PushDC(HDC hdc);
	protected:
		EBitmap* _winBitmap = NULL;
		virtual void OnSize(const Size& sz)override;
		void OnPaint(HDC hdc, const Rect& rePaintRect) override;
		void OnRect(const Rect& rect);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	public:
		BoxShadow* _boxShadow = NULL;
		LayeredWindow(int cx, int cy, HWND owner = NULL);
		void CloseShadow();
		virtual ~LayeredWindow();
		void Hide();
	};
	class  UI_EXPORT MenuWindow :public BorderlessWindow {
	public:
		MenuWindow(int cx, int cy, HWND owner = NULL);
		LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void Show(int cmdShow = SW_SHOW);
	};

	//Window ������߿�WIN32������ʽ
	//BorderlessWindow �ޱ߿����Ӱ�Ĵ��� �������WM_PAINT��Ϣ
	//LayeredWindow //�ޱ߿� ����Ӱ �ֲ㴰�� ������WM_PAINT��Ϣ �볣�洰�ڻ�����Ϣ��ͬ �ŵ�:��Ļ�����봰�岻�Ῠ��,��ʹ��˫���� ����˸ ȱ��:���ڴ�С�����ı�ʱ�����Ҷ���(��ΪOnSize��ʱ�������ˢ��)
	using Form = BorderlessWindow;
};