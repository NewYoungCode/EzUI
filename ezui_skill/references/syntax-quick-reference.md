# EzUI 语法速查

用于查询当前 EzUI 2.x 的标签、属性、样式键、枚举和回调。方言与布局约束见 [htm-css-dialect.md](htm-css-dialect.md)；发生冲突时以目标项目当前源码为准。

## 内建标签

以下名称来自 `sources/core/UILoader.cpp` 的注册表：

| 标签 | 类型 |
| --- | --- |
| `control` / `layout` / `box` | `Control` |
| `vlayout` / `vbox` | `VLayout` |
| `hlayout` / `hbox` | `HLayout` |
| `vlist` / `vlistview` | `VListView` |
| `hlist` / `hlistview` | `HListView` |
| `tilelist` / `tilelistview` | `TileListView` |
| `datagridview` | `DataGridView` |
| `tabcontrol` | `TabControl` |
| `spacer` / `vspacer` / `hspacer` | `Spacer` / `VSpacer` / `HSpacer` |
| `label` | `Label` |
| `LinkLabel` / `a` | `LinkLabel` |
| `button` | `Button` |
| `radiobutton` | `RadioButton` |
| `checkbox` | `CheckBox` |
| `combobox` / `select` | `ComboBox` |
| `textbox` / `input` / `edit` | `TextBox` |
| `img` / `pictureBox` / `image` | `PictureBox` |
| `iframe` / `frame` | `Frame` |
| `treeview` / `treenode` | `TreeView` / `TreeNode` |
| `slider` | `Slider` |
| `richtextview` | `RichTextView` |

标签查找不区分大小写，但示例优先沿用注册表写法。项目可注册自定义标签：

```cpp
RegisterControl<MyControl>("MyControl"); // MyControl 必须可默认构造
```

## 通用 XML 属性

| 属性 | 含义 |
| --- | --- |
| `name` / `id` | 控件名，用于 `FindControl` |
| `class` | 一个或多个样式类 |
| `x` / `y` / `location` | 坐标；`location="x,y"` |
| `width` / `height` / `size` | 固定值、`auto` 或百分比；`size="w,h"` |
| `rect` | `x,y,width,height` |
| `visible` / `display` | 可见性 |
| `float` | 是否脱离父布局 |
| `enable` / `disabled` | 启用状态 |
| `event="none"` | 忽略鼠标命中 |
| `action` | `title` / `move` / `movewindow` / `mini` / `max` / `close` |
| `style` | `VisualState::Normal` 内联样式 |
| `style:hover` / `style:active` / `style:focus` | 对应状态内联样式 |
| `style:disabled` / `style:checked` | 对应状态内联样式 |
| `scrollbar` | 绑定滚动条对象名 |
| `tooltip` | 工具提示文本 |

`action="title"` 同时提供移动窗口和双击最大化语义；`move` 与 `movewindow` 等价。

## 选择器、状态与样式键

支持 `#id`、`.class`、逗号分隔的多选择器，以及：

```text
:hover  :active  :focus  :disabled  :checked
```

`VisualState`：`Normal`、`Disabled`、`Checked`、`Hover`、`Active`、`Focus`。

已确认的常用样式键：

| 类别 | 样式键 |
| --- | --- |
| 尺寸位置 | `x` `y` `width` `height` `min-width` `max-width` `min-height` `max-height` |
| 显示交互 | `display` `pointer-events` `opacity` `cursor` |
| 背景 | `background-color` `background-image` `background-position` `background-size` `background-image-size` |
| 前景 | `color` `fore-color` `fore-image` `fore-image-size` |
| 边框 | `border` `border-width` `border-color` `border-style` `border-left` `border-top` `border-right` `border-bottom` |
| 圆角 | `border-radius` 及四个角的 `border-*-radius` |
| 字体 | `font-size` `font-weight` `font-family` |
| 间距 | `margin` / `margin-*` / `padding` / `padding-*` |

