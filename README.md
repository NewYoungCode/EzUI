<p align="center">
  <h1 align="center">EzUI</h1>
  <p align="center">基于 Win32 + Direct2D 的桌面 UI 框架</p>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/platform-Windows%207+-blue" />
  <img src="https://img.shields.io/badge/render-Direct2D-orange" />
  <img src="https://img.shields.io/badge/version-2.0-green" />
  <img src="https://img.shields.io/badge/license-暂时闭源-red" />
</p>

---

**EzUI 2.0** 是一套基于原生 Win32 消息机制和 Direct2D 的桌面 UI 框架。它提供声明式 XML 控件树、CSS 风格样式子集、状态伪类、资源包、动画、自绘和常见 Shell 组件，适合 Windows 桌面应用开发，而不是浏览器页面开发。

## ✨ 特性概览

- **Win32 原生窗口体系**：保留桌面程序的窗口、消息循环、模态流程和系统交互能力。
- **Direct2D 渲染**：支持高 DPI、透明窗口、图片/GIF、自绘和渐变相关能力。
- **声明式 UI**：通过 `LoadXml(...)` / `UILoader` 加载 `.htm` 控件树，支持 XML-first、code-first 和 hybrid 三种组合方式。
- **XML 解析**：使用第三方库 `tinyxml` 提供 XML 解析功能。
- **布局与列表**：内置 `VLayout`、`HLayout`、`VListView`、`HListView`、`TileListView`、`DataGridView`、`TabControl`、`TreeView`、`Slider`、`RichTextView` 等控件。
- **状态样式系统**：支持 ID、class、伪状态以及内联样式覆盖。
- **资源打包**：支持 `Resource::Package(...)` 打包资源目录，并通过 `Application::SetResource(...)` 统一挂载。
- **线程与交互基础设施**：提供 `Thread`、`ThreadPool`、`Timer`、`DebounceTimer`、`MessageQueue`、`Invoke(...)`、`BeginInvoke(...)`。
- **Shell 组件**：提供 `PopupMenu`、`SystemMenu`、`ToolTip`、`TrayIcon`、`WindowShadow`。
- **调试辅助**：Debug 下按 `F11` 可切换控件边界高亮，便于排查布局问题。

---

## 🚀 快速开始

### `res/mainForm.htm`

```xml
<vbox style="padding: 24px; background-color: #f5f7fb;">
    <label text="EzUI 2.0"
           style="font-size: 24px; font-weight: bold; color: #20242c;"></label>

    <label text="这里的 .htm 是控件树，不是浏览器 HTML 页面"
           style="margin-top: 8px; color: #5a6472;"></label>

    <spacer height="16"></spacer>

    <button id="btnClose"
            text="关闭窗口"
            action="close"
            style="width: 120px; height: 40px; background-color: #20242c; color: #ffffff; border-radius: 6px; cursor: pointer;"></button>
</vbox>

<style>
    #btnClose:hover {
        background-color: #3a4250;
    }

    #btnClose:active {
        background-color: #11151c;
    }
</style>
```

### C++

```cpp
#include <Windows.h>
#include "EzUI/EzUI.h"

using namespace ezui;

class MainForm : public Window {
public:
    MainForm() : Window() {
        SetSize(1024, 720);
        LoadXml("res/mainForm.htm");
    }

protected:
    void OnClose(bool& allowClose) EZUI_OVERRIDE {
        Application::Exit(0);
    }
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    Application app(hInstance);
    app.EnableHighDpi();
    app.SetResource("my_res"); // 可填 VS 资源名或本地资源包路径

    MainForm form;
    form.CenterToScreen();
    form.Show();

    return app.Exec();
}
```

> 推荐优先包含 `EzUI/EzUI.h`。如果只想按模块拆分，也可以改用 `core/`、`control/`、`graphics/`、`window/`、`shell/` 下的头文件。

---

## 🧭 先建立正确心智模型

- `Application` 负责应用入口、DPI、资源包和消息循环。
- `Window` 及其子类负责窗口壳层、顶层消息和模态行为。
- `Control` 负责控件树、布局、状态、命中测试和事件分发。
- `Frame` 负责隔离子树中的 ID、class 和样式作用域。
- `.htm` 是 **控件声明树**，不是 HTML。
- `<style>` 是 **框架解析的样式子集**，不是标准 CSS。

