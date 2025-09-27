# EzUI 框架使用说明

**EzUI 2.0** 是一个基于原生 Win32 消息机制和 Direct2D 的高性能桌面 UI 框架，具备强大的弹性布局、伪类样式支持和控件系统，目标是提供如 Web 前端般灵活、直观的界面开发体验。

> 🚀 **新增与优化特性（2.0）**  
> - 性能大幅提升，布局与绘制速度优化  
> - 修复旧版本代码 bug 与潜在隐患  
> - 布局新增精度补偿机制，解决间隙问题  
> - 布局支持 `min-width` / `max-width` / `min-height` / `max-height`  
> - 控件新增状态属性，提高可控性  
> - 增强动画效果，提升用户体验  
> - 支持高 DPI、分层窗口、响应式布局、CSS 风格皮肤、GIF 动画、控件组合与继承机制


## ✨ 框架特色

- 🪱 **基于 Win32 消息机制**：轻量、无依赖，逻辑控件系统拦截并下发鼠标键盘消息
- 🎨 **2D 图形绘制**：当前基于 Direct2D 绘图，具备高性能、高分辨率适配能力。
- 🧹 **弹性布局系统**：支持自动宽高、自适应布局，开发体验类比前端 Flex 设计。
- 🎭 **伪类 CSS 支持**：支持 `hover`、`active`、`checked`、`focus`、`disabled` 状态样式，类选择器、ID 选择器、组合选择器等选择器系统。
- 🌟 **控件组合系统**：Label、Button、TextBox、CheckBox、RadioButton 等丰富控件可自由组合构建 UI。
- 💡 **事件系统**：支持事件冒泡机制，可实现事件捕获与穿透。
- 🧩 **高 DPI 适配**：框架自动处理 DPI 缩放与坐标换算，支持多显示器高分屏。
- 💡 在 Debug 模式下运行时，按下 F11 可实时查看布局信息，高亮显示控件边界。

# EzUI 框架头文件说明

- **Animation.h**：动画效果相关接口与实现。
- **Application.h**：应用程序入口类，负责初始化、资源加载和消息循环。
- **Bitmap.h**：位图图像加载与处理。
- **BorderlessWindow.h**：无边框窗口类，带阴影效果。
- **Button.h**：按钮控件。
- **CheckBox.h**：复选框控件。
- **ComboBox.h**：下拉选择控件。
- **Control.h**：所有控件的基类，提供属性、样式和事件管理。
- **Direct2DRender.h**：Direct2D 渲染接口封装。
- **EzUI.h**：框架总入口头文件，包含核心功能。
- **Frame.h**：Frame 容器控件，用于布局或嵌套控件。
- **HLayout.h**：水平布局控件。
- **HListView.h**：水平列表控件。
- **HScrollBar.h**：水平滚动条控件。
- **Label.h**：文本标签控件。
- **LayeredWindow.h**：分层窗口类，支持透明和异形窗口。
- **Menu.h**：菜单控件及弹出菜单支持。
- **NotifyIcon.h**：系统托盘图标控件。
- **PagedListView.h**：分页列表控件。
- **PictureBox.h**：图片显示控件。
- **PopupWindow.h**：弹出窗口控件，失去焦点自动隐藏或关闭。
- **RadioButton.h**：单选按钮控件。
- **RenderTypes.h**：渲染相关基础类型定义。
- **Resource.h**：资源管理相关接口。
- **ScrollBar.h**：滚动条控件。
- **ShadowBox.h**：阴影窗口。
- **Spacer.h**：占位控件，用于布局间距。
- **TabLayout.h**：分页布局控件。
- **Task.h**：异步任务接口与实现。
- **TextBox.h**：文本输入框控件。
- **TileListView.h**：瓷砖式列表控件。
- **Timer.h**：定时器接口。
- **tinystr.h** / **tinyxml.h**：XML 第三方解析相关头文件。
- **TreeView.h**：树形列表控件。
- **UIDef.h**：UI 基础宏定义。
- **UILoader.h**：UI XML 加载器。
- **UISelector.h**：样式选择器接口。
- **UIString.h**：字符串类型封装。
- **UIStyle.h**：样式对象定义。
- **VLayout.h**：垂直布局控件。
- **VListView.h**：垂直列表控件。
- **VScrollBar.h**：垂直滚动条控件。
- **Window.h**：经典 Win32 窗口类。


