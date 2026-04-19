# EzUI 库地图

## 关键入口

| 路径 | 作用 | 关键符号 |
| --- | --- | --- |
| `README.md` | 框架能力总览，包含 XML、样式、属性和状态说明 | `Application`、`Window`、控件属性、样式示例 |
| `EzUI/Application.h` | 应用生命周期和全局资源挂载 | `SetResource`、`EnableHighDpi`、`Exec`、`Exit` |
| `EzUI/Window.h` | 顶层窗口生命周期、布局挂载、模态流程、事件派发 | `SetLayout`、`LoadXml`、`ShowModal`、`OnNotify`、`OnPaint` |
| `EzUI/BorderlessWindow.h` | 无边框窗口、阴影和分层透明支持 | `EnableTransparency()`、`SetShadow`、`UpdateShadowBox` |
| `EzUI/Control.h` | 控件树、布局、样式、事件、运行时插入节点的核心基类 | `Append`、`Prepend`、`FindControl`、`SetStyleSheet` |
| `EzUI/Frame.h` | 样式和命名隔离子树 | `LoadXml`、`SetLayout`、`GetUILoader` |
| `EzUI/UILoader.h` | 声明式 UI 加载器和自定义控件注册入口 | `LoadXml`、`SetupUI`、`RegisterControl<T>()` |
| `EzUI/UIStyle.h` | 选择器解析与样式应用 | `AnalysisStyle`、`ApplyStyle` |
| `EzUI/Resource.h` | 资源打包与读取 | `Resource::Package`、`GetFile` |
| `EzUI/Animation.h` | 动画插值 | `Start`、`ValueChanged` |
| `EzUI/Timer.h` | 计时器和延迟任务 | `Timer`、`Timeout` |
| `EzUI/Task.h` | 后台任务和任务工厂 | `Task`、`TaskFactory` |
| `EzUI/EzUI.h` | 公共类型、事件参数、UI 线程同步辅助 | `BeginInvoke`、`Invoke`、`Color`、`EventArgs` |
| `EzUI/D2DGraphics.h` | Direct2D 绘制接口 | `Graphics`、`TextLayout`、`Geometry` |
| `EzUI/GraphicsTypes.h` | 基础几何类型、对齐、尺寸模式、值容器 | `Rect`、`Size`、`Align`、`SizeMode`、`Value<T>` |

## Shell 与弹出组件

| 头文件 | 主要职责 | 关键符号 |
| --- | --- | --- |
| `EzUI/PopupWindow.h` | 下拉弹出窗口基类 | `PopupWindow`、`Show(x,y)`、`Hide` |
| `EzUI/PopupMenu.h` | 自定义多级菜单 | `PopupMenu`、`AddItem`、`AddSubMenu`、`MenuClick` 回调 |
| `EzUI/SystemMenu.h` | Win32 原生右键菜单封装 | `SystemMenu`、`AddItem`、`AddSubMenu`、`Show`、`MenuClick` 回调 |
| `EzUI/ToolTip.h` | 浮动提示 | `ToolTip`、`Show`、`Hide`、`SetText` |
| 系统托盘类（头文件名以实际源码为准） | 系统托盘图标、提示文本、右键菜单与气泡消息 | `TrayIcon`、`SetIcon`、`SetTips`、`SetMenu`、`ShowMessage`、`EventHandler` |
| `EzUI/WindowShadow.h` | 窗口阴影效果 | `WindowShadow`、`SetShadow`、`UpdateShadowBox` |

> 证据等级：代码推断（来自头文件分析）

## 推荐心智模型

把 EzUI 拆成五层来理解：

1. `Application` 负责应用入口、DPI、资源包和消息循环。
2. `Window` 及其子类负责窗口壳层和顶层事件。
3. `Control` 负责控件树、布局、状态、命中测试和事件分发。
4. 样式系统负责选择器、伪状态、继承和覆盖。
5. `Task`、`Timer`、`Animation`、`Graphics` 负责行为、异步和绘制扩展。

### Object 基类（所有控件的根）

`EzUI/Object.h` 是整个控件树的根基类，提供：

- `SetUserData(void*)` / `GetUserData()` — 任意用户数据绑定
- `SetAttribute(key, value)` / `GetAttribute(key)` — 动态属性字典
- `DeleteLater()` — 延迟销毁（安全释放）
- `GetWeakPtr()` / `GetAlive()` — 弱引用和存活检查

