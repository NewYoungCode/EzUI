WIN32 desktop application UI framework (2d graphical library:direct2d,后期可能会采用其他跨平台的2d图形库对整个UI框架进行跨平台)

编译方式

1.下载源码 使用git命令

	git clone https://github.com/NewYoungCode/EzUI.git

2.下载并且安装cmkae https://cmake.org/download/ 

	安装 Windows x64 Installer:	cmake-3.26.3-windows-x86_64.msi

3.编译运行

	请将这个文件 "kuGou运行依赖(需要将此解压到exe所在位置).zip" 解压到编译完成的exe所在目录(运行资源以及动态链接库)

简介:

原理:基于windows消息机制,所有控件都是逻辑控件 处理鼠标键盘对控件的命中消息并进行下发 响应WM_PAINT消息进行重绘 分层窗口则采用一个线程进行检测无效区域并进行重绘

高分辨率适配 使用direct2d进行高性能绘图 

可灵活使用控件组成大部分想要的效果(combox,datagridview,tree)将轻易实现

css普通进行修饰控件外观  模仿前端的继承样式 hover,active,checked伪样式 id选择器,类选择器,选择器可多选

弹性布局 自动宽高 布局代码可以像写web前端一样轻松高DPI的适配 GIF显示

窗口类

Window            //经典带边框带系统菜单WIN32窗口样式

BorderlessWindow :public Window //继承于Window 无边框 带阴影 

LayeredWindow :public BorderlessWindow	  //分层窗口 继承于Window 无边框 带阴影 窗口透明异形 窗口大小发生改变重绘 (自定义绘制机制)

PopupWindow :public LayeredWindow    //继承于LayeredWindow 特点(弹出式窗口 窗口失去焦点将会自动关闭,可用于做弹出式菜单)

注:所有窗口调用Show()函数之前 所有窗口必须设置布局 使用 Window::SetLayout()进行设置布局

常用控件(类所有控件都继承于"Control"):

Label(带文字的控件)，Button(按钮),TextBox(文字输入框),HLayout(水平布局),VLayout(垂直布局),HList(水平排列),VList(垂直排列),TileList(瓦片布局),TabLayout(视图切换),PictureBox(可用于显示GIF),CheckBox(复选框),RadioButton(单选框) 


Tips:
    Debug模式运行程序按下F11可查看当前布局信息 每个控件将高亮显示边框

学习&技术支持 

框架学习:https://space.bilibili.com/240269358/video

QQ:718987717 //qq群:758485934 //email:19980103ly@gmail.com //微信:

<img src="https://github.com/NewYoungCode/EzUI/blob/master/wx.jpg?raw=true" width=400 height=565 />
