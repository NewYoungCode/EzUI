#pragma once
#include "Window.h"
#include "ShadowBox.h"

namespace ezui {
	/// <summary>
	/// BorderlessWindow //无边框 带阴影 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
		friend class LayeredWindow;
	private:
		int m_shadowWeight = 20;
		ShadowBox* m_shadowBox = NULL;
		float m_shadowScale = 1.0f;
		//是否第一次已经绘制
		bool isFirstPaint = false;
	private:
		virtual void DoPaint(HDC winDC, const Rect& rePaint)override;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnMove(const Point& location) override;
		virtual void OnSize(const Size& sz) override;
		virtual void OnDpiChange(float systemScale, const Rect& newRect);//当dpi发生更改时
	public:
		//设置阴影宽度
		void SetShadow(int weight);
		BorderlessWindow(int width = 0, int height = 0, HWND owner = NULL, DWORD dwStyle = NULL, DWORD dwExStyle = NULL);
		virtual ~BorderlessWindow();
		//更新窗口阴影
		void UpdateShadowBox();
		//获取阴影窗口
		ShadowBox* GetShadowBox();
		//关闭窗口阴影 关掉阴影窗口 已有的边框也会随之消失
		void CloseShadowBox();
	};
};