> 证据等级：代码推断（来自头文件分析）

## 典型运行路径

```cpp
Application app(hInstance);
app.EnableHighDpi();
app.SetResource("my_res");

MainForm form(1024, 720);
form.LoadXml("res/mainForm.htm");
form.Show();

return app.Exec();
```

实际做界面时，按下面顺序判断：

1. 是否需要资源包。
2. 窗口应该选 `Window`、`BorderlessWindow` 还是 `PopupWindow`，以及是否需要给 `BorderlessWindow` 调用 `EnableTransparency()`。
3. 布局应该走 XML-first、code-first 还是 hybrid。
4. 事件放在单个控件 `EventHandler`，还是统一进 `OnNotify(...)`。
5. 是否涉及后台任务、计时器、动画或自绘。

## 窗口类型选择表

| 类型 | 适合场景 | 不适合场景 |
| --- | --- | --- |
| `Window` | 标准桌面程序、保留系统标题栏 | 需要完全自定义窗体外观 |
| `BorderlessWindow` | 自定义标题栏、阴影，以及透明/异形/圆角/淡入淡出窗体（调用 `EnableTransparency()`） | 必须保留系统标题栏和系统边框 |
| `PopupWindow` | 下拉、浮层、临时工具面板 | 主窗口或长驻页面 |

补充说明：

- `Frame` / `<iframe>` 不属于 window 层；它本质上仍然是一个控件，只是带有子页面、命名和样式作用域隔离效果。
- 需要把一块独立 XML 或局部工作区嵌进现有界面时，用 `Frame`；需要真正的顶层窗口壳层时，再从 `Window`、`BorderlessWindow`、`PopupWindow` 里选。需要分层窗口效果时，不再单独选类，而是在 `BorderlessWindow` 上调用 `EnableTransparency()`。

## 核心控件和布局原语

| 头文件 | 主要职责 | 备注 |
| --- | --- | --- |
| `VLayout.h` / `HLayout.h` | 主布局容器 | 支持固定、百分比、auto；未指定尺寸的子控件会由布局器分配空间，常见表现是平分可分配区域 |
| `Label.h` | 文本或图片化展示 | `Button`、`CheckBox` 都建立在它上面 |
| `Button.h` | 按钮 | 常搭配 `HoverStyle`、`ActiveStyle` |
| `CheckBox.h` / `RadioButton.h` | 选择控件 | `CheckedStyle` 很关键；同一父控件下的 `RadioButton` 会自动互斥 |
| `TextBox.h` | 单行或多行输入框 | 光标、选区、`align/halign/valign`、`placeholder`、`passwordchar`、`readonly`、`multiline`、垂直滚动 |
| `ComboBox.h` | 下拉选择框 | 内部组合了 `TextBox`、`CheckBox`、`VListView`、`PopupWindow` |
| `VListView.h` / `HListView.h` / `TileListView.h` | 滚动列表容器 | 适合会话列表、画廊、面板组 |
| `TabLayout.h` | 页面切换容器 | 支持 `SlideToPage(...)`；可按同级 `RadioButton` 的顺序索引切页 |
| `TreeView.h` | 树结构浏览 | `TreeNode` 支持定制头部 |
| `PictureBox.h` | 图片或 GIF | 自动处理动图播放 |
| `ScrollBar.h` | 滚动条 | 支持样式、自动隐藏、动画 |
| `Spacer.h` | 空白占位 | 优先用它表达弹性空间，不要用空 label 顶布局 |
| `Slider.h` | 滑块控件，有 min/max/value | `ValueChanged` 回调 |
| `RichTextView.h` | 富文本显示，支持 HTML 子集 `<p>` `<a>` `<span>` | 只读富文本 |
| `LinkLabel`（在 `Label.h` 中） | 超链接文本 | 内建 URL 跳转 |
| `PagedListView.h` | 分页列表基类 | `NextPaging` 回调实现无限滚动 |

## 已确认的 XML 标签

