#pragma once
#include "control/Control.h"
#include "control/ScrollBar.h"

#undef IsMinimized
#undef IsMaximized
#undef IsRestored

namespace ezui {

#ifdef DEBUG
	//配置截图和布局快照
	struct DumpConfig
	{
		bool dumpUiTree;
		bool dumpScreenshot;
		std::wstring uiFile;
		std::wstring shotFile;
		DumpConfig()
			: dumpUiTree(false), dumpScreenshot(false), uiFile(L"dbg_layout.json"), shotFile(L"dbg_capture.png") {
		}
	};
#endif // DEBUG

	class IWindow {
	public:
		virtual ~IWindow() {};
	};
	class WindowFileDropTarget;

	/**
	 * @brief Window：经典带边框、带系统菜单的 Win32 窗口封装。
	 *
	 * 说明：
	 * - 本类大多数方法应在 UI（主）线程调用。
	 */
	class EZUI_API Window :public IWindow, public Object
	{
		friend class BorderlessWindow;
		friend class WindowFileDropTarget;
	private:
		struct WindowContext {
			/// 上一次鼠标按下的按钮
			MouseButton m_lastBtn;
			// 窗口是否可被激活。
			bool m_activatable;
			/// 是否支持调整大小（缩放）
			bool m_bResize;
			/// 是否正在跟踪鼠标（用于 WM_MOUSELEAVE 等）
			bool m_bTracking;
			/// 鼠标是否在窗口内
			bool m_mouseIn;
			/// 鼠标是否处于按下状态
			bool m_mouseDown;
			/// 是否正在移动窗口（内部控制）
			bool m_moveWindow;
			/// 是否以模态方式显示
			bool m_isShowModal;
			/// 窗口关闭时的退出代码（由 Close 设置）
			int m_closeCode;
			/// 当前窗口的绘图对象（若为 NULL 则表示未创建）
			Graphics* m_graphics;
			/// 具有逻辑焦点控件
			Collection<CtrlWeakPtr<Control>> m_focusedControls;
			/// 当前具有鼠标焦点的控件
			CtrlWeakPtr<Control> m_hoverControl;
			/// 当前具有键盘输入焦点的控件
			CtrlWeakPtr<Control> m_focusedControl;
			// 窗口桥接对象指针（用于 Control 与 Window 之间的通信）
			// 通过该对象调用窗口相关能力（如重绘、刷新、设置焦点等）
			WindowBridge* m_windowBridge;
			/// 当前具有焦点的滚动条（若有）
			CtrlWeakPtr<ScrollBar> m_scrollBar;
			/// 当前文件拖拽悬停并接受拖拽的控件
			CtrlWeakPtr<Control> m_fileDragHoverControl;
			/// 文件拖拽 COM 接收对象
			WindowFileDropTarget* m_fileDropTarget;
			/// 当前线程是否由本窗口成功初始化 OLE
			bool m_oleInitialized;
			/// 当前窗口是否已注册 OLE 拖拽目标
			bool m_dragDropRegistered;
			/// 原生窗口句柄（WindowId）
			WindowHandle m_hWnd;
			/// 记录拖拽过程中的鼠标坐标（屏幕坐标或客户端坐标视实现而定）
			POINT m_dragPoint;
			/// 记录按下控件时的偏移（防止拖拽引起逻辑回路）
			Point m_dragOffset;
			/// 记录最后一次鼠标按下的坐标
			Point m_downPoint;
			/// 上一次鼠标按下的时间（毫秒/时间戳，用于双击判定等）
			int64_t m_lastDownTime;
			/// 窗口最小尺寸约束
			Size m_minSize;
			/// 窗口最大尺寸约束
			Size m_maxSize;
			/// 所属（Owner）窗口句柄，若无则为 NULL
			WindowHandle m_ownerWnd;
			/// 窗口根 Frame（窗口内容根容器 负责加载布局文件 内部使用）
			Frame* m_rootFrame;
			//缩放率
			float Scale;
			WindowContext();
		};
	private:
		///数据上下文
		WindowContext m_ctx;
	private:
		/// 判断控件 candidate 是否在 rootControl 的子树或范围内（内部使用）
		bool IsInWindow(Control* rootControl, Control* candidate);
		/// 创建并初始化窗口（内部）
		void Init(WindowHandle ownerHwnd, DWORD style, DWORD exStyle);
		/// 仅移动窗口（内部实现）
		void MoveWindow();
		/// 以标题栏方式通过鼠标按下移动窗口（内部）
		void TitleMoveWindow();
		/// 重置焦点相关控件（内部）
		void ResetStateFocusControls();
		/// 向指定控件派发事件（内部）
		void SendEvent(const CtrlWeakPtr<Control>& target, EventArgs* args);
		/// 向控件树派发文件拖拽事件（内部）
		CtrlWeakPtr<Control> DispatchFileDragEvent(Event eventType, const std::vector<UIString>& files,
			const Point& windowPoint, uint32_t keyState, DragDropEffect allowedEffect, DragDropEffect* outEffect);
		/// 默认窗口级文件拖拽处理（内部）
		void HandleWindowFileDrag(FileDragEventArgs* args, Event eventType);
		/// 清理文件拖拽 COM 接收对象（内部）
		void CleanupFileDropTarget();
		/// 清理焦点控件列表（移除已失效的控件代理）
		void CleanUpFocusedControls();
		/// 执行实际绘制操作（生成渲染器并传入渲染参数 内部使用）
		virtual void OnPaintBefore(PaintEventArgs* args);
	protected:
		/// 当 DPI 发生改变时调用
		virtual void OnDpiChanged(float systemScale);
		/// 鼠标移动事件回调（传入客户端坐标）
		virtual void OnMouseMove(const Point& position);
		/// 鼠标悬停事件回调（传入客户端坐标）
		virtual void OnMouseHover(const Point& position);
		/// 鼠标离开事件回调
		virtual void OnMouseLeave();
		/// 鼠标滚轮事件回调
		/// @param wheelDelta 滚轮增量（与 WM_MOUSEWHEEL 的 zDelta 对应）
		/// @param position   鼠标位置（客户端坐标）
		virtual void OnMouseWheel(int wheelDelta, const Point& position);
		/// 鼠标双击事件回调
		virtual void OnMouseDoubleClick(MouseButton button, const Point& position);
		/// 鼠标按下事件回调
		virtual void OnMouseDown(MouseButton button, const Point& position);
		/// 鼠标弹起事件回调
		virtual void OnMouseUp(MouseButton button, const Point& position);
		/// 文件拖拽进入窗口事件回调
		virtual void OnDragEnter(FileDragEventArgs* args);
		/// 文件拖拽悬停在窗口上事件回调
		virtual void OnDragOver(FileDragEventArgs* args);
		/// 文件拖拽离开窗口事件回调
		virtual void OnDragLeave(FileDragEventArgs* args);
		/// 文件拖拽释放到窗口事件回调
		virtual void OnDragDrop(FileDragEventArgs* args);
		/// 绘制事件回调
		virtual void OnPaint(PaintEventArgs* args);
		/// 窗口位置改变事件回调（客户端/窗口坐标视实现）
		virtual void OnMove(const Point& position);
		/// 窗口大小改变事件回调
		virtual void OnSize(const Size& newSize);
		/// 窗口关闭前回调，可通过 allowClose 控制是否允许关闭（false 表示取消关闭）
		virtual void OnClose(bool& allowClose);
		/// 窗口销毁时回调（资源释放等）
		virtual void OnDestroy();
		/// 窗口显示时回调
		virtual void OnShow();
		/// 窗口隐藏时回调
		virtual void OnHide();
		/// 字符输入消息回调（如 WM_CHAR）
		virtual void OnKeyChar(WPARAM wParam, LPARAM lParam);
		/// 键盘按下消息回调（如 WM_KEYDOWN）
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		/// 键盘抬起消息回调（如 WM_KEYUP）
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		/// 获得输入焦点时回调（传入失去焦点的窗口句柄）
		virtual void OnFocus(WindowHandle lostFocusHwnd);
		/// 失去输入焦点时回调（传入获得焦点的窗口句柄）
		virtual void OnKillFocus(WindowHandle focusedHwnd);
		/// 默认的通知入口（鼠标/键盘事件等默认会进入此函数）
		virtual void OnNotify(Control* sender, EventArgs* args);
		/// 默认窗口消息处理（子类可重写）
		virtual LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		/// 构造：ownerHwnd 为所属窗口句柄（可为 NULL）
		explicit Window(WindowHandle ownerHwnd = NULL, DWORD style = WS_OVERLAPPEDWINDOW, DWORD exStyle = NULL);

