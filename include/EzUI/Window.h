#pragma once
#include "Control.h"
#include "ScrollBar.h"
#include "Spacer.h"

#undef IsMinimized
#undef IsMaximized
#undef IsRestored

namespace ezui {
	/// <summary>
	/// Window //经典带边框带系统菜单WIN32窗口样式
	/// </summary>
	class UI_EXPORT Window :public Object
	{
		friend class BorderlessWindow;
		friend class LayeredWindow;
	private:
		//上一次鼠标按下的按钮
		MouseButton m_lastBtn = MouseButton::None;
		//是否支持缩放
		bool m_bResize = true;
		//鼠标跟踪
		bool m_bTracking = false;
		//鼠标是否在里面
		bool m_mouseIn = false;
		//鼠标是否已经按下
		bool m_mouseDown = false;
		//窗口移动
		bool m_moveWindow = false;
		//是否显示为模态窗口
		bool m_isShowModal = false;
		//是否为分层窗口
		bool m_isLayeredWindow = false;
		//当窗口关闭的时候退出代码
		int m_closeCode = 0;
		//当前窗口的绘图对象
		Graphics* m_graphics = NULL;
		//具有鼠标焦点的控件
		Control* m_focusControl = NULL;
		//具有键盘焦点的控件
		Control* m_inputControl = NULL;
		//窗口公共数据
		WindowContext* m_windowContent = NULL;
		//具有焦点的滚动条
		ScrollBar* m_scrollBar = NULL;
		//窗口句柄
		HWND m_hWnd = NULL;
		//记录鼠标坐标
		POINT m_dragPoint;
		//记录按下控件的偏移(防止鼠标按下控件移动时导致回路)
		Point m_dragOffset;
		//记录鼠标按下的坐标
		Point m_downPoint;
		//上一次鼠标按下的时间
		ULONGLONG m_lastDownTime = 0;
		//窗口最小尺寸
		Size m_miniSize;
		//窗口最大尺寸
		Size m_maxSize;
		//所属窗口句柄
		HWND m_ownerWnd = NULL;
		//窗口根Frame
		Frame* m_frame;
		// 管理图片的释放
		detail::PtrManager<Image*> m_imgs;
		// 标记是否已被释放
		std::shared_ptr<bool> m_alive;
	public:
		//对外暴露消息通知回调
		std::function<void(Control*, EventArgs&)> NotifyHandler = NULL;
	private:
		bool IsInWindow(Control* pControl, Control* it);
		//创建与初始窗口
		void Init(int width, int height, HWND owner, DWORD dStyle, DWORD  dwExStyle);
		//仅移动窗口
		void MoveWindow();
		//鼠标按下以标题栏方式移动窗口
		void TitleMoveWindow();
		//重置焦点
		void ReleaseFocusControls();
		//派发事件
		void SendEvent(Control* ctrl, const EventArgs& args);
		//生成渲染器 渲染参数
		virtual void DoPaint(HDC winDC, const Rect& rePaint);
	protected:
		//当dpi发生更改时
		virtual void OnDpiChange(float systemScale, const Rect& newRect);
		//鼠标移动时发生
		virtual void OnMouseMove(const Point& point);
		//当鼠标悬停时发生
		virtual void OnMouseHover(const Point& point);
		//鼠标离开时发生
		virtual void OnMouseLeave();
		//鼠标滚动发生
		virtual void OnMouseWheel(int zDelta, const Point& point);
		//鼠标双击是发生
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);
		//鼠标按下时发生
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);
		//鼠标弹起时发生
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);
		//渲染中
		virtual void OnPaint(PaintEventArgs& arg);
		//位置发生改变时发生
		virtual void OnMove(const Point& point);
		//大小发生改变时发生
		virtual void OnSize(const Size& sz);
		//当窗口关闭时发生 
		virtual void OnClose(bool& bClose);
		//当窗口销毁时发生
		virtual void OnDestroy();
		//当窗口显示时发生
		virtual void OnShow();
		//当窗口隐藏时发生
		virtual void OnHide();
		//字符消息
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);
		//键盘按下
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		//键盘抬起
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		//获得输入焦点时发生
		virtual void OnFocus(HWND hWnd);
		//失去输入焦点时发生
		virtual void OnKillFocus(HWND hWnd);
		//默认情况下 鼠标/键盘 事件 会进入此函数
		virtual void OnNotify(Control* sender, EventArgs& args);
		//处理消息队列的
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	public:
		Window(int width = 0, int height = 0, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD dwExStyle = NULL);

		virtual ~Window();

		//在窗口中使用基于客户区的鼠标位置寻找可命中的控件 
		Control* HitTestControl(const Point& clientPoint, Point* outPoint);

		//使用id寻找控件
		Control* FindControl(const UIString& objectName);

		//获取公共数据
		const WindowContext* GetWindowContext();

		//窗口句柄
		HWND GetWindowId();

		// 将当前窗口在 Z-order 上置于指定窗口之上，但不激活自身
		void SetTopAt(HWND hWndTarget);

		//将当前窗口置于最顶部 不激活/不获取焦点
		void Raise();

		//设置所属窗口
		void SetOwnerWindow(HWND ownerHwnd);

		//获取所属窗口句柄
		HWND GetOwnerWindow();

		//获取窗口X坐标
		int X();

		//获取窗口Y坐标
		int Y();

		// 获取窗口宽度
		int Width();

		// 获取窗口高度
		int Height();

		//获取相对矩形位置(有父窗口则相对于父窗口 没有父窗口则相对于屏幕)
		Rect GetRect();

		//获取基于屏幕的矩形位置
		Rect GetRectInScreen();

		//获取客户区矩形
		Rect GetClientRect();

		//获取当前窗口dpi缩放系数
		float GetScale();

		//设置窗口size
		void SetSize(const Size& size);

		//设置窗口位置
		void SetLocation(const Point& pt);

		//设置窗口位置大小
		void SetRect(const Rect& rect);

		//设置窗口最小size
		void SetMiniSize(const Size& size);

		//设置窗口最大size
		void SetMaxSize(const Size& size);

		//设置绝对宽高
		void SetFixedSize(const Size& size);

		//设置窗口icon
		void SetIcon(HICON icon);

		//设置窗口主布局
		void SetLayout(ezui::Control* layout);

		//从文件中加载布局
		void LoadXml(const UIString& fileName);

		//获取窗口主布局
		Control* GetLayout();

		//设置Frame
		void SetFrame(Frame* frame);

		//获取窗口Frame
		Frame* GetFrame();

		//设置窗口标题
		void SetText(const UIString& text);

		//获取窗口标题
		UIString GetText();

		//设置与取消窗口置顶
		void SetTopMost(bool top);

		//是否支持调整大小
		bool IsResizable();

		//设置窗口调整大小的支持
		void SetResizable(bool resize);

		//是否全屏
		bool IsFullScreen();

		//是否最小化
		bool IsMinimized();

		//窗口是否最大化
		bool IsMaximized();

		//窗口是否置顶
		bool IsTopMost();

		//是否显示为模态窗口
		bool IsModal();

		//操作窗口的显示
		virtual void Show();

		//操作窗口的显示 带参数
		void Show(int cmdShow);

		//隐藏窗口
		virtual void Hide();

		// 恢复窗口到正常状态（从最小化或最大化状态恢复）
		void Restore();

		//关闭窗口 exitCode为退出代码
		void Close(int exitCode = 0);

		//进入模态消息循环并阻塞当前线程 关闭窗口则退出循环
		int Exec();

		/// 以模态方式显示窗口（阻塞当前线程）
		/// @param disableOwnerWindow 是否禁用所有者窗口 前置条件：必须设置有效的所有者窗口句柄
		/// @return 窗口退出代码
		virtual int ShowModal(bool disableOwnerWindow = false);

		//最小化窗口
		void ShowMinimized();

		//最大化窗口
		void ShowMaximized();

		//让窗口占满当前屏幕
		void ShowFullScreen();

		//窗口是否显示
		bool IsVisible();

		//设置窗口显示/隐藏
		void SetVisible(bool flag);

		//窗口是否已启用
		bool IsEnabled();

		//设置启用/禁用窗口
		void SetEnabled(bool bEnable);

		//刷新窗口(标记无效区域延迟刷新)
		void Invalidate();

		//立即更新所有无效区域(立即刷新)
		void Refresh();

		//居中到屏幕
		void CenterToScreen();

		//基于某个窗口进行居中
		void CenterToWindow(HWND wnd = NULL);

		//给指定控件为焦点控件
		void SetFocus(Control* ctl);

		//绑定对象(跟随释放)
		using Object::Attach;

		//分离对象(解除跟随释放)
		using Object::Detach;

		//绑定图片(跟随释放)
		Image* Attach(Image* img);

		//分离图片(解除跟随释放)
		void Detach(Image* img);
	};
};