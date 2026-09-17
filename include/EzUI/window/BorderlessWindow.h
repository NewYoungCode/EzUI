#pragma once
#include "window/Window.h"

namespace ezui {
	class WindowShadow;

	/// <summary>
	/// BorderlessWindow //无边框 带阴影 
	/// </summary>
	class EZUI_API BorderlessWindow :public Window {
		friend class detail::RenderContext;
	private:
		WindowShadow* m_shadowHost;
		//是否第一次已经绘制
		bool m_isFirstPaint;
		//窗口透明度(分层窗口下才有效)
		float m_opacity;
	private:
		void Init();
		virtual void OnPaintBefore(PaintEventArgs* args)EZUI_OVERRIDE;
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)EZUI_OVERRIDE;
		virtual void OnMove(const Point& location) EZUI_OVERRIDE;
		virtual void OnSize(const Size& sz) EZUI_OVERRIDE;
		virtual void OnDpiChanged(float systemScale);//当dpi发生更改时
		virtual void UpdateLayeredWindow(HDC hdc, const Rect& rePaintRect);/// 更新分层窗口
	public:
		explicit BorderlessWindow(WindowHandle owner = NULL, DWORD dwStyle = NULL, DWORD dwExStyle = NULL);

		explicit BorderlessWindow(Window* ownerWnd);

		//更新窗口阴影
		void UpdateShadow();

		//获取窗口阴影实例
		WindowShadow* GetShadow();

		//关闭窗口阴影 关掉阴影窗口 已有的边框也会随之消失
		void CloseShadow();

		//启用透明通道混合 启用后支持圆角异形/透明/窗口特性(在windows平台上内部会转为分层窗口)
		void EnableAlphaBlending();

		//设置窗口透明度 0.0~1.0
		void SetOpacity(float opacity);

		//获取窗口透明度
		float GetOpacity();

		//获取窗口DC(如果当前是非layeredwindow则需要调用ReleaseDC 内部使用)
		virtual HDC GetDC()EZUI_OVERRIDE;

		virtual ~BorderlessWindow();
	};

};


namespace ezui {
	//用于实现窗口阴影效果的类
	class EZUI_API WindowShadow :public IWindow
	{
		friend class BorderlessWindow;
	private:
		//窗口透明度
		float m_opacity;
		bool m_isUpdating;
		Color m_shadowColor;
		int m_shadowBorderWidth;
		float m_shadowScale;
		int16_t m_radius;//圆角
		Bitmap* m_bufBitmap;
		WindowHandle m_hWnd;
		WindowHandle m_mainHWnd;
		Size m_lastSize;
		WindowBridge* m_windowContext;
	private:
		bool RenderShadow(Bitmap* bitmap, int iSize, float radius);
	protected:
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void OnSize(const Size& sz);
	public:
		WindowShadow(WindowHandle mainHwnd);//构造函数
		virtual ~WindowShadow();
		//在父窗口发生改变的时候更新阴影区域
		//shadowWidth:阴影宽度
		//radius:阴影边框半径
		//shadowColor:阴影颜色
		virtual bool Update(int shadowWidth, int radius, const Color& shadowColor);
		//更新透明度
		void SetOpacity(float opacity);
		//设置阴影宽度
		void SetShadow(int borderWidth);
		//设置阴影的边框半径
		void SetBorderRadius(int radius);
		//设置窗口阴影颜色
		void SetShadowColor(const Color& color);
		//获取窗口句柄
		WindowHandle GetWindowHandle();
		//获取DC句柄
		HDC GetDC();
	};
};