# Application 类介绍

`Application` 是 EzUI 框架中程序入口管理类，用于管理窗口创建、资源加载以及消息循环。

---

## 创建要求

- 程序必须在 `WinMain` 或 `wWinMain` 函数中创建一个 `Application` 对象。
- 每个应用程序只能有一个 `Application` 实例。

---

## 主要功能

| 方法 | 说明 |
| ---- | ---- |
| `Application(HINSTANCE hInstance = NULL)` | 构造函数，可传入应用实例句柄 |
| `bool SetResource(const UIString& localOrResName)` | 使用本地文件或资源名称加载资源包。支持 VS 资源 ID 或本地文件路径。一个 `Application` 只允许有一个资源文件。 |
| `void EnableHighDpi()` | 启用高 DPI 适配 |
| `int Exec()` | 执行消息循环，启动程序运行 |
| `static void Exit(int exitCode = 0)` | 退出消息循环，结束程序 |
| `static UIString StartPath()` | 获取程序启动路径 |
| `~Application()` | 析构函数，释放资源 |

---

## 使用示例
### xml内容示例(文件位置:d:/res/form.xml)
```xml
<vbox>
    <label text="我是标签1,按住我移动窗口" action="title" style="cursor:pointer" class="label"></label>
    <label text="我是标签2" class="label"></label>
    <label text="我是标签3,点我关闭窗口" action="close" class="label"></label>
</vbox>
<style>
    /*类选择器*/
    .label {
        color: black;
        font-size: 13px;
    }
    /*统一设置鼠标悬浮样式*/
        .label:hover {
            font-size: 15px;
        }
</style>
```

```cpp
#include <Windows.h>
#include "Ezui/EzUI.h"
#include "Ezui/Window.h"
#include "Ezui/Application.h"

using namespace ezui;

class MainFrm :public Window {
public:
	MainFrm() :Window(800, 600) {
		this->LoadXml("d:/res/form.xml");//从文件中加载xml界面
	}
	virtual ~MainFrm() {
	}
	//当窗口被关闭的时候发生
	virtual void OnClose(bool& bClose) override {
		int result = ::MessageBoxW(frm.Hwnd(), L"要退出程序吗？", L"提示", MB_YESNO | MB_ICONQUESTION);
		if (result == IDYES) {
			Application::Exit(0);//当窗口关闭时 整个程序退出
		} else {
			bClose=false;  // 用户点击了“否” 此标志设置为false将不会关闭窗口
		}
	}
};
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    
    Application app;// 创建应用程序对象
    app.EnableHighDpi(); // 启用高 DPI 适配
    app.SetResource("my_res");// 设置资源包（VS 自定义资源名称或本地文件）
	
    MainFrm frm;// 创建主窗口
    frm.CenterToScreen(); // 屏幕窗口居中
    frm.Show(); // 显示窗口
	
    return app.Exec();// 执行消息循环
}
```
---

# 窗口类介绍

EzUI 框架提供四种主要窗口类，用于不同的窗口类型和效果需求：

---

## 1. `Window`

- **类型**：经典 Win32 窗口  
- **特点**：
  - 带系统标题栏
  - 标准边框和系统菜单
  - 适用于传统应用场景

---

## 2. `BorderlessWindow` 继承于Window类

- **类型**：无边框窗口  
- **特点**：
  - 没有系统边框和标题栏
  - 支持自定义阴影
  - 常用于自定义界面或美化窗口外观

---

## 3. `LayeredWindow` 继承于BorderlessWindow类

- **类型**：分层窗口  
- **特点**：
  - 支持异形窗口和圆角
  - 完全透明背景，可绘制任意形状
  - 适用于悬浮窗、特效窗或复杂 UI 场景

### 4. `PopupWindow` (继承于LayeredWindow类) 是一种浮动窗口，用于显示临时信息、菜单或工具提示。特点如下：  

- **浮动显示**：窗口不依附于主窗口布局，可以在任意位置显示。
- **自动关闭/隐藏**：
  - **模态方式**启动：失去焦点后自动关闭窗口。
  - **非模态方式**启动：失去焦点后自动隐藏窗口。