		explicit Window(Window* ownerWnd);

		/// 析构
		virtual ~Window();

		/// 在窗口中使用基于客户区的坐标寻找可命中的控件
		/// @param clientPoint 客户区坐标
		/// @param outPoint 输出为控件相对坐标（若不需可传 NULL）
		/// @param eventType 事件类型（用于调试，默认为 None）
		/// @return 命中的控件指针或 NULL
		CtrlWeakPtr<Control> HitTestControl(const Point& clientPoint, Point* outPoint, Event eventType = Event::None);

		/// 使用控件的 name/id 查找控件（包括子控件）
#ifdef EZUI_COMPILER_VS2010
		template<typename T>
#else
		template<typename T = Control>
#endif
		T* FindControl(const UIString& ctrlName) {
			Control* layout = this->GetLayout();
			if (layout) {
				return layout->FindControl<T>(ctrlName);
			}
			return NULL;
		}

#ifdef EZUI_COMPILER_VS2010
		Control* FindControl(const UIString& ctrlName) {
			return this->FindControl<Control>(ctrlName);
		}
#endif // #ifdef EZUI_COMPILER_VS2010

		///在当前窗口布主局中控件中使用属性查找控件树匹配的控件。
		ControlCollection FindControls(const UIString& attrName, const UIString& attrValue);

