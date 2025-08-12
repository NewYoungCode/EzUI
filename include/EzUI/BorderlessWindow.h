#pragma once
#include "Window.h"
#include "ShadowBox.h"

namespace ezui {
	/// <summary>
	/// BorderlessWindow //无边框 带阴影 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int m_shadowWeight = 20;
		ShadowBox* m_shadowBox = NULL;
		float m_shadowScale = 1.0f;
		//是否支持缩放
		bool m_bResize = false;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnMove(const Point& location) override;
		virtual void OnSize(const Size& sz) override;
		virtual void OnDpiChange(float systemScale, const Rect& newRect);//当dpi发生更改时
	public:
		//设置阴影宽度
		void SetShadow(int weight);
		BorderlessWindow(int width, int height, HWND owner = NULL, DWORD exStyle = NULL);
		virtual ~BorderlessWindow();
		//更新窗口阴影
		void UpdateShadowBox();
		//获取阴影窗口
		ShadowBox* GetShadowBox();
		//关闭窗口阴影 关掉阴影窗口 已有的边框也会随之消失
		void CloseShadowBox();
		//设置窗口缩放支持
		void SetResizable(bool resize);
		//是否支持调整大小
		bool IsResizable();
	};
};