- **适合场景**：
  - 下拉菜单
  - 工具提示（Tooltip）
  - 临时弹出设置面板



# 控件类介绍 

## Control类(所有控件的基础类)
### 基本属性(通过xml或者Control::SetAttribute函数进行设置)
| 属性名 | 说明 |
|--------|------|
| `name` / `id` | 控件的唯一标识，用于查找和引用。(参考web前端的css样式) |
| `class`  | 控件的类名，用于可以用于样式批量设置。(参考web前端的css样式) |
| `x` | 控件 X 坐标（可乘缩放系数计算）。 |
| `y` | 控件 Y 坐标（可乘缩放系数计算）。 |
| `location` | 控件位置，可使用 `"x,y"` 格式。 |
| `width` | 控件宽度，可使用绝对值、`auto` 或百分比（如 `"50%"`）。 |
| `height` | 控件高度，可使用绝对值、`auto` 或百分比（如 `"50%"`）。 |
| `size` | 控件尺寸，可使用 `"width,height"` 或 `"auto"`。 |
| `rect` | 控件矩形区域，格式 `"x,y,width,height"`。 |
| `margin` | 控件外边距，可使用单值、双值或四值，支持 web 风格语法（如 `"10"`, `"10 20"`, `"10 20 30 40"`）。 |
| `margin-left` / `margin-top` / `margin-right` / `margin-bottom` | 分别设置控件的单侧外边距。 |
| `visible` | 控件可见性，`false` 隐藏控件。 |
| `display` | 控件显示方式，`none` 隐藏控件，其他值显示控件。 |
| `float` | 控件浮动，`true` 启用浮动布局。 |
| `action` | 控件行为类型，例如窗口按钮：`close`、`mini`、`max`、`title`、`move` / `movewindow`。 |
| `enable` | 是否启用控件，`true` 或 `false`。 |
| `disabled` | 禁用控件，等效于 `enable="false"`。 |
| `event` | `none` 忽略鼠标键盘事件,鼠标穿透效果 |
| `style` | 内联样式字符串，优先级最高。 |
| `scrollbar` | 绑定控件的滚动条对象，通过名称引用滚动条控件。 |
---

### Control类样式(控件状态)
| 状态 | 说明 | 示例样式 |
| ---- | ---- | -------- |
| Static | 默认状态样式 | `#btn{color: #000000; background-color: #FFFFFF; border: 1px solid #CCCCCC;}` |
| Hover | 鼠标悬浮在控件上样式 | `#btn:hover{color: #000000; background-color: #F0F0F0; border-color: #999999;}` |
| Active | 鼠标按下控件样式 | `#btn:active{color: #FFFFFF; background-color: #0078D7; border-color: #005A9E;}` |
| Focus | 鼠标按下控件样式 | `#btn:focus{color: #FFFFFF; background-color: #0078D7; border-color: #005A9E;}` |
| Disabled | 控件被禁用样式 | `#btn:disabled{color: #AAAAAA; background-color: #EEEEEE; border-color: #CCCCCC;}` |

Control 支持的样式属性包括：

| 样式名                                                                                                       | 说明                                                    |
| --------------------------------------------------------------------------------------------------------- | ----------------------------------------------------- |
| pointer-events                                                                                            | "none" 忽略鼠标键盘事件,鼠标穿透效果                                       |
| min-width / max-width                                                                                     | 控件最小/最大宽度                                             |
| min-height / max-height                                                                                   | 控件最小/最大高度                                             |
| opacity                                                                                                   | 控件透明度(0~1.0)                                                 |
| cursor                                                                                                    | "pointer" / "help" / "n-resize" / "e-resize" / "move" |
| background-color                                                                                          | 背景颜色                                                  |
| background-image                                                                                          | 背景图片                                                  |
| background-position                                                                                       | 背景图片位置                                                |
| background-size / background-image-size                                                                   | 背景图片尺寸                                                |
| fore-image                                                                                                | 前景图片                                                  |
| fore-image-size                                                                                           | 前景图片尺寸                                                |
| color / fore-color                                                                                        | 前景颜色                                                  |
| border-color                                                                                              | 边框颜色                                                  |
| border-style                                                                                              | "solid" / "dashed"    (实线/虚线)                                |
| border-radius                                                                                             | 四角圆角半径                                                |
| border-top-left-radius / border-top-right-radius / border-bottom-left-radius / border-bottom-right-radius | 单角圆角半径                                                |
| border-width / border                                                                                     | 边框宽度（可单边或统一设置）                                        |
| border-left / border-top / border-right / border-bottom                                                   | 单边边框宽度                                                |
| font-size                                                                                                 | 字体大小                                                  |
| font-weight                                                                                               | "bold" 或数字权重                                          |
| font-family                                                                                               | 字体名称                                                  |
| x / y                                                                                                     | 控件位置                                                  |
| width / height                                                                                            | 宽度 / 高度，可用百分比或 "auto"                                 |
| margin / margin-left / margin-top / margin-right / margin-bottom                                          | 外边距，支持 CSS 写法                                         |
| display                                                                                                   | "none" 隐藏控件                                           |

