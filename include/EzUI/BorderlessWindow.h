#pragma once
#include "Window.h"
#include "ShadowBox.h"
namespace EzUI {
	/// <summary>
	/// BorderlessWindow //无边框 带阴影 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWeight = 20;
		bool _firstPaint = true;
		ShadowBox* _shadowBox = NULL;
		float _shadowScale = 1.0f;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnRect(const Rect& rect) override;
		virtual void OnDpiChange(const float& systemScale, const Rect& newRect);//当dpi发生更改时
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
