# EzUI

**EzUI** 是一个基于原生 Win32 消息机制和 Direct2D 的高性能桌面 UI 框架，具备强大的弹性布局、伪类样式支持和控件系统，目标是提供如 Web 前端般灵活、直观的界面开发体验。

> 🚀 支持高 DPI、分层窗口、响应式布局、CSS 风格皮肤、GIF 动画、控件组合与继承机制。未来将支持跨平台 2D 图形库扩展。

---

## ✨ 框架特色

- 🪱 **基于 Win32 消息机制**：轻量、无依赖，逻辑控件系统拦截并下发鼠标键盘消息
- 🎨 **2D 图形绘制**：当前基于 Direct2D 绘图，具备高性能、高分辨率适配能力。
- 🧹 **弹性布局系统**：支持自动宽高、自适应布局，开发体验类比前端 Flex 设计。
- 🎭 **伪类 CSS 支持**：支持 `hover`、`active`、`checked` 等状态样式，类选择器、ID 选择器、组合选择器等完整选择器系统。
- 🌟 **控件组合系统**：Label、Button、TextBox、CheckBox、RadioButton 等丰富控件可自由组合构建 UI。
- 💡 **事件系统**：支持事件冒泡机制，可实现事件捕获与穿透。
- 🧩 **高 DPI 适配**：框架自动处理 DPI 缩放与坐标换算，支持多显示器高分屏。
- 🪪 **多种窗口类型支持**：
  - `Window`：经典边框窗口 windows自动发送`WM_PAINT`消息绘制
  - `BorderlessWindow`：无边框、带阴影 windows自动发送`WM_PAINT`消息绘制
  - `LayeredWindow`：分层透明窗口，支持异形与实时重绘 手动发送 `WM_PAINT`消息进行绘制
  - `PopupWindow`：失焦自动关闭的弹出窗口，适用于菜单等

---

## 📆 快速开始

### 1. 下载源码

```bash
git clone https://github.com/NewYoungCode/EzUI.git
```

### 2. 安装 CMake

请前往 [CMake 官网](https://cmake.org/download/) 下载安装。

### 3. 构建与运行

- 构建 32 位项目：

```bash
build.bat
```

- 构建 64 位项目（未包含 kugou 项目）：

```bash
build64.bat
```

---

## 📀 使用说明

### 🏁 程序入口

请在 `WinMain` 函数中创建 `Application` 实例并调用 `app.Exec()` 以启动消息循环：

```cpp
#include <Windows.h>
#include "EzUI/UIManager.h"
#include "Ezui/Window.h"
#include "Ezui/Application.h"

using namespace ezui;

class TestForm :public Window {
private:
	UIManager umg;//ui管理器
public:
	TestForm() :Window(800, 600) {
		//umg.LoadXmlFile("res/form.htm");//从文件中加载xml界面
		umg.LoadXmlData("<vbox> <label text=\"hello world\"></label> </vbox>");//从内存中加载布局
		umg.SetupUI(this);//设置ui
	}
	virtual ~TestForm() {
	}
	virtual void OnClose(bool& bClose) override {
		int result = ::MessageBoxW(frm.Hwnd(), L"要退出程序吗？", L"提示", MB_YESNO | MB_ICONQUESTION);
		if (result == IDYES) {
			Application::Exit(0);//当窗口关闭时 整个程序退出
		} else {
			bClose=false;  // 用户点击了“否” 此标志设置为false将不会关闭
		}
	}
};

//程序入口;
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	Application app;

	TestForm testForm;
	testForm.SetText(L"我是测试窗口");
	testForm.Show(nCmdShow);

	return app.Exec();
};
```

### 🧱 控件与组件列表（部分）

- `Application`：EzUI 应用入口与消息循环控制器
- `Window`：经典 Win32 边框窗口
- `BorderlessWindow`：无边框带阴影窗口
- `LayeredWindow`：支持透明和异形的分层窗口
- `PopupWindow`：失焦自动关闭的弹出窗口
- `Control`：所有控件基础类，封装鼠标命中、事件分发和重绘逻辑
- `Label`：文字标签控件
- `Button`：标准按钮控件
- `TextBox`：文本输入框
- `CheckBox` / `RadioButton`：复选框 / 单选框控件
- `PictureBox`：图片显示控件，支持 GIF
- `ComboBox`：下拉框控件
- `Menu`：菜单支持
- `NotifyIcon`：托盘图标支持
- `ScrollBar` / `HScrollBar` / `VScrollBar`：滚动条组件
- `HLayout` / `VLayout`：水平/垂直布局容器
- `HListView` / `VListView`：横向/纵向列表视图容器
- `TileListView`：瓦片式列表视图
- `TabLayout`：选项卡切换容器
- `Spacer`：空隙组件，占位用
- `ShadowBox`：为无边框窗口提供窗口阴影
- `IFrame`：内嵌页面控件，功能类似 Web 前端中的 `<iframe>`
- `UIDef`：框架内使用的宏定义集合
- `UIManager`：UI 样式与资源统一管理
- `UISelector`：选择器匹配系统，实现类选择器、链式查询控件
- `UIString` / `tinystr.h` / `tinyxml.h`：字符串处理与 XML 配置解析支持
- `RenderTypes`：绘图相关类型定义（颜色、对齐方式等）
- `Direct2DRender`：Direct2D 绘图实现类
- `Bitmap`：图像资源封装
- `Resource.h`：自定义资源管理类（非 VS rc 文件），用于手动加载与管理框架资源
- `Timer` / `Task`：定时与异步任务处理辅助类
- `EzUI.h`：框架主接口头文件，定义事件类、枚举、全局资源等核心结构

### 🎨 样式示例

```css
#submitBtn {
    width: 100px; /*静态样式时支持调整大小*/
    height: 30px; /*静态样式时支持调整大小*/
    background-color: #0078d7;
}

#submitBtn:hover {
    background-color: #005a9e;
}

.classA .classB {  /*多选择器*/
    color:#ffffff;
}

label{  /*标签选择器*/
    color:#ffffff;
}

.check:checked {
    border:1px;
    border-radius:10px;
    border-color: #005a9e;
}
```

---

## 🔧 调试技巧

- 在 Debug 模式下运行时，按下 `F11` 可实时查看布局信息，高亮显示控件边界。

---

## 🧬 开发理念

- **“一切皆控件”**：无 UI 描述文件，纯代码式组件组合；
- **“CSS 驱动视觉”**：结构和样式分离；
- **“面向前端思维”**：布局、交互风格向 Web 靠拟；
- **“跨平台可拟扩”**：绘图模块可替换（未来可能会使用跨平台图形库）；

---

## 📖 学习 & 技术支持

- 视频教程：https://space.bilibili.com/240269358/video
- QQ：718987717
- QQ群：758485934
- 邮箱：[19980103ly@gmail.com]/[19980103ly@gmail.com]
- 微信：wx19980103yon

---

## 📄 License