### 样式示例
```css
#submitBtn { /*设置默认样式*/
    width: 100px; /*静态样式时支持调整大小*/
    height: 30px; /*静态样式时支持调整大小*/
    background-color: #0078d7;
}

#submitBtn:hover {  /*鼠标悬浮样式*/
    background-color: #005a9e;
}

.classA,.classB {  /*为多个类名设置样式*/
    color:#ffffff;
}

.check:checked { /*选中样式*/
    border:1px;
    border-radius:10px;
    border-color: #005a9e;
}
```

---

### Control 方法与属性概览

- 位置与大小管理（X/Y/Width/Height/SetSize/SetRect/SetRateSize 等）
- 父子控件管理（AddChild/RemoveChild/FindControl/FindChildren/SetParent 等）
- 样式设置（Style/HoverStyle/FocusStyle/ActiveStyle/DisabledStyle, SetStyle, SetStyleSheet）
- 布局管理（OnLayout, RefreshLayout, EndLayout, IsPendLayout 等）
- 事件处理（Mouse/Keyboard/Focus/OnEvent/SendEvent 等）
- 可见性与浮动管理（SetVisible/Show/Hide/SetFloat/IsFloat）
- 光标管理（GetCursor, SetCursor）
- 边距管理（SetMargin/GetMargin）
- 内容尺寸管理（GetContentSize/SetContentWidth/SetContentHeight）
- 滚动条支持（GetScrollBar）
- 焦点控制（SetFocus）


### Control 类函数与属性补充说明


#### 样式设置函数

| 方法 | 说明 |
| ---- | ---- |
| ApplyStyleProperty(key, value) | 为静态样式应用单个属性 |
| SetStyle(style, key, value) | 为指定状态样式设置属性 |
| SetStyleSheet(state, styleStr) | 为指定状态批量设置样式 |
| SetStyleSheet(styleStr) | 设置样式集合并匹配应用到子控件 |

#### 布局与大小函数

| 方法 | 说明 |
| ---- | ---- |
| SetContentWidth(width) | 设置内容宽度（子类） |
| SetContentHeight(height) | 设置内容高度（子类） |
| SetContentSize(size) | 设置内容尺寸（子类） |
| OnLayout() | 控件布局逻辑，可重写 |
| RefreshLayout() | 立即刷新布局 |
| SetRect(rect) | 设置相对父控件矩形 |
| SetSize(size) / SetFixedSize(size) | 设置控件大小 |
| SetWidth(width) / SetHeight(height) | 设置宽高 |
| SetFixedWidth(width) / SetFixedHeight(height) | 设置绝对宽高 |
| SetRateWidth(rate) / SetRateHeight(rate) | 设置百分比宽高 |
| SetAutoWidth(flag) / SetAutoHeight(flag) / SetAutoSize(flag) | 自动大小控制 |
| GetContentSize() | 获取内容大小 |
| GetSize() | 获取控件大小 |
| GetLocation() | 获取控件位置 |
| GetRect() / GetRectInClient() / GetRectInScreen() / GetRectInFrame() | 获取控件矩形 |

#### 可见性与浮动

