//WIN32 desktop application UI framework (2d graphical library:direct2d,后期可能会采用其他跨平台的2d图形库对整个UI框架进行跨平台)

//email:718987717@qq.com //qq群: 758485934

编译方式
1.下载源码 使用git命令
	git clone https://github.com/NewYoungCode/EzUI.git
2.下载并且安装cmkae https://cmake.org/download/ 
	安装 Windows x64 Installer:	cmake-3.26.3-windows-x86_64.msi
3.使用cmake编译
	打开cmake-gui程序
	选择源码目录(包含CMakelists.txt文件的目录)
	点击"Configure"按钮
	选择输出目录
	编译时ide建议请选择vs2019
	编译时选择32
	点击"Generate"按钮
	点击"Open Project"按钮
5.编译运行
	请将这个文件 "kuGou运行依赖(需要将此解压到exe所在位置).zip" 解压到编译完成的exe所在目录(运行资源以及动态链接库)


窗口类
	Window            //经典带边框带系统菜单WIN32窗口样式
	BorderlessWindow //无边框 带阴影 不透明窗口   屏幕外拖入会重绘     常规接受WM_PAINT消息 
	LayeredWindow	  //无边框  带阴影 窗口透明异形 窗口大小发生改变重绘 自定义UI_PAINT消息(定时检测无效区域) 不允许有子窗口 
	注:调用Show()函数之前 所有窗口必须设置布局 使用 SetLayout()进行设置布局