常用值格式：固定数字或 `px`、`auto`、百分比、`url(path)`、边框样式 `solid/dashed/none`。长度优先显式使用 `px`。

## 控件专用属性

| 控件 | 属性 | 说明 |
| --- | --- | --- |
| `VLayout` / `HLayout` | `items-align` / `align` | 子项对齐；双值顺序为“垂直 + 水平” |
| `VListView` | `items-align` / `align` | `left/center/right` |
| `HListView` | `items-align` / `align` | `top/middle/bottom` |
| `Frame` | `src` | 加载子页面 |
| `TabControl` | `tabindex` / `pageindex` | 当前页索引 |
| `CheckBox` / `RadioButton` | `checked` | `true/false` |
| `Label` | `text` | 文本 |
| `Label` | `text-align` / `align` | 文本对齐 |
| `Label` | `underline` / `strikethrough` | `起点,长度` |
| `Label` | `ellipsis` | 溢出替代文本，常用 `...` |
| `TextBox` | `text` / `value` | 当前文本 |
| `TextBox` | `text-align` / `align` | 常用 `top left`、`middle center`、`bottom right` |
| `TextBox` | `passwordchar` | 密码字符 |
| `TextBox` | `placeholder` | 占位文本 |
| `TextBox` | `placeholder-color` / `placeholdercolor` | 占位文本颜色 |
| `TextBox` | `readonly` / `multiline` | `true/false` |
| `ComboBox` | `text-align` / `align` / `placeholder` | 转发到内部 `TextBox` |
| `PictureBox` | `src` | 图片路径 |
| `Slider` | `value` | 当前值 |
| `Slider` | `min` / `minvalue` | 最小值 |
| `Slider` | `max` / `maxvalue` | 最大值 |
| `DataGridView` | `columns` / `headers` | 逗号分隔表头 |
| `DataGridView` | `column-widths` / `column-aligns` | 列宽和对齐 |
| `DataGridView` | `data` / `rows` | `|` 分列、`;` 分行 |
| `DataGridView` | `show-header` / `header-visible` | 是否显示表头 |
| `DataGridView` | `header-height` / `row-height` / `cell-padding` / `grid-line-width` | 尺寸 |
| `DataGridView` | `empty-text` | 空数据文本 |
| `DataGridView` | `grid-color`、`*-back-color`、`*-fore-color` | 表格颜色，完整键以头文件为准 |

同一父控件下的 `RadioButton` 会互斥。要自动联动 `TabControl`，给按钮设置 `tabcontrol="<TabControl id>"`，并让按钮在父控件中的非 `Spacer` 顺序与页面一致；不要在按钮之间插入其他普通控件。

## 颜色

| 格式 | 示例 |
| --- | --- |
| RGB | `#FF5500` |
| RGBA | `#FF550080`，格式为 `#RRGGBBAA` |
| 函数 | `rgb(255,85,0)` / `rgba(255,85,0,0.5)` |
| 内建名称 | `red` `yellow` `blue` `black` `white` `green` `orange` `purple` `gray` `transparent` |

不要使用 `#AARRGGBB`、`#RGB` 或 `#RGBA`。`rgba()` 的 alpha 使用 `0..1` 或百分比。

## 常用枚举