| 方法 | 说明 |
| ---- | ---- |
| SetVisible(flag) / IsVisible() | 设置/获取控件可见性 |
| Show() / Hide() | 显示/隐藏控件 |
| SetFloat(flag) / IsFloat() | 设置/获取控件浮动状态,浮动状态下控件不会参与布局 |
| SetHitTestVisible(flag) / IsHitTestVisible() | 设置/获取鼠标穿透状态 |

#### 子控件操作

| 方法 | 说明 |
| ---- | ---- |
| AddChild(child) | 添加子控件到末尾 |
| Append(UIString) | 解析xml字符并插入控件到末尾 |
| Prepend(UIString) | 解析xml字符并插入到子控件到最前面 |
| InsertChild(pos, child) | 插入子控件 |
| RemoveChild(child, freeCtrl) | 移除子控件 |
| RemoveAll(freeAll) | 移除所有子控件 |
| SwapChildren(child1, child2) | 交换两个子控件位置 |
| GetControls() | 获取所有子控件 |
| GetControl(pos) | 使用下标获取控件 |
| Contains(ctrl) | 是否包含指定控件 |
| IndexOf(child) | 获取子控件索引 |
| FindControl(name) / FindControls(attr, value) | 查找控件 |
| FindChild(name) / FindChildren(attr, value) | 查找子控件（直接子集） |

#### 边距与布局控制

| 方法 | 说明 |
| ---- | ---- |
| SetMargin(...) | 设置四周边距（重载支持不同组合） |
| GetMargin() | 获取控件边距信息 |
| TryPendLayout() | 尝试挂起布局 |
| EndLayout() | 结束布局使其生效 |
| IsPendLayout() | 是否存在挂起布局 |
| GetLayoutState() | 获取当前布局状态 |

#### 边框与样式获取

| 方法 | 说明 |
| ---- | ---- |
| GetBorderTopLeftRadius() / GetBorderTopRightRadius() / GetBorderBottomRightRadius() / GetBorderBottomLeftRadius() | 获取圆角半径 |
| GetBorderLeft() / GetBorderTop() / GetBorderRight() / GetBorderBottom() | 获取边框宽度 |
| GetBorderColor() | 获取当前边框颜色 |
| GetBorderStyle() | 获取当前边框样式 |
| GetForeImage() | 获取当前前景图片 |
| GetBackImage() | 获取当前背景图片 |
| GetBackColor() | 获取当前背景颜色 |
| GetForeColor() | 获取当前前景颜色(字体颜色) |
| GetFontFamily() / GetFontSize() / GetFontWeight() | 获取当前字体属性 |
| GetOpacity() | 获取当前透明度 |
| GetAngle() | 获取当前旋转角度 |
| GetCursor() | 获取当前鼠标光标 |

#### 事件相关

| 方法 | 说明 |
| ---- | ---- |
| OnEvent(arg) | 所有事件优先进入此函数 |
| SendEvent(arg) | 派发事件 |
| OnMouseMove / OnMouseLeave / OnMouseWheel / OnMouseDown / OnMouseUp / OnMouseDoubleClick / OnMouseEnter | 鼠标事件 |
| OnMouseEvent | 鼠标事件统一入口 |
| OnKeyChar / OnKeyDown / OnKeyUp / OnKeyBoardEvent | 键盘事件 |
| OnFocus / OnKillFocus | 焦点事件 |
| OnRemove | 被移除时执行的逻辑 |

#### 其他实用函数

| 方法 | 说明 |
| ---- | ---- |
| Attach(img) / Detach(img) | 绑定/解绑图片 (绑定之后生命周期跟随当前对象,无其他功能) |
| Hwnd() / SetHwnd() | 获取/设置窗口句柄 |
| SetTips(text) / GetTips() | 设置/获取提示文字 |
| GetScrollBar() | 获取控件滚动条对象 |
| IsPressed() | 控件是否被按住 |
| SetFocus() | 设置控件为焦点控件 |

# 布局控件

EzUI 提供多种布局控件，用于管理子控件的排列和尺寸，包括 **VLayout、HLayout、TabLayout**。

---

## VLayout（垂直布局）

**功能**：  
垂直排列子控件，支持水平对齐、间距、内外边距和自动调整子控件高度。

**主要属性**：

- `halign / align`：子控件水平对齐方式 `"left" / "center" / "right"`  
- 自动宽度/高度：支持自动适应子控件尺寸