如果某个标签、属性或写法只是“看起来像 Web”，不要默认它拥有浏览器同等语义。当前实现和当前源码应优先于旧文档、旧示例和想当然的前端经验。

---

## 🪟 窗口体系

| 类型 | 作用 | 适合场景 |
| --- | --- | --- |
| `Window` | 标准 Win32 窗口，带系统标题栏和边框 | 传统桌面程序、工具软件 |
| `BorderlessWindow` | 无边框窗口，支持阴影、透明度和 Alpha 混合 | 自定义标题栏、现代桌面壳层 |
| `PopupWindow` | 失焦自动隐藏或关闭的弹出窗口 | 下拉、浮层、临时面板 |
| `Frame` | 隔离样式和命名的嵌入子树 | 复用子页面、局部作用域隔离 |

说明：

- `PopupWindow` 继承自 `BorderlessWindow`，显示时会前置并抢占焦点；非模态失焦隐藏，模态失焦关闭，适合临时浮层，不适合作为主窗口。
- `WindowShadow` 定义在 `BorderlessWindow.h` 中，用于自定义阴影。
- `BorderlessWindow` 提供 `EnableAlphaBlending()` 和 `SetOpacity(...)`，但固定尺寸窗体更容易暴露裁切问题，布局要先做预算。

---

## 🧱 XML / 样式方言边界

### 这不是标准 HTML/CSS

- 标签名以 `RegisterControl<T>(...)` 的注册表为准。
- `margin`、`padding`、`border`、`background-color` 这类视觉规则，优先写在节点 `style` 或 `<style>` 里。
- `style` / `style:hover` / `style:active` / `style:checked` / `style:focus` / `style:disabled` 都是当前实现支持的写法。
- `Label` / `TextBox` 的文字对齐值可组合 `left` / `right` / `center` 和 `top` / `bottom` / `mid` / `middle`，新示例优先使用 `mid`。
- 当前实现识别的 tooltip 节点属性是 `tooltip`。
- `action` 支持：`close`、`mini`、`max`、`title`、`move`、`movewindow`。

### `event="none"` 和 `mousetransparent="true"` 不是一回事

- `event="none"` 会让控件整体不参与命中测试，对应 `SetHitTestVisible(false)`。
- `mousetransparent="true"` 只让控件自身对鼠标透明，子控件仍可参与命中，对应 `SetMouseTransparent(true)`。

### 不要把 README 当成浏览器兼容表

下面这些事都不该默认成立：

- 不要默认标准盒模型行为。
- 不要默认所有控件都和网页输入框一样处理 `padding`。
- 不要默认所有容器都把 `100%` 当成“吃剩余空间”。
- 不要默认内容超界后会自动扩展父容器或自然滚动。
- 不要默认深色 PNG 图标在深色背景上一定可见。

---

## 📐 布局规则

- `VLayout` / `HLayout` 下，子控件如果既没有固定尺寸、也没有 `auto`、也没有百分比尺寸，就会由布局器分配剩余空间，多个同类子控件常见表现是平分剩余区域。
- 对“吃掉剩余空间”的子控件，不要用 `height="100%"` 或 `width="100%"` 表达；在 `vbox` / `hbox` 中更稳妥的方式是省略该方向尺寸，让布局器分配。
- `fixed > auto > percent > 未指定（布局器分配）` 是更接近当前实现的尺寸优先级理解。
- `width="100%"` 不要再叠加左右 `margin`；`height="100%"` 不要再叠加上下 `margin`。这类组合很容易出界。
- 固定间距优先用 `margin`，需要占位拉伸时再用 `Spacer`。
- `VLayout` / `HLayout` / `VListView` / `HListView` 当前识别 `items-align` 或 `align`，新文档和新示例以 `items-align` 为主。
- 在 `vlist` / `hlist` 里，item 跟随列表容器同宽或同高时，使用 `width="100%"` / `height="100%"` 是合理的；这和 `vbox` / `hbox` 的“剩余空间分配”不是同一语义。
- 根布局通常不要直接写 `margin`。如果需要整页留白、背景或卡片效果，更稳妥的做法是根布局里再包一层真实内容容器。