| 标签 | 含义 | 来源 |
| --- | --- | --- |
| `<control>` / `<layout>` / `<box>` | 通用基础控件或容器 | `InitControls()` 源码 |
| `<vlayout>` / `<vbox>` | 垂直布局 | `InitControls()` 源码 |
| `<hlayout>` / `<hbox>` | 水平布局 | `InitControls()` 源码 |
| `<vlist>` / `<vlistview>` | 垂直滚动列表 | `InitControls()` 源码 |
| `<hlist>` / `<hlistview>` | 水平滚动列表 | `InitControls()` 源码 |
| `<tilelist>` / `<tilelistview>` | 瓦片列表 | `InitControls()` 源码 |
| `<spacer>` / `<vspacer>` / `<hspacer>` | 固定或弹性空白 | `InitControls()` 源码 |
| `<label>` | 文本或图片承载控件 | `InitControls()` 源码 |
| `<LinkLabel>` / `<a>` | 超链接标签 | `InitControls()` 源码 |
| `<button>` | 按钮 | `InitControls()` 源码 |
| `<checkbox>` | 复选框 | `InitControls()` 源码 |
| `<radiobutton>` | 单选按钮 | `InitControls()` 源码 |
| `<combobox>` / `<select>` | 下拉框 | `InitControls()` 源码 |
| `<textbox>` / `<input>` / `<edit>` | `TextBox` 的 XML 形式 | `InitControls()` 源码 |
| `<tabcontrol>` | 分页控件 | `InitControls()` 源码 |
| `<iframe>` / `<frame>` | `Frame` 控件，对应带隔离效果的子页面容器 | `InitControls()` 源码 |
| `<img>` / `<pictureBox>` / `<image>` | 图片/GIF 展示 | `InitControls()` 源码 |
| `<treeview>` / `<treenode>` | 树形视图与树节点 | `InitControls()` 源码 |
| `<slider>` | 滑块控件 | `InitControls()` 源码 |
| `<richtextview>` | 富文本视图 | `InitControls()` 源码 |

补充说明：

- 这里按框架内部 `InitControls()` 注册表整理；如果和旧示例或旧文档冲突，以这里为准。
- `LinkLabel`、`pictureBox` 这类名字按源码保留原大小写。
- `<option>` 虽然在部分示例里可能出现，但它不在这份 `RegisterControl` 注册表里，不要和控件标签注册混为一谈。

这些标签是 EzUI 的控件标签映射，不是 HTML 语义标签。看起来像网页，并不代表拥有浏览器同等布局和渲染行为。

## 基础设施和工具类

| 头文件 | 主要职责 | 关键符号 |
| --- | --- | --- |
| `EzUI/Object.h` | 控件树基类，属性/生命周期/弱引用 | `SetUserData`/`GetUserData`、`SetAttribute`/`GetAttribute`、`DeleteLater`、`GetWeakPtr`/`GetAlive` |
| `EzUI/Image.h` | 图片加载/绘制/操作 | `Image::Make`、`Lock`/`Unlock` 像素操作、`Save`、`Clone`、`SizeMode`、`Clip` |
| `EzUI/Bitmap.h` | BGRA 32-bit 软件位图 | `Bitmap`、像素级读写 |
| `EzUI/Gradient.h` | 渐变（线性/径向/锥形） | `LinearGradient`、`RadialGradient`、`ConicGradient`、`GradientType`、`GradientSpread` |
| `EzUI/UISelector.h` | jQuery 风格链式控件选择器 | `UISelector`、按 ID/class/类型/属性选择、链式操作 |
| `EzUI/DebounceTimer.h` | 防抖计时器 | `DebounceTimer`、适合搜索框延迟触发 |
| `EzUI/WaitableTimer.h` | 高精度可等待计时器 | `WaitableTimer` |
| `EzUI/MessageQueue.h` | 线程安全消息队列 | `MessageQueue`、`Push`/`Pop` |
| `EzUI/MonitorInfo.h` | 显示器信息 | `MonitorInfo`、`GetMonitor`、`GetMonitorRefreshRate` |
| `EzUI/PtrManager.h` | 智能指针工具 | `PtrManager`、`SharedPtr`、`WeakPtr` |
| `EzUI/PropertyValue.h` | 可选值包装器 | `PropertyValue<T>` |
| `EzUI/Collection.h` | 泛型集合 | `Collection<T>` |

> 证据等级：代码推断（来自头文件分析）

## 方言边界

