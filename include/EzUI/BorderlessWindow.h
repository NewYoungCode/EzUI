#pragma once
#include "Window.h"
#include "ShadowBox.h"
namespace EzUI {
	/// <summary>
	/// BorderlessWindow //无边框 带阴影 
	/// </summary>
	class UI_EXPORT BorderlessWindow :public Window {
	private:
		int_t _shadowWeight = 20;
		ShadowBox* _shadowBox = NULL;
		float _shadowScale = 1.0f;
		//是否支持缩放
		bool _resize = false;
	public:
		//窗口的边框信息
		EzUI::Border Border;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)override;
		virtual void OnMove(const Point& location) override;
		virtual void OnSize(const Size& sz) override;
		virtual void OnDpiChange(float systemScale, const Rect& newRect);//当dpi发生更改时
		virtual void OnPaint(PaintEventArgs& args)override;
	public:
		//设置阴影宽度
		void SetShadow(int_t weight);
		BorderlessWindow(int_t width, int_t height, HWND owner = NULL, DWORD exStyle = NULL);
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
