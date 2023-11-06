#pragma once
#include "Window.h"
#include "ShadowBox.h"
namespace EzUI {
	/// <summary>
	/// BorderlessWindow //�ޱ߿� ����Ӱ 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWeight = 12;
		bool _firstPaint = true;
		ShadowBox* _shadowBox = NULL;
		float _shadowScale = 1.0f;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnRect(const Rect& rect) override;
		virtual void OnDpiChange(const float& systemScale, const Rect& newRect);//��dpi��������ʱ
		virtual void DoPaint(HDC winHDC, const Rect& rePaintRect)override;
		void UpdateShadowBox();
	public:
		void SetShadow(int weight);
		BorderlessWindow(int width, int height, HWND owner = NULL, DWORD exStyle = NULL);
		virtual ~BorderlessWindow();
		ShadowBox* GetShadowBox();
		void CloseShadowBox();
		void Hide();
	};
};