---

## 🧩 已确认注册的 XML 标签

- 通用基础：`<control>`、`<layout>`、`<box>`
- 布局容器：`<vlayout>` / `<vbox>`、`<hlayout>` / `<hbox>`
- 列表容器：`<vlist>` / `<vlistview>`、`<hlist>` / `<hlistview>`、`<tilelist>` / `<tilelistview>`
- 页面切换：`<tabcontrol>`
- 占位控件：`<spacer>`、`<vspacer>`、`<hspacer>`
- 基础控件：`<label>`、`<button>`、`<checkbox>`、`<radiobutton>`
- 文本输入：`<textbox>` / `<input>` / `<edit>`
- 下拉框：`<combobox>` / `<select>`
- 图片控件：`<img>` / `<image>` / `<pictureBox>`
- 链接文本：`<a>` / `<LinkLabel>`
- 隔离容器：`<frame>` / `<iframe>`
- 树、数据与扩展控件：`<treeview>`、`<treenode>`、`<datagridview>`、`<slider>`、`<richtextview>`

说明：

- 这些标签来自当前 `UILoader` 注册表；和旧版 README 冲突时，以当前实现为准。
- `ComboBox` 的下拉项不建议把 `<option>` 当成基础控件标签能力来描述。稳定做法是通过 `AddOption(...)`、`OnCreateOption(...)` 或自定义控件来构建选项。

---

## 🧷 常用属性与状态

| 类型 | 当前实现常用写法 |
| --- | --- |
| 标识 | `name` / `id`、`class` |
| 内联样式 | `style` |
| 状态样式 | `style:hover`、`style:active`、`style:checked`、`style:focus`、`style:disabled` |
| 几何 | `location="x,y"`、`size="w,h"`、`rect="x,y,w,h"` |
| 可见性 | `visible="false"`、`display:none` |
| 布局参与 | `float="true"` |
| 行为 | `action="close|mini|max|title|move|movewindow"` |
| 启用状态 | `enable="true|false"`、`disabled="true"` |
| 命中测试 | `event="none"`、`mousetransparent="true"` |
| 滚动条命名 | `scrollbar="name"` |
| 提示文字 | `tooltip="..."` |

### 控件状态

- 默认状态：普通状态，无伪类
- `:hover`：鼠标悬浮
- `:active`：按下
- `:focus`：获得焦点
- `:disabled`：禁用
- `:checked`：选中

### 文本相关控件的常用属性

- `Label`：`text`、`underline`、`ellipsis`、`align` / `text-align`
- `TextBox`：`text` / `value`、`placeholder`、`passwordchar`、`readonly`、`multiline`、`align` / `text-align`
- `CheckBox`：`checked`

---

## 🛠️ 核心模块

| 模块 | 关键能力 |
| --- | --- |
| `Application` | `EnableHighDpi()`、`SetResource(...)`、`Exec()`、`Exit(...)`、`GetStartupPath()` |
| `Window` | `SetSize(...)`、`SetLayout(...)`、`LoadXml(...)`、`Show()`、`ShowModal()`、`CenterToScreen()`、`OnNotify(...)` |
| `Control` | `Append(...)`、`Prepend(...)`、`FindControl(...)`、`FindChildren(...)`、`SetStyleSheet(...)`、`AddEventHandler(...)` |
| `Frame` | 隔离样式作用域、隔离命名空间、承接局部通知 |
| `UILoader` | XML 加载、`RegisterControl<T>(...)` 自定义控件注册 |
| `UISelector` | 链式筛选和批量操作控件 |
| `Animation` | 数值插值、淡入淡出、切页滑动 |
| `Thread` / `ThreadPool` | 后台执行和线程池任务 |
| `Timer` / `DebounceTimer` | 延迟任务、周期任务、防抖场景 |
| `Invoke(...)` / `BeginInvoke(...)` | 同步或异步回到 UI 线程 |
| `MessageQueue` | 线程安全 `Post(...)` / `Exec()` 模型消息队列 |
| `Resource` | 资源目录打包与包内文件读取 |
| `PopupMenu` / `SystemMenu` | 自绘多级菜单与 Win32 原生菜单 |
| `ToolTip` / `TrayIcon` | 浮动提示、系统托盘 |