- `.htm` 文件应理解为“控件声明树”，不是 DOM。
- `<style>` 应理解为“框架解析的样式子集”，不是浏览器 CSS。
- 即使某个值“长得像 CSS”，也不要自动推断它拥有同样的行为。
- 固定尺寸窗体和调用过 `EnableTransparency()` 的 `BorderlessWindow` 更容易暴露这种差异，因为超界内容通常会直接裁切，而不是像网页那样自然扩展或滚动。
- 看到某个例子出现过，不代表它在所有控件和所有布局里都稳定；仍然要结合目标控件、目标窗口和真实运行结果验证。

## 样式系统要点

| 特性 | 依据 | 使用建议 |
| --- | --- | --- |
| 内联样式优先级最高 | `README.md`、`Control::SetAttribute` | 只做局部覆盖，不要把整套主题都写成行内 |
| 支持伪状态 | `README.md`、QQ demo | 重点关注 `:hover`、`:active`、`:focus`、`:disabled`、`:checked` |
| 支持 class 和 ID 选择器 | README 与 QQ demo | class 复用、ID 定点 |
| `Frame` 会隔离作用域 | `Frame.h` 注释 | 嵌入复杂子页面时优先用它防止选择器冲突 |
| 部分写法借鉴 CSS | `UIStyle.h` | 支持部分 `px`、百分比、`url(...)`、边框简写，但不要推断为标准 CSS 兼容 |
| 样式和属性部分重叠 | `Control::ApplyStyleProperty` | `margin`、`padding`、`border`、`background-color` 这类键统一写进 `style` 或 `<style>`；几何和行为类信息再写节点属性 |

## 事件和线程模型

| 工具 | 用途 | 备注 |
| --- | --- | --- |
| `OnNotify(Control*, EventArgs&)` | 页面级事件路由 | `Window` 和 `Frame` 都会暴露它 |
| `EventHandler` | 单控件局部事件 | 简短交互逻辑时很好用 |
| `Task` | 后台线程逻辑 | 不要直接在里面修改 UI |
| `Invoke(...)` | 同步回到 UI 线程 | 后台结果落地到 UI 时使用 |
| `BeginInvoke(...)` | 异步回到 UI 线程 | 适合 fire-and-forget UI 更新 |
| `Animation` | 平滑数值变化 | 当前 demo 主要用于透明度和滑动 |
| `Timer` | 周期或延迟行为 | 也被内部用于动画和 GIF 播放 |
| `UISelector` | 批量选择和操作控件 | jQuery 风格链式 API |
| `DebounceTimer` | 搜索框等场景的防抖 | 比普通 Timer 更语义化 |
| `WaitableTimer` | 高精度等待 | 替代 Sleep |
| `MessageQueue` | 线程间消息传递 | Push/Pop 模型 |

## 自绘路径

默认样式不足时，优先阅读：

- `EzUI/D2DGraphics.h`
- `EzUI/GraphicsTypes.h`
- `EzUI/Label.h`
- EzUI 官方 QQ 示例的主窗口实现

通用做法：

1. 继承 `Control`、`Label` 或 `Frame`。
2. 重写 `OnPaint(...)`、`OnBackgroundPaint(...)` 或 `OnForePaint(...)`。
3. 通过 `args.Graphics` 调用 `DrawString`、`DrawImage`、`FillRectangle`、`FillGeometry` 等接口。
4. 把布局和绘制职责拆开，布局放 `OnLayout()`，绘制只读当前几何结果。

### 渐变与几何图形

- `Gradient` 支持线性（`LinearGradient`）、径向（`RadialGradient`）和锥形（`ConicGradient`）三种类型。
- `D2DGraphics` 支持 `PieGeometry`、`EllipseGeometry`、`RoundedRectGeometry` 等几何图形。
- 可通过 `FillGeometry` 配合 `GradientBrush` 实现复杂填充效果。

> 证据等级：代码推断（来自头文件分析）

## 自定义控件注册模式

```cpp
class SessionFrame : public Frame {
public:
    SessionFrame(Object* owner = nullptr) : Frame(owner) {
        LoadXml("res/session.htm");
    }
};

RegisterControl<SessionFrame>("SessionFrame");
```

当 XML 需要直接通过标签名实例化自定义控件时，优先使用这个模式。