---

## HLayout（水平布局）

**功能**：  
水平排列子控件，支持垂直对齐、间距、内外边距和自动调整子控件宽度。

**主要属性**：

- `valign / align`：子控件垂直对齐方式 `"top" / "middle" / "bottom"`  
- 自动宽度/高度：支持自动适应子控件尺寸

---

## TabLayout（选项卡布局）

**功能**：  
管理多个 Tab 页，每页可包含不同控件，实现分页显示。

**主要属性**：
- `tabindex` 可控制默认显示页。

---

# 列表控件（带滚动条）
EzUI 提供三种列表控件，用于显示多项内容，分别为 **VListView、HListView、TileListView**。

---

## VListView（垂直列表）

**功能**：  
垂直排列子项，带滚动条)可滚动显示，常用于聊天列表、菜单列表等。

**主要属性**：

- `halign / align`：子控件水平对齐 `"left" / "center" / "right"`  


---

## HListView（水平列表）

**功能**：  
水平排列子项，(带滚动条)可滚动显示，常用于图标列表或工具栏列表。

**主要属性**：

- `valign / align`：子控件垂直对齐 `"top" / "middle" / "bottom"`  

---

## TileListView（平铺列表/ 流动列表 / 瓷砖列表）

**功能**：  
网格排列子项，类似图标平铺，适合相册、图库或应用图标界面(带滚动条)。
子控件必须指定宽高


## Frame控件

| 属性名 | 说明         |
| --- | ---------- |
| src | 加载 XML 内联子页面 (Frame页面内的子控件与外部隔离,无法匹配样式 也不可以通过FindControl等函数进行查找)| 


## Spacer/VSpacer/HSpacer控件 占位控件(类似于QT的弹簧控件)
xml中的用法:
 ```xml
 <vbox> <!--垂直布局-->
    <spacer></spacer>  <!--占位控件,自动拉伸 会自动挤压label标签-->
    <label text="我是一个标签"></label>
    <spacer height="50"></spacer> <!--占位控件:占固定高度50像素-->
</vbox>
 ```


## Label控件

| 属性名                     | 说明                     |
| ----------------------- | ---------------------- |
| valign / halign / align | 设置文本对齐left,top,right,bottom,center,middle              |
| text                    | 文本内容                   |
| underline               | 下划线位置与长度 "start,count" |
| ellipsis                | 设置省略文本          |


## CheckBox控件 继承于Label

| 属性名     | 说明               |
| ------- | ---------------- |
| checked | "true" / "false" |

(控件状态及样式)
| 状态 | 说明 | 示例样式 |
| ---- | ---- | -------- |
| Checked | 控件获得焦点时样式 | `#btn:checked{color: #000000; background-color: #FFFFFF; border-color: #0078D7;}` |



## TextBox控件

| 属性名                     | 说明               |
| ----------------------- | ---------------- |
| valign / halign / align | 文本对齐             |
| passwordchar            | 密码掩码字符           |
| placeholder             | 占位文本             |
| text / value            | 文本内容             |
| readonly                | "true" / "false" |
| multiline               | "true" / "false" |


## RadioButton控件 继承于CheckBox

- 鼠标点击后自动设置自身为选中，并取消同父控件下其它 RadioButton 的选中状态

- 
## PictureBox控件

| 属性名 | 说明      |
| --- | ------- |
| src | 图片路径 (支持gif动图)


## Button控件 继承于Label

- 继承 Label  
- 鼠标默认光标为手型  


## 📖 学习 & 技术支持

- 视频教程：https://space.bilibili.com/240269358/video
- QQ：718987717
- QQ群：758485934
- 邮箱：[19980103ly@gmail.com]/[19980103ly@gmail.com]
- 微信：wx19980103yon

---


# EzUI License

**版本**：2.0（暂时闭源）  
**适用系统**：Windows 7 及以上

## 使用说明

1. EzUI 框架当前 **暂时闭源**，提供 **完整的编译后文件**（DLL / LIB / 头文件），可直接用于开发。
2. 允许在商业项目或个人项目中使用这些编译文件。
3. 框架在 Windows 7 及以上系统中测试可用。

---

> **说明**：本 License 表示暂时闭源状态，框架提供的是编译后的使用版本。