控件类:
	所有控件都继承于"Control"
	属性 函数:
		EzUI::Margin Margin;//外边距 让容器独占一行 或 一列的情况下 设置边距会使控件变小 不可设置为负数
		WindowData* PublicData = NULL;//窗口上的公共数据
		EzUI::Cursor Cursor = EzUI::Cursor::None;//鼠标样式
		int MousePassThrough = 0;//忽略的鼠标消息
		const bool IsXmlControl = false;//是否是xml加载进来的
		bool Visible = true;//控件是否可见
		EString Name;//控件的ObjectName ID
		ControlState State = ControlState::Static;//控件状态
		ControlAction Action = ControlAction::None;//控件行为
		ControlStyle Style;//默认样式
		ControlStyle HoverStyle;//鼠标悬浮样式
		ControlStyle ActiveStyle;//鼠标按下样式
		Control* Parent = NULL;//父控件
		Controls VisibleControls;//基于控件中的可见控件
		DockStyle Dock = DockStyle::None;//dock样式
		const Rect ClipRect;//控件在窗口中的可见区域
	public:
		EventMouseMove MouseMove;//移动事件
		EventMouseEnter MouseEnter;//移入事件
		EventMouseWheel MouseWheel;//滚轮事件
		EventMouseLeave MouseLeave;//鼠标离开事件
		EventMouseDown MouseDown;//鼠标按下事件
		EventMouseUp MouseUp;//鼠标抬起
		EventMouseClick MouseClick;//鼠标单击
		EventMouseDoubleClick MouseDoubleClick;//鼠标双击
		EventPaint Painting;//绘制事件
	protected:
		ControlStyle& GetStyle(const ControlState& _state);//获取当前控件状态下的样式信息
		virtual void OnPaint(PaintEventArgs& args);//绘制 
		virtual void ChildPainting(Controls& controls, PaintEventArgs& args);//子控件绘制 可以重载此函数优化鼠标操作性能
		virtual void OnBackgroundPaint(PaintEventArgs& painter);//背景绘制
		virtual void OnForePaint(PaintEventArgs& e);//前景绘制
		virtual void OnBorderPaint(PaintEventArgs& painter);//边框绘制
		virtual void OnLoad();//控件第一次加载 警告 此函数在LayerWindow里面不允许在函数内添加控件 但是允许设置控件参数  
		virtual void OnLocation(const Point& pt);//坐标发生改变
		virtual void OnSize(const Size& size) override;//大小发生改变
		virtual void OnLayout();//布局代码在此 布局完成之后PendLayout设置成false
		virtual void OnKeyBoardEvent(const KeyboardEventArgs& _args);//键盘事件消息
		virtual void OnMouseMove(const Point& point);//鼠标在控件上移动
		virtual void OnMouseLeave();//鼠标离开控件
		virtual void OnMouseWheel(short zDelta, const Point& point);//鼠标滚轮
		virtual void OnMouseDown(MouseButton mbtn, const Point& point);//鼠标按下
		virtual void OnMouseUp(MouseButton mbtn, const Point& point);//鼠标弹起
		virtual void OnMouseClick(MouseButton mbtn, const Point& point);//鼠标单击
		virtual void OnMouseDoubleClick(MouseButton mbtn, const Point& point);//鼠标双击
		virtual void OnMouseEnter(const Point& point);//鼠标移入
		virtual void OnMouseEvent(const MouseEventArgs& args);//鼠标事件消息
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;//WM_CAHR消息
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam) override;//WM_CAHR消息
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);//键盘弹起
	public:
		//以下函数请保证在父控件布局已完成的情况下使用 使用ResumeLayout()执行布局
		const int& X();
		const int& Y();
		const int& Width();
		const int& Height();
		void SetX(const int& X);
		void SetY(const int& Y);
		void SetLocation(const Point& pt);//移动相对与父控件的位置
		void SetSize(const Size& size); //当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetFixedSize(const Size& size); //设置绝对宽高
		void SetWidth(const int& width);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetHeight(const int& height);//当重绘控件时不建议多次使用 影响性能(会调用SetRect函数)
		void SetFixedWidth(const int& fixedWidth);//设置绝对宽度
		void SetFixedHeight(const int& fixedHeight);//设置绝对高度
		const int& GetFixedWidth();//获取绝对宽度
		const int& GetFixedHeight();//获取绝对高度
		virtual const Rect& GetRect();//获取相对与父控件矩形 布局计算后
		Rect GetClientRect();//获取基于客户端的矩形
		bool IsPendLayout();//是否含有挂起的布局
		void TryPendLayout();//尝试挂起布局
		void EndLayout();//结束布局
		void SetRect(const Rect& rect);//设置相对父控件矩形
		virtual void ResumeLayout();//直接进行布局
		virtual void SetTips(const EString& text);
		virtual void OnKillFocus();//失去焦点的时候发生
		virtual void OnRemove();//被移除该做的事情
		void Trigger(const MouseEventArgs& args);//触发鼠标相关消息
		void Trigger(const KeyboardEventArgs& args);//触发键盘相关消息
		void AddEventNotify(int eventType);//添加到主窗口ontify函数中可拦截
		void RemoveEventNotify(int eventType);//移除特定的通知消息 ontify函数中不再接收
		void SetCursor(const EString& fileName);//设置鼠标样式
		virtual ScrollBar* GetScrollBar();//获取滚动条
		HCURSOR GetCursor();
	public:
		Control();
		virtual ~Control();
		void DestroySpacers();
		//普通样式
		HImage GetForeImage(ControlState _state = ControlState::None);
		HImage GetBackgroundImage(ControlState _state = ControlState::None);
		UI_Int GetRadius(ControlState _state = ControlState::None);
		UI_Int GetBorderLeft(ControlState _state = ControlState::None);
		UI_Int GetBorderTop(ControlState _state = ControlState::None);
		UI_Int GetBorderRight(ControlState _state = ControlState::None);
		UI_Int GetBorderBottom(ControlState _state = ControlState::None);
		Color GetBorderColor(ControlState _state = ControlState::None);
		Color GetBackgroundColor(ControlState _state = ControlState::None);
		//具有继承性样式
		EString GetFontFamily(ControlState _state = ControlState::None);//获取默认控件状态下字体Family
		UI_Int GetFontSize(ControlState _state = ControlState::None);//获取默认控件状态下字体大小样式
		Color GetForeColor(ControlState _state = ControlState::None);//获取默认控件状态下前景色
		virtual void SetStyleSheet(const EString& styleStr, ControlState _state = ControlState::Static);//
		virtual void SetAttribute(const EString& attrName, const EString& attrValue);//基础控件设置属性
		Controls& GetControls();//获取当前所有子控件
		Control* GetControl(size_t pos);//使用下标获取控件
		Control* FindControl(const EString& objectName);//寻找子控件 包含孙子 曾孙 等等
		Controls FindControl(const EString& attr, const EString& attrValue);//使用属性查找
		size_t Index();
		virtual void AddControl(Control* ctl);//添加控件
		virtual ControlIterator RemoveControl(Control* ctl);//删除控件 返回下一个迭代器
		virtual void Clear(bool freeControls = false);//清空当前所有子控件, freeControls是否释放所有子控件
		virtual void Rending(PaintEventArgs& args);//绘制函数
		bool IsVisible();//当前是否显示在窗口内
		virtual bool Invalidate();// 使当前控件的区域为无效区域
		virtual void Refresh();// 使当前控件区域为无效区域并且立即更新全部的无效区域
		void ComputeClipRect();//计算基于父控件的裁剪区域

有建议或者发现bug可以反馈给我,使用愉快~