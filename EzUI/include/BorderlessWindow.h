#pragma once
#include "Window.h"
#include "ShadowBox.h"
namespace EzUI {
	/// <summary>
	/// BorderlessWindow //ÎÞ±ß¿ò ´øÒõÓ° 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int _shadowWeight = 12;
		bool _firstPaint = true;
		ShadowBox* _shadowBox = NULL;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnRect(const Rect& rect) override;
		virtual void OnPaint(PaintEventArgs& args)override;
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