		/// 根据 class 查找控件
		ControlCollection FindControlsByClass(const UIString& className);

		// 窗口桥接对象指针（用于 Control 与 Window 之间的通信）
		// 通过该对象调用窗口相关能力（如重绘、刷新、设置焦点等）
		const WindowBridge* GetWindowBridge()const EZUI_NOEXCEPT;

		/// 获取原生窗口句柄
		WindowHandle GetWindowHandle()const EZUI_NOEXCEPT;

		/// 将当前窗口在 Z-order 上置于指定窗口之上，但不激活自身
		void SetZOrderAbove(WindowHandle targetHwnd);

		/// 将当前窗口置于最前端（不激活/不获取焦点）
		void SetZOrderTopNoActivate();

		/// 设置所属窗口（Owner）
		void SetOwnerWindow(WindowHandle ownerHwnd);

		/// 获取所属窗口句柄（Owner）
		WindowHandle GetOwnerWindow() EZUI_NOEXCEPT;

		/// 设置窗口根 Frame 的唯一通知处理器。
		/// 重复调用会替换已有处理器。设置非空处理器后，根 Frame 的通知由该处理器完全接管，
		/// 不再自动转发到虚函数 Window::OnNotify。
		/// 传入空 EventHandler 会清除处理器，并由根 Frame 直接执行默认通知逻辑；
		/// 此操作不会恢复到 Window::OnNotify 的转发。
		void SetNotifyHandler(const EventHandler& handler);

		/// 获取窗口左上角 X（屏幕坐标或父窗口相对坐标）
		int X()const EZUI_NOEXCEPT;

		/// 获取窗口左上角 Y（屏幕坐标或父窗口相对坐标）
		int Y()const EZUI_NOEXCEPT;

		/// 获取窗口宽度
		int Width()const EZUI_NOEXCEPT;

		/// 获取窗口高度
		int Height()const EZUI_NOEXCEPT;

		/// 获取相对矩形位置（有父窗口则相对于父窗口，否则相对于屏幕）
		Rect GetRect()const EZUI_NOEXCEPT;

		/// 获取基于屏幕的矩形位置
		Rect GetRectInScreen()const EZUI_NOEXCEPT;

		/// 获取客户区矩形
		Rect GetClientRect()const EZUI_NOEXCEPT;

		/// 设置当前窗口 DPI 缩放系数（scale）
		void SetScale(float scale);

		/// 获取当前窗口 DPI 缩放系数（scale）
		float GetScale()const EZUI_NOEXCEPT;

		/// 设置窗口大小
		/// @param scaledSize 已经按某缩放处理过的大小
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetSize(const Size& scaledSize, float srcScale = 1.0f);

		/// 设置窗口大小
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetSize(int width, int height, float srcScale = 1.0f);

		/// 设置窗口位置
		/// @param scaledPos 已经按某缩放处理过的位置
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetPosition(const Point& scaledPos, float srcScale = 1.0f);

		/// 设置窗口位置
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetPosition(int x, int y, float srcScale = 1.0f);

		/// 设置窗口位置和大小
		/// @param scaledRect 已经按某缩放处理过的矩形
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetRect(const Rect& scaledRect, float srcScale = 1.0f);

		/// 设置窗口位置和大小
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetRect(int x, int y, int width, int height, float srcScale = 1.0f);

		/// 设置窗口最小允许尺寸
		/// @param scaledSize 已经按某缩放处理过的最小尺寸
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetMinSize(const Size& scaledSize, float srcScale = 1.0f);

		/// 设置窗口最大允许尺寸
		/// @param scaledSize 已经按某缩放处理过的最大尺寸
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetMaxSize(const Size& scaledSize, float srcScale = 1.0f);

		/// 设置固定尺寸（忽略自动缩放）
		/// @param scaledSize 已经按某缩放处理过的固定尺寸
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetFixedSize(const Size& scaledSize, float srcScale = 1.0f);