| 枚举 | 值 |
| --- | --- |
| `Event` | `Click` `DoubleClick` `MouseDown` `MouseUp` `MouseMove` `MouseEnter` `MouseLeave` `MouseWheel` `KeyDown` `KeyUp` `KeyPress` `GotFocus` `LostFocus` `TextChanged` `CheckedChanged` `SelectedChanged` `ValueChanged` `DpiChanged` `ControlAdded` `ControlRemoved` |
| `MouseButton` | `None` `Left` `Right` `Middle` `XButton1` `XButton2` |
| `EasingCurve` | `Linear` `Damping` `EaseIn` `EaseOut` `Cubic` `InOut` |
| `HAlign` | `Left` `Center` `Right` |
| `VAlign` | `Top` `Middle` `Bottom` |
| `Align` | `TopLeft` `TopCenter` `TopRight` `MiddleLeft` `MiddleCenter` `MiddleRight` `BottomLeft` `BottomCenter` `BottomRight` |
| `SizeMode` / `ImageSizeMode` | `Stretch` `Cover` `Fit` `Original` |
| `FillStyle` | `Solid` `Diagonal` `Cross` `Horizontal` `Vertical` |
| `GradientType` | `Linear` `Radial` `Conic` |
| `GradientSpread` | `Pad` `Reflect` `Repeat` |

`Event` 是位掩码，可组合使用。数值或冷门枚举不要依赖本速查，直接查看当前头文件。

## 常用回调签名

| 类型 | 回调 | 签名 |
| --- | --- | --- |
| `CheckBox` | `CheckedChanged` | `void(CheckBox*, bool)` |
| `ComboBox` | `SelectedChanged` | `void(ComboBox*, int)` |
| `TextBox` | `TextChanged` | `void(TextBox*, const UIString&)` |
| `Slider` | `ValueChanged` | `void(Slider*, float)` |
| `ScrollBar` | `ValueChanged` | `void(ScrollBar*, int)` |
| `DataGridView` | `SelectedRowChanged` | `void(DataGridView*, int)` |
| `DataGridView` | `CellClick` | `void(DataGridView*, int, int)` |
| `PagedListView` | `NextPaging` | `bool(PagedListView*, int)` |
| `PopupMenu` | `MenuClick` | `void(PopupMenu::Item*)` |
| `SystemMenu` | `MenuClick` | `void(UINT_PTR)` |
| `TrayIcon` | `EventHandler` | `void(const MouseEventArgs&)` |
| `Animation` | `ValueChanged` | `void(float)` |

## 常用 C++ API

### 控件树

| API | 用途 |
| --- | --- |
| `FindControl<T>(name)` | 按名称递归查找 |
| `FindControls(attr, value)` | 按属性查找 |
| `FindControlsByClass(className)` | 按 class 查找，参数不带 `.` |
| `FindChild<T>(name)` / `FindChildren(...)` | 只查直接子控件 |
| `IsChildOf(parent)` | 判断严格子孙关系 |
| `AddChild(ctrl, autoDelete)` | 添加现有控件 |
| `InsertChildAt(pos, ctrl, autoDelete)` | 指定位置插入 |
| `Append(xml)` / `Prepend(xml)` | 解析 XML 字符串后插入 |

外层查找不会穿透 `Frame` 的子页面；先取得 `Frame` 或其 layout，再在内部查找。

### 动画、计时与切页

| API | 用途 |
| --- | --- |
| `Animation::SetStartValue/SetEndValue/SetEasingCurve` | 配置动画 |
| `Animation::Start(durationMs)` | 启动 |
| `Animation::Stop/IsRunning/IsFinished/GetRatio` | 状态控制 |
| `Timer::SetTickHandler/SetInterval/Start/Restart/Stop` | 周期任务；Tick 非 UI 线程 |
| `Timer::Timeout(...)` | 延迟执行一次 |
| `DebounceTimer::Trigger/Cancel` | 防抖 |
| `TabControl::SetPageIndex/GetPageIndex` | 直接切页 |
| `TabControl::SlideToPage(index, direction, durationMs)` | 动画切页 |
| `Invoke(...)` / `BeginInvoke(...)` | 回到 UI 线程 |

### 常用工具

`LoadCursor`、`FreeCursor`、`LoadIcon`、`InstallFont`、`CopyToClipboard`、`GetClipboardData`、`GetDisplay`、`GetPrimaryDisplayRefreshRate`、`GetTickCountMs`、`SleepMs`、`Color::Make`、`Image::Make`、`Format`、`ToString`。