### 常见控件回调

- `CheckBox::CheckedChanged`
- `TextBox::TextChanged`
- `Slider::ValueChanged`
- `ComboBox::SelectedChanged`
- `Window::OnNotify(...)` / `Frame::OnNotify(...)` / `SetNotifyHandler(...)`

---

## 📦 资源包工作流

### 1. 打包资源目录

```cpp
bool ok = Resource::Package("res", "my_res");
```

`Resource::Package(...)` 会递归打包目录中的文件，并保留相对路径。当前仓库里 `demo/ResPackage` 展示了完整流程。

### 2. 在应用启动时挂载资源包

```cpp
Application app(hInstance);
app.EnableHighDpi();
app.SetResource("my_res");
```

`SetResource(...)` 支持两种输入：

- VS 内嵌资源名称
- 本地资源包文件路径

注意：

- 同一时刻只有一个挂载资源包；重复调用 `SetResource(...)` 会替换旧资源包。
- `LoadXml("res/mainForm.htm")`、图片路径、子页面路径等会优先读取本地文件；找不到本地文件时，再从当前资源包中按包内相对路径读取。

---

## 🧪 自定义控件注册

当 XML 需要直接按标签名创建你的控件时，在调用 `LoadXml(...)` 之前使用 `RegisterControl<T>(...)`：

```cpp
#include "EzUI/EzUI.h"

using namespace ezui;

class SessionFrame : public Frame {
public:
    SessionFrame(Object* owner = NULL) : Frame(owner) {
        LoadXml("res/session.htm");
    }
};

void RegisterEzUIControls() {
    RegisterControl<SessionFrame>("SessionFrame");
}
```

然后即可在 XML 中使用：

```xml
<SessionFrame></SessionFrame>
```

---

## 🎨 自绘、图片和分页

- `PictureBox` 支持 GIF 自动播放。
- `TabControl` 提供 `SetPageIndex(...)` 和 `SlideToPage(...)`。
- `RichTextView` 用于只读富文本展示。
- 默认样式不够时，可以继承 `Control` / `Label` / `Frame`，重写 `OnPaint(...)`、`OnBackgroundPaint(...)`、`OnForePaint(...)`。
- 自绘时通过 `PaintEventArgs::Graphics()` 使用 Direct2D 封装接口。

建议：

- 布局放在 `OnLayout()`，绘制放在 `OnPaint()`。
- 交互反馈优先改颜色、透明度和位置，不要轻易用 hover 改宽高导致 relayout 抖动。

---

## 🧵 线程与 UI 线程规则

- 后台任务用 `Thread`；需要排队并发时用 `ThreadPool`。
- 周期或延迟行为用 `Timer`。
- 搜索框、防抖输入场景优先用 `DebounceTimer`。
- 线程间消息传递用 `MessageQueue`。
- 后台线程里不要直接改 UI，统一通过 `Invoke(...)` 或 `BeginInvoke(...)` 回到 UI 线程。

---

## 🔍 调试与预览

- Debug 下按 `F11` 可切换控件边框高亮，排查文本叠加、尺寸裁切和布局越界问题。
- `include/EzUI/XMLPreview.hpp` 提供 XML 预览窗口能力，适合做页面调试和热重载式预览。

改完 `.htm` 之后，至少验证这三件事：

1. 文本是否叠在一起。
2. 内容是否被裁切。
3. 图标是否在当前背景上可见。

---

## 📖 学习与技术支持

- 视频教程：[Bilibili 主页](https://space.bilibili.com/240269358/video)
- QQ：`718987717`
- QQ 群：`758485934`
- 邮箱：`19980103ly@gmail.com`
- 微信：`wx19980103yon`

---

## 📄 License

**版本**：2.0  
**适用系统**：Windows 7 及以上  
**当前状态**：暂时闭源

- 当前以编译产物和头文件的形式提供使用。
- 可用于个人项目和商业项目。
