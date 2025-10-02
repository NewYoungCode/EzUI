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
	private:
		DXRender* graphics = NULL;
		//具有鼠标焦点的控件
		Control* m_focusControl = NULL;
		//具有键盘焦点的控件
		Control* m_inputControl = NULL;
		//窗口公共数据
		WindowContext* m_publicData = NULL;
		//窗口句柄
		HWND m_hWnd = NULL;
		//鼠标跟踪
		bool m_bTracking = false;
		//鼠标是否在里面
		bool m_mouseIn = false;
		//鼠标是否已经按下
		bool m_mouseDown = false;
		//窗口移动
		bool m_moveWindow = false;
		//记录鼠标坐标
		POINT m_dragPoint;
		//记录鼠标按下的坐标
		Point m_downPoint;
		//上一次鼠标按下的时间
		ULONGLONG m_lastDownTime = 0;
		//上一次鼠标按下的按钮
		MouseButton m_lastBtn = MouseButton::None;
		//窗口最小尺寸
		Size m_miniSize;
		//窗口最大尺寸
		Size m_maxSize;
		//当窗口关闭的时候退出代码
		int m_closeCode = 0;
		//基于桌面的坐标
		Rect m_rect;
		//客户绘图区域
		Rect m_rectClient;
		//所属窗口句柄
		HWND m_ownerWnd = NULL;
		//窗口根Frame
		Frame* m_frame;
		// 管理图片的释放
		PtrManager<Image*> m_imgs;
	public:
		//对外暴露消息通知回调
		std::function<void(Control*, EventArgs&)> NotifyHandler = NULL;
	private:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		bool IsInWindow(Control* pControl, Control* it);
		void Init(int width, int height, HWND owner, DWORD dStyle, DWORD  dwExStyle);//初始窗口
		//仅移动窗口
		void MoveWindow();
		//鼠标按下以标题栏方式移动窗口
		void TitleMoveWindow();
		//在窗口中使用基于客户区的鼠标位置寻找可命中的控件 
		Control* HitTestControl(const Point& clientPoint, Point* outPoint);
		//派发事件
		void SendEvent(Control* ctrl, const EventArgs& args);
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
		//生成渲染器 渲染参数
		virtual void DoPaint(HDC winDC, const Rect& rePaint);
		//渲染中
		virtual void OnPaint(PaintEventArgs& arg);
		//位置发生改变时发生
		virtual void OnMove(const Point& point);
		//大小发生改变时发生
		virtual void OnSize(const Size& sz);
		//当窗口收到WM_CLOSE消息时发生 
		virtual void OnClose(bool& bClose);
		//当窗口销毁时发生
		virtual void OnDestroy();
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
		//鼠标 键盘 重绘 会进入此函数,如果返回true则事件将不再交给sender控件处理 将忽略类似OnMouseDown... Notiify事件处理器...
		virtual void OnNotify(Control* sender, EventArgs& args);
		//处理消息队列的
		virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
		//获取阴影窗口句柄
		virtual HWND GetShadowHwnd();
	public:
		Window(int width, int height, HWND owner = NULL, DWORD dStyle = WS_OVERLAPPEDWINDOW, DWORD dwExStyle = NULL);

		virtual ~Window();

		//使用id寻找控件
		Control* FindControl(const UIString& objectName);

		//获取公共数据
		WindowContext* GetWindowContext();

		//窗口句柄
		HWND Hwnd();

		//获取窗口X坐标
		int X();

		//获取窗口Y坐标
		int Y();

		// 获取窗口宽度
		int Width();

		// 获取窗口高度
		int Height();

		//获取基于父窗口或显示器的矩形
		const Rect& GetRect();

		//获取客户区矩形
		const Rect& GetClientRect();

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

		//是否全屏
		bool IsFullScreen();

		//是否最小化
		bool IsMinimized();

		//窗口是否最大化
		bool IsMaximized();

		//窗口是否置顶
		bool IsTopMost();

		//操作窗口的显示
		virtual void Show();

		//操作窗口的显示 带参数
		void Show(int cmdShow);

		//隐藏窗口
		virtual void Hide();

		//正常显示窗口
		void ShowNormal();

		//关闭窗口 exitCode为退出代码
		void Close(int exitCode = 0);

		//模态窗口方式显示窗口(会阻塞) 请务必在窗口构造函数中传入owner窗口句柄
		virtual int ShowModal(bool disableOnwer = true);

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

		//使区域无效(延迟刷新)
		void Invalidate();

		//立即更新所有无效区域(立即刷新)
		void Refresh();

		//居中到屏幕
		void CenterToScreen();

		//参考某个窗口进行居中
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