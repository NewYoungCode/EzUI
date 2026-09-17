# EzUI 库地图

用于定位模块、公开头文件和实现入口。精确 XML 语法见 [syntax-quick-reference.md](syntax-quick-reference.md)，线程与生命周期见 [runtime-rules.md](runtime-rules.md)。

## 核心入口

| 路径 | 主要职责 | 常用符号 |
| --- | --- | --- |
| `include/EzUI/EzUI.h` | 公共聚合头、事件和 UI 线程派发 | `Invoke`、`BeginInvoke`、`EventArgs`、`Color` |
| `include/EzUI/core/Application.h` | 应用生命周期、DPI 和资源 | `Application`、`EnableHighDpi`、`SetResource`、`Exec`、`Exit` |
| `include/EzUI/window/Window.h` | 顶层窗口、布局、XML、模态和通知 | `SetLayout`、`LoadXml`、`ShowModal`、`OnNotify` |
| `include/EzUI/window/BorderlessWindow.h` | 无边框、阴影和 Alpha 混合 | `EnableAlphaBlending`、`SetOpacity`、`GetShadow` |
| `include/EzUI/window/PopupWindow.h` | 临时浮层窗口 | `PopupWindow` |
| `include/EzUI/control/Control.h` | 控件树、尺寸、样式、命中和事件 | `AddChild`、`InsertChildAt`、`FindControl`、`SetStyle` |
| `include/EzUI/control/Frame.h` | 隔离子页面 | `LoadXml`、`SetLayout`、`GetLayout`、`GetUILoader` |
| `include/EzUI/core/UILoader.h` | XML 加载与自定义控件注册 | `LoadXml`、`SetupUI`、`RegisterControl<T>` |
| `include/EzUI/core/UIStyle.h` | 选择器和样式解析 | `AnalysisStyle`、`ApplyStyle` |
| `include/EzUI/core/Resource.h` | 资源读取与打包 | `Resource::Package`、`GetFile` |

典型启动路径：

```cpp
Application app(hInstance);
app.EnableHighDpi();
app.SetResource("my_res");

MainForm form;
form.SetSize(1024, 720);
form.LoadXml("res/mainForm.htm");
form.Show();
return app.Exec();
```

## 窗口选择

| 类型 | 使用场景 |
| --- | --- |
| `Window` | 标准顶层窗口、系统标题栏 |
| `BorderlessWindow` | 自定义标题栏和阴影；透明、异形、圆角或窗口透明度需先 `EnableAlphaBlending()` |
| `PopupWindow` | 下拉、浮层和临时工具面板 |
| `Frame` | 嵌入独立 XML 子树并隔离 ID/class/style；它仍是控件，不是窗口 |

## 控件入口

| 头文件 | 控件与用途 |
| --- | --- |
| `control/VLayout.h` / `HLayout.h` | 垂直/水平布局 |
| `control/VListView.h` / `HListView.h` / `TileListView.h` | 滚动列表与瓦片列表 |
| `control/DataGridView.h` | 表格数据、行选择和单元格点击 |
| `control/Label.h` | `Label`、`LinkLabel` |
| `control/Button.h` | 按钮 |
| `control/CheckBox.h` / `RadioButton.h` | 复选与单选 |
| `control/TextBox.h` | 单行/多行输入 |
| `control/ComboBox.h` | 下拉选择 |
| `control/TabControl.h` | 多页容器和动画切页 |
| `control/TreeView.h` | 树和树节点 |
| `control/PictureBox.h` | 图片与 GIF |
| `control/Slider.h` | 数值滑块 |
| `control/RichTextView.h` | 只读富文本 HTML 子集 |
| `control/PagedListView.h` | 分页/无限滚动基类 |
| `control/ScrollBar.h` | 滚动条 |
| `control/Spacer.h` | 弹性占位 |

## Shell 与弹出组件

| 路径 | 用途 | 常用符号 |
| --- | --- | --- |
| `include/EzUI/shell/Menu.h` | 自定义菜单和 Win32 原生菜单 | `PopupMenu`、`SystemMenu` |
| `include/EzUI/shell/ToolTip.h` | 浮动提示 | `ToolTip`、`ShowTooltip`、`HideTooltip` |
| `include/EzUI/shell/TrayIcon.h` | 托盘图标、菜单和气泡消息 | `TrayIcon`、`SetIcon`、`SetMenu`、`ShowBalloonTip` |
| `include/EzUI/window/BorderlessWindow.h` | 窗口阴影 | `WindowShadow` |

## 运行时与基础设施

| 路径 | 用途 |
| --- | --- |
| `core/Object.h` | owner 生命周期、弱引用、属性和延迟删除 |
| `core/Thread.h` | `Thread`、`ThreadPool` |
| `core/Timer.h` | `Timer`、`DebounceTimer`、`WaitableTimer` |
| `core/Animation.h` | 数值动画 |
| `core/MessageQueue.h` | `Post` / `Exec` 任务队列 |
| `core/UISelector.h` | 按 ID/class/类型/属性链式选择 |
| `core/SmartPtr.h` | `SharedPtr`、`WeakPtr` |
| `core/Utility.h` | 显示器和系统工具 |

## 图像与自绘

| 路径 | 用途 |
| --- | --- |
| `graphics/Graphics.h` | 文本、图片、路径和几何绘制 |
| `graphics/GraphicsTypes.h` | `Rect`、`Size`、`Align`、`Gradient` 等 |
| `graphics/Image.h` | 图片加载、保存、克隆和像素锁定 |
| `graphics/Bitmap.h` | BGRA 软件位图 |

自绘控件通常继承 `Control`、`Label` 或 `Frame`：

1. 布局计算放 `OnLayout()`。
2. 背景、主体或前景分别放 `OnBackgroundPaint(...)`、`OnPaint(...)`、`OnForePaint(...)`。
3. 通过 `args->Graphics()` 使用 `DrawString`、`DrawImage`、`FillRectangle`、`FillGeometry` 等 API。
4. 线性、径向和锥形渐变使用 `Gradient::Linear/Radial/Conic`；具体重载以当前头文件为准。

## 自定义控件注册

```cpp
class SessionFrame : public Frame {
public:
    SessionFrame() { LoadXml("res/session.htm"); }
};

RegisterControl<SessionFrame>("SessionFrame");
```

注册类型必须可默认构造；应用层不要调用注册机制背后的 `detail::` 实现。