		/// 设置固定尺寸（忽略自动缩放）
		/// @param srcScale 传入值对应的缩放，默认 1.0
		void SetFixedSize(int fixedWidth, int fixedHeight, float srcScale = 1.0f);

		/// 设置窗口图标
		void SetIcon(HICON icon);

		/// 设置窗口主布局控件；takeOwnership 为 true 时窗口根 Frame 同时取得其所有权。
		void SetLayout(ezui::Control* layout, bool takeOwnership = false);

		/// 从 XML 文件加载并应用布局到窗口（路径为工程可访问路径）
		void LoadXml(const UIString& filePath);

		/// 获取窗口主布局控件（若未设置返回 NULL）
		Control* GetLayout()const EZUI_NOEXCEPT;

		/// 设置窗口标题文字
		void SetTitle(const UIString& text);

		/// 获取窗口标题文字
		UIString GetTitle()const EZUI_NOEXCEPT;

		/// 设置或取消窗口置顶
		void SetTopMost(bool top);

		/// 窗口是否支持调整大小
		bool IsResizable()const EZUI_NOEXCEPT;

		/// 设置窗口是否支持调整大小
		void SetResizable(bool resize);

		//获取当前窗口是否可被激活(当前是否会抢占焦点)
		bool IsActivatable() const;

		// 设置窗口是否可被激活 为false 时窗口不会抢占其他窗口的焦点。
		void SetActivatable(bool activatable);

		/// 窗口是否处于全屏状态
		bool IsFullScreen()const;

		/// 窗口是否最小化
		bool IsMinimized()const EZUI_NOEXCEPT;

		/// 窗口是否最大化
		bool IsMaximized()const EZUI_NOEXCEPT;

		/// 窗口是否位于最顶层（TopMost）
		bool IsTopMost()const EZUI_NOEXCEPT;

		/// 窗口是否以模态方式显示
		bool IsModal()const EZUI_NOEXCEPT;

		/// 显示窗口（默认 Show，非模态）
		virtual void Show();

		/// 显示窗口（传入 WinAPI 的 cmdShow）
		void Show(int cmdShow);

		/// 隐藏窗口
		virtual void Hide();

		/// 恢复窗口到正常状态（从最小化或最大化恢复）
		void Restore();

		/// 关闭窗口，exitCode 为退出代码（将作为 ShowModal/Exec 的返回值）
		void Close(int exitCode = 0);

		/// 进入循环并阻塞当前线程，直到窗口关闭（内部消息循环）
		/// 返回值为退出代码（由 Close 设置）
		int Exec();

		/**
		 * @brief 以模态方式显示窗口（阻塞当前线程）
		 * @param disableOwnerWindow 是否禁用所有者窗口。前提：必须先设置有效的 Owner 窗口句柄。
		 * @return 窗口退出代码（等同于 Close(exitCode) 所传值）
		 *
		 * 注意：通常应在 UI 线程调用，本函数会阻塞直到窗口关闭。
		 */
		virtual int ShowModal(bool disableOwnerWindow = false);

		/// 将窗口最小化
		void ShowMinimized();

		/// 将窗口最大化
		void ShowMaximized();

		/// 让窗口占满当前屏幕（全屏显示）
		void ShowFullScreen();

		/// 窗口当前是否可见
		bool IsVisible()const EZUI_NOEXCEPT;

		/// 设置窗口可见性（true 显示，false 隐藏）
		void SetVisible(bool flag);

		/// 窗口是否已启用（接受输入）
		bool IsEnabled()const EZUI_NOEXCEPT;

		/// 启用或禁用窗口（接受输入/交互）
		void SetEnabled(bool bEnable);

		/// 标记窗口为需要刷新（延迟重绘）
		void Invalidate();

		/// 立即刷新窗口（处理所有无效区域）
		void Refresh();

		//立即刷新布局窗口内的所有脏布局
		int RefreshLayout();

		/// 将窗口居中到屏幕中央
		void CenterToScreen();

		/// 基于指定窗口进行居中（默认基于当前活动窗口）
		void CenterToWindow(WindowHandle ownerHwnd = NULL);

		/// 将指定控件设置为焦点控件（控件必须属于当前窗口）
		void SetFocusControl(CtrlWeakPtr<Control> control);

		/// 获取当前具有焦点的控件（若无则返回 NULL）
		Control* GetFocusControl()const EZUI_NOEXCEPT;

		// 是否启用 Alpha 混合（Windows 下即分层窗口）
		bool IsAlphaBlendingEnabled()const EZUI_NOEXCEPT;

		//获取窗口DC(如果当前是非layeredwindow则需要调用ReleaseDC 内部使用)
		virtual HDC GetDC();
	};
};
