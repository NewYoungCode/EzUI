# EzUI htm/style/animation 速查表

这份速查表的来源限定为两类：

- `README.md`
- `EzUI/*.h`

## 证据等级

| 等级 | 含义 |
| --- | --- |
| `明确确认` | `README` 明写，或头文件声明/注释能直接确认 |
| `源码确认` | 直接看到了实现源码，例如 `InitControls()`、`SetAttribute(...)` |
| `代码推断` | 头文件结构、命名、重载关系可推断，但没有看到实现或 README 明文保证 |

## 一句话原则

- `.htm` 是 EzUI 控件树 DSL，不是 HTML。
- `<style>` 是 EzUI 样式子集，不是标准 CSS。
- 能力边界以 `README + 头文件` 为准，不以“看起来像 Web”推断。

## 1. 内建标签与控件

### 明确确认

| 标签/类型 | 证据 | 说明 |
| --- | --- | --- |
| `<vbox>` | `README.md` 示例 | 垂直布局 |
| `<label>` | `README.md` 示例 | 文本/图片展示控件 |
| `<style>` | `README.md` 示例 | 样式块 |

### 源码确认

以下标签名来自你提供的框架内部 `InitControls()` 注册表。这里的字符串就是当前这份框架源码实际注册的控件名；如果与旧示例或旧文档冲突，以这里为准。

| 注册名 | 控件类型 | 说明 |
| --- | --- | --- |
| `<control>` | `Control` | 通用基础控件 |
| `<layout>` | `Control` | 通用容器别名 |
| `<box>` | `Control` | 通用容器别名 |
| `<vlist>` / `<vlistview>` | `VListView` | 垂直列表 |
| `<hlist>` / `<hlistview>` | `HListView` | 水平列表 |
| `<vlayout>` / `<vbox>` | `VLayout` | 垂直布局 |
| `<hlayout>` / `<hbox>` | `HLayout` | 水平布局 |
| `<tilelist>` / `<tilelistview>` | `TileListView` | 瓦片列表 |
| `<tabcontrol>` | `TabControl` | 当前 `InitControls()` 明确注册的是这个名字 |
| `<spacer>` | `Spacer` | 通用空白占位 |
| `<vspacer>` | `VSpacer` | 垂直空白占位 |
| `<hspacer>` | `HSpacer` | 水平空白占位 |
| `<label>` | `Label` | 文本/图片承载控件 |
| `<LinkLabel>` / `<a>` | `LinkLabel` | 超链接标签；注意 `LinkLabel` 的大小写按源码注册 |
| `<button>` | `Button` | 按钮 |
| `<radiobutton>` | `RadioButton` | 单选按钮 |
| `<checkbox>` | `CheckBox` | 复选框 |
| `<combobox>` / `<select>` | `ComboBox` | 下拉选择框 |
| `<textbox>` / `<input>` / `<edit>` | `TextBox` | 文本输入框 |
| `<img>` / `<pictureBox>` / `<image>` | `PictureBox` | 图片控件；注意 `pictureBox` 的大小写按源码注册 |
| `<iframe>` / `<frame>` | `Frame` | 子页面/隔离容器；本质仍是控件，不是 window 层对象 |
| `<treeview>` | `TreeView` | 树形视图 |
| `<treenode>` | `TreeNode` | 树节点 |
| `<slider>` | `Slider` | 滑块 |
| `<richtextview>` | `RichTextView` | 富文本视图 |

### 代码推断

以下控件类型在 `EzUI/UILoader.h` 中被直接包含，说明它们是内建可加载控件集合的一部分；但如果标签名与上面的 `InitControls()` 注册表冲突，应以上面的源码注册名为准：

| 控件类型 | 头文件证据 | 备注 |
| --- | --- | --- |
| `Spacer` | `EzUI/UILoader.h` | 常见标签通常是 `<spacer>` |
| `HLayout` | `EzUI/UILoader.h` | 常见标签通常是 `<hbox>` |
| `VLayout` | `EzUI/UILoader.h` | 常见标签通常是 `<vbox>` |
| `TileListView` | `EzUI/UILoader.h` | 常见标签通常是 `<tilelist>` 或项目约定名 |
| `Button` | `EzUI/UILoader.h` | 常见标签通常是 `<button>` |
| `VListView` | `EzUI/UILoader.h` | 常见标签通常是 `<vlist>` |
| `HListView` | `EzUI/UILoader.h` | 常见标签通常是 `<hlist>` |
| `RadioButton` | `EzUI/UILoader.h` | 常见标签通常是 `<radiobutton>` |
| `CheckBox` | `EzUI/UILoader.h` | 常见标签通常是 `<checkbox>` |
| `TextBox` | `EzUI/UILoader.h` | 常见标签通常是 `<input>` 或 `<textbox>` |
| `PictureBox` | `EzUI/UILoader.h` | 常见标签通常是 `<picturebox>` |
| `Window` | `EzUI/UILoader.h` | 顶层窗口本身通常不作为普通子标签使用 |
| `ComboBox` | `EzUI/UILoader.h` | 常见标签通常是 `<combobox>` |
| `TreeView` | `EzUI/UILoader.h` | 常见标签通常是 `<treeview>` |

## 2. 自定义控件标签

### 明确确认

可以通过 `EzUI/UILoader.h` 中的 `RegisterControl<T>(ctrlName)` 注册自定义控件。

```cpp
RegisterControl<MyControl>("MyControl");
```

这意味着：

- `.htm` 里允许出现项目自定义标签。
- 技能不能假设“只有框架内建标签”。

## 3. 通用 XML 属性

### 明确确认

以下来自 `README.md` 的 Control 基本属性表：

| 属性 | 说明 |
| --- | --- |
| `name` / `id` | 控件唯一标识 |
| `class` | 类名 |
| `x` / `y` | 坐标 |
| `location` | `x,y` |
| `width` / `height` | 绝对值、`auto`、百分比 |
| `size` | `width,height` 或 `auto`；只是宽高简写，不是额外的 size 策略开关 |
| `rect` | `x,y,width,height` |
| `visible` | 可见性 |
| `display` | `none` 等显示控制 |
| `float` | 浮动布局 |
| `action` | `close` / `mini` / `max` / `title` / `move` / `movewindow`；见下方 `ControlAction` 语义 |
| `enable` / `disabled` | 启用或禁用 |
| `event` | `none` 时忽略鼠标事件 |
| `style` | 内联样式字符串；`margin`、`padding`、`border`、`background-color` 等样式键都应写在这里 |
| `scrollbar` | 绑定滚动条对象名 |

补充硬规则：

- `margin`、`padding`、`border`、`background-color` 等样式键属于样式系统，不要直接写成节点属性。
- 需要节点级局部覆盖时，统一写成 `style="margin: ...; padding: ..."` 这类形式。
- 控件默认没有宽高；在 `vbox` / `hbox` 下，如果子控件既没有固定宽高、也没有 `auto`、也没有百分比尺寸，就会由布局器平分空间。
- 如果根控件显式指定了 `width` 和 `height`，载入这份界面文件的窗口宽高会和根控件宽高一致。也就是说，根控件写了多大，窗口本身就多大；不是“窗口里面再放一个这么大的内容区”。
- 尺寸策略不用单独声明，直接由宽高配置决定；按 `fixedsize > autosize > ratesize > 未指定（布局器分配）` 理解即可。
- 如果控件已经写了 `width="100%"`，不要再叠加左右 `margin`；如果控件已经写了 `height="100%"`，不要再叠加上下 `margin`。这种组合很容易让控件最终显示出界。水平和垂直同理。
- `margin`、`padding` 这类长度值建议显式带 `px` 单位，例如 `margin: 0px 28px 0px 28px;`，不要再写成 `margin: 0 28 0 28` 这种省略单位的形式。
- 在 `vbox` / `hbox` 里，子控件如果只是想占剩余空间，不要写 `height="100%"` 或 `width="100%"`；应省略对应维度，让布局器直接分配剩余高度或剩余宽度。
- `vbox` 里的子控件在水平方向默认就会吃满可用宽度，通常不要再习惯性写 `width="100%"`；`hbox` 里的子控件在垂直方向默认就会吃满可用高度，通常也不要再习惯性写 `height="100%"`。只有在确实要表达比例尺寸时，才再写对应的百分比。
- 在 `vlist` / `hlist` 里，如果子控件就是要和容器保持同宽或同高，可以直接写 `width="100%"` 或 `height="100%"`；这和 `vbox` / `hbox` 中“不要用 100% 表示剩余空间”不是一回事。前者是列表 item 相对列表容器的尺寸保持，后者是布局器主轴上的剩余空间分配。
- 如果列表 item 已经通过 `width="100%"` 或 `height="100%"` 跟随 `vlist` / `hlist`，就不要再给 item 本身加对应方向的左右或上下 `margin` 做缩进。更推荐把这部分留白写到列表控件自己的 `padding` 上，这样更不容易出现滚动条遮住 item 内容的问题。水平和垂直同理。
- `hbox` / `vbox` 的子项对齐统一写 `items-align`，支持单值或双值。单值可写 `left`、`center`、`top`、`middle`；双值可写成 `top left`。`hbox` 默认从左到右排列，垂直方向默认居中，未显式设置时可按 `items-align="middle left"` 理解；`vbox` 默认从上到下排列，水平方向默认居中，未显式设置时可按 `items-align="top center"` 理解。`item-align` 是旧属性名，现已弃用；`mid` 可按 `middle` 的兼容别名理解。
- 只有在需要占位拉伸时才优先使用 `Spacer`；固定间隙更推荐通过子控件自己的 `margin` 处理。
- 做整行可点击的列表项时，外层 item 容器负责接鼠标事件，内部 `Label`、`Spacer`、内层 `hbox` / `vbox` 统一加 `event="none"`。
- `event="none"` 就按“忽略鼠标事件”理解即可。

### 代码推断

从不同控件头文件可再确认一批控件级属性入口存在：

| 控件 | 头文件证据 | 推断能力 |
| --- | --- | --- |
| `TextBox` | `EzUI/TextBox.h` 的 `SetAttribute`、`SetPlaceholderText`、`SetPasswordChar`、`SetReadOnly`、成员 `m_multiLine` | 源码确认支持 `align` / `halign` / `valign`、`passwordchar`、`placeholder`、`text` / `value`、`readonly`、`multiline` |
| `CheckBox` | `EzUI/CheckBox.h` 的 `SetAttribute`、`SetCheck` | 支持选中状态相关属性映射 |
| `ComboBox` | `EzUI/ComboBox.h` 的 `SetAttribute`、`AddOption` | 支持选项、当前值、下标等相关属性映射 |
| `VLayout/HLayout` | `EzUI/VLayout.h`、`EzUI/HLayout.h` 的 `SetAttribute` | 支持布局方向对齐相关属性映射 |
| `Frame` | `EzUI/Frame.h` 的 `SetAttribute`、`LoadXml` | 支持子页面/子布局入口相关属性映射 |

## 4. 选择器与伪状态

### 明确确认

来自 `README.md` 和 `EzUI/EzUI.h`：

| 类型 | 支持 |
| --- | --- |
| ID 选择器 | `#id` |
| 类选择器 | `.class` |
| 多选择器 | `.a,.b` |
| 伪状态 | `:hover` / `:active` / `:focus` / `:disabled` / `:checked` |

`VisualState` 在 `EzUI/EzUI.h` 中明确声明为：

- `Static`
- `Disabled`
- `Checked`
- `Hover`
- `Active`
- `Focus`

## 5. 明确支持的样式键

### 明确确认

以下来自 `README.md` 的样式表和 `EzUI/EzUI.h` 中 `ControlStyle` / `Border` 等结构。这里列出的键应写在 `style` 属性或 `<style>` 块里，不要直接写成节点属性：

| 类别 | 样式键 |
| --- | --- |
| 尺寸位置 | `x` `y` `width` `height` |
| 约束 | `min-width` `max-width` `min-height` `max-height` |
| 显隐交互 | `display` `pointer-events` `opacity` `cursor` |
| 背景 | `background-color` `background-image` `background-position` `background-size` `background-image-size` |
| 前景 | `color` `fore-color` `fore-image` `fore-image-size` |
| 边框 | `border` `border-width` `border-color` `border-style` `border-left` `border-top` `border-right` `border-bottom` |
| 圆角 | `border-radius` `border-top-left-radius` `border-top-right-radius` `border-bottom-left-radius` `border-bottom-right-radius` |
| 字体 | `font-size` `font-weight` `font-family` |
| 间距 | `margin` 及单边 `margin-*`，`padding` 及单边 `padding-*` |

### 代码推断

从 `EzUI/UIStyle.h` 可直接确认解析器内部至少专门处理了：

- `border-width`
- `border-color`
- `border-style`
- `border-radius`
- `border-top-left-radius`
- `border-top-right-radius`
- `border-bottom-right-radius`
- `border-bottom-left-radius`
- `border`
- `border-left`
- `border-top`
- `border-right`
- `border-bottom`
- `url(...)` 风格的图片值

这说明边框简写和图片 URL 是内建解析逻辑，不只是 README 文案。

## 6. 单位与值格式

### 明确确认

来自 `README.md`：

| 类型 | 格式 |
| --- | --- |
| 百分比尺寸 | `50%` |
| 自动尺寸 | `auto` |
| 坐标/尺寸组合 | `x,y` / `width,height` / `x,y,width,height` |
| 比例尺寸 | `50%` |
| 边距内边距样式值 | `10` / `10 20` / `10 20 30 40` |
| 布尔类 | `true` / `false` |

### 代码推断

从 `EzUI/UIStyle.h` 可确认：

| 格式 | 说明 |
| --- | --- |
| `px` | 显式像素单位会被解析 |
| 纯数字 | 会转成数值 |
| `%` | 会转成比例值 |
| `url(path)` | 会抽取内部路径生成图片 |
| `solid` / `dashed` / `none` | 边框样式值内建处理 |

### 谨慎使用

以下虽然在项目文本里能看到，但不应直接当成”标准 CSS 完整兼容”：

| 写法 | 当前建议 |
| --- | --- |
| `rgba(...)` | 有项目样例，但不要推断所有控件、所有场景都稳定 |
| `font-weight: 700` | 从 `ControlStyle.FontWeight` 看是支持的，但建议先看目标控件是否表现正常 |
| `padding` | 应写在 `style` 或 `<style>` 中；固定尺寸窗体和复杂布局里仍应做真实运行验证 |

### 尺寸分配规则

- 控件默认没有宽高。
- 在 `vbox` / `hbox` 这类布局控件下，子控件的尺寸优先级按 `fixedsize > autosize > ratesize > 未指定（布局器平分剩余空间）` 理解。
- 固定宽高就是在标签上直接写宽高，本质上等同于调用 `SetFixedWidth` / `SetFixedHeight`。
- `width="auto"` / `height="auto"` 表示 autosize，控件按内容确定最终尺寸。
- `width="50%"` / `height="50%"` 表示 ratesize，控件按父布局比例参与分配。
- 如果 `Label` 使用 autosize，则会根据文字最终宽高调整自己的大小。

错误示例：

```xml
<hbox width="500">
    <label width="50"></label>
    <hbox width="100%"></hbox>
</hbox>
```

在这个例子里，第二个子控件如果只是要拿走剩余宽度，`width="100%"` 是错误写法。正确理解应是：

- 第一个子控件固定宽度 `50`
- 第二个子控件不写宽度
- 由 `hbox` 把剩余的 `450` 宽度分给它

同理，垂直布局也一样：

```xml
<vbox height="500">
    <label height="50"></label>
    <vbox height="100%"></vbox>
</vbox>
```

如果第二个子控件只是要拿走剩余高度，`height="100%"` 也是错误写法。正确理解应是：

- 第一个子控件固定高度 `50`
- 第二个子控件不写高度
- 由 `vbox` 把剩余的 `450` 高度分给它

补充：

- 上面这条“不要用 `100%` 表示剩余空间”的规则针对的是 `vbox` / `hbox`。
- 如果父容器是 `vlist` / `hlist`，而你的目标就是让 item 跟列表容器保持同宽或同高，则可以直接使用 `width="100%"` 或 `height="100%"`。
- 判断时先分清父控件类型：`vbox` / `hbox` 是布局器，`vlist` / `hlist` 是列表控件，不要按同一套分配规则去推断。
- 如果 item 跟 `vlist` / `hlist` 保持同宽或同高，缩进优先交给列表控件自己的 `padding`，不要再用 item 对应方向的外边距去挤出空间；这样更不容易被滚动条压住。`vlist` / `hlist` 在两个方向上是同理的。

## 7. 布局相关速记

### 明确确认

| 类型 | 头文件证据 | 可确认能力 |
| --- | --- | --- |
| `VLayout` | `EzUI/VLayout.h` | 垂直排列、自动高度分配、内容对齐、`AddSpacer` |
| `HLayout` | `EzUI/HLayout.h` | 水平排列、自动宽度分配、内容对齐、`AddSpacer` |
| `Frame` | `EzUI/Frame.h` | 隔离作用域、独立布局、独立通知 |
| `TabLayout` | `EzUI/TabLayout.h` | 多页容器、切页、动画切页 |

### 实务建议

- 固定尺寸窗体里，先判断独立文本块是否更适合 `auto`；不适合再显式给 `height`。
- 不要假设文本会像浏览器一样自然占满正确高度。
- 大量写在 `style` 里的 `margin-top` 加上 `spacer` 和固定高度控件时，要先算总高度。

## 8. 动画与计时器速记

### 明确确认

来自 `EzUI/Animation.h`：

| API | 说明 |
| --- | --- |
| `EasingCurve::Linear` | 线性 |
| `EasingCurve::Damping` | 阻尼 |
| `SetEasingCurve(...)` | 设置缓动类型 |
| `SetStartValue(...)` / `SetEndValue(...)` | 设定插值区间 |
| `Start(durationMs, fps = 90)` | 启动动画 |
| `ValueChanged` | 值变化回调，注释明确说已处理线程同步 |
| `GetRate()` | 获取完成百分比 |
| `Stop()` / `IsStopped()` / `IsFinished()` | 停止与状态查询 |

来自 `EzUI/Timer.h`：

| API | 说明 |
| --- | --- |
| `Timer::Tick` | 周期回调 |
| `Interval` | 定时间隔 |
| `Start()` / `Stop()` | 启停 |
| `SetClockMode(true)` | 更频繁的时钟模式 |
| `Timer::Timeout(msec, func...)` | 延迟执行一次 |

来自 `EzUI/TabLayout.h`：

| API | 说明 |
| --- | --- |
| `SetPageIndex(index)` | 直接切页 |
| `SlideToPage(index, direction, durationMs)` | 带动画切页 |
| `SlideDirection::Horizontal` / `Vertical` | 切页方向 |
| `GetPageIndex()` | 当前页索引 |

来自 `EzUI/EzUI.h`：

| API | 说明 |
| --- | --- |
| `BeginInvoke(...)` | 异步回到 UI 线程 |
| `Invoke(...)` | 同步回到 UI 线程 |

## 9. 更稳的动画使用方式

### 明确确认

从头文件 API 可以直接支持这些模式：

- `Opacity` 淡入淡出
- `Location` 做上浮/下沉/平移
- `TabLayout::SlideToPage(...)` 做切页
- `ComboBox` 下拉面板动画，因为 `EzUI/ComboBox.h` 内部就含 `Animation* m_ani`

### 建议优先顺序

1. 透明度动画
2. 位置动画
3. 切页动画
4. 局部控件轻微强调动画

不要优先做：

- 宽高动画
- 大量依赖 relayout 的动画
- 未验证语法下的“网页式”复杂视觉特效

## 10. 绝对不要按 HTML/CSS 推断的点

| 误判 | 正确理解 |
| --- | --- |
| `.htm` 就是 HTML | 它只是控件树 DSL |
| `<style>` 就是浏览器 CSS | 它只是 EzUI 样式子集 |
| `margin` / `padding` 可以直接写成节点属性 | 这类内联样式应写进 `style` 属性或 `<style>` 块 |
| `padding`/`margin` 一定和浏览器盒模型一样 | 不要这样假设 |
| PNG 图标在深色主题会自动适配 | 不会，必须人工检查 |
| 内容超界会像网页一样滚动或自动扩展 | 固定尺寸窗体里很可能直接裁切 |
| 文本块会自动得到合理高度 | 在 `VLayout` 中经常需要显式 `height` |

## 11. 一份最小可执行清单

改 `.htm` 前：

1. 先确认目标窗口是固定尺寸还是可拉伸。
2. 先列出要用的标签、属性、样式键。
3. 对不确定项去 `README + 头文件` 求证。

改 `.htm` 后：

1. 检查文本是否叠加。
2. 检查底部和右侧是否裁切。
3. 检查图标是否在当前底色上可见。
4. 检查动画是否导致布局抖动。

如果以上任一项失败，优先降级为更保守写法，不要继续堆类似 Web 的表现层语法。

## 12. 完整枚举速查

### Event 事件位掩码（代码推断）

来自 `EzUI/EzUI.h`：

| 枚举值 | 数值 | 说明 |
| --- | --- | --- |
| `None` | 0 | 无事件 |
| `Click` | 1 | 点击 |
| `DoubleClick` | 2 | 双击 |
| `MouseDown` | 4 | 鼠标按下 |
| `MouseUp` | 8 | 鼠标释放 |
| `MouseMove` | 16 | 鼠标移动 |
| `MouseEnter` | 32 | 鼠标进入 |
| `MouseLeave` | 64 | 鼠标离开 |
| `MouseWheel` | 128 | 滚轮 |
| `KeyDown` | 256 | 键盘按下 |
| `KeyUp` | 512 | 键盘释放 |
| `KeyPress` | 1024 | 字符输入 |
| `GotFocus` | 2048 | 获得焦点 |
| `LostFocus` | 4096 | 失去焦点 |
| `TextChanged` | 8192 | 文本变化 |
| `CheckedChanged` | 16384 | 选中状态变化 |
| `SelectedChanged` | 32768 | 选择项变化 |
| `ValueChanged` | 65536 | 值变化 |
| `DpiChanged` | 131072 | DPI 变化 |
| `ControlAdded` | 262144 | 子控件添加 |
| `ControlRemoved` | 524288 | 子控件移除 |

### MouseButton 枚举（代码推断）

| 值 | 说明 |
| --- | --- |
| `None` | 无 |
| `Left` | 左键 |
| `Right` | 右键 |
| `Middle` | 中键 |
| `XButton1` | 侧键1 |
| `XButton2` | 侧键2 |

### Cursor 枚举（代码推断）

常用值：`Arrow`、`Hand`、`IBeam`、`Cross`、`Wait`、`SizeNWSE`、`SizeNESW`、`SizeWE`、`SizeNS`、`SizeAll`、`No`、`Help`、`AppStarting` 等，共约 22 种标准光标类型。

### EasingCurve 完整值（代码推断）

来自 `EzUI/Animation.h`：

| 值 | 说明 |
| --- | --- |
| `Linear` | 线性（明确确认） |
| `Damping` | 阻尼（明确确认） |
| `EaseIn` | 缓入 |
| `EaseOut` | 缓出 |
| `Cubic` | 三次曲线 |
| `InOut` | 缓入缓出 |

### Align / HAlign / VAlign（代码推断）

来自 `EzUI/GraphicsTypes.h`：

| HAlign | VAlign | 含义 |
| --- | --- | --- |
| `Left` | `Top` | 左上 |
| `Center` | `Center` | 居中 |
| `Right` | `Bottom` | 右下 |

组合对齐 `Align` 支持 9 种位置：`TopLeft`、`TopCenter`、`TopRight`、`CenterLeft`、`Center`、`CenterRight`、`BottomLeft`、`BottomCenter`、`BottomRight`。

### SizeMode / ImageSizeMode（代码推断）

| 值 | 说明 |
| --- | --- |
| `Stretch` | 拉伸填满 |
| `Cover` | 保持比例覆盖 |
| `Fit` | 保持比例适应 |
| `Original` | 原始尺寸 |

### FillStyle（代码推断）

| 值 | 说明 |
| --- | --- |
| `Solid` | 实心填充 |
| `Diagonal` | 斜线填充 |
| `Cross` | 十字线填充 |
| `Horizontal` | 水平线填充 |
| `Vertical` | 垂直线填充 |

### GradientType / GradientSpread（代码推断）

| GradientType | GradientSpread |
| --- | --- |
| `Linear` | `Pad`（默认） |
| `Radial` | `Reflect` |
| `Conic` | `Repeat` |

### Control 内部标志位 detail::Flags（代码推断）

| 标志 | 说明 |
| --- | --- |
| `Visible` | 可见 |
| `Float` | 浮动布局 |
| `Pressed` | 被按下 |
| `HitTestEnabled` | 命中测试启用 |
| `AutoWidth` | 自动宽度，按内容参与 autosize |
| `AutoHeight` | 自动高度，按内容参与 autosize |
| `LayoutPending` | 布局待执行 |
| `InLayout` | 正在布局中 |

## 13. 缺失 XML 属性补充

### 通用控件属性（源码确认 + 代码推断）

| 属性 | 说明 |
| --- | --- |
| `event="none"` | 忽略鼠标事件 |
| `tips` | 工具提示文本 |

补充规则：

- `event="none"` 更适合“整块鼠标事件都交给外层容器”的场景，例如整行 item 由最外层统一处理 hover 和 click。

### 布局属性补充（代码推断）

| 属性 | 控件 | 说明 |
| --- | --- | --- |
| `items-align` / `halign` / `align` | `VLayout` | 子项对齐；支持单值或双值，单值如 `left`、`center`，双值如 `top left`、`top center`、`bottom right`；默认可按 `top center` 理解 |
| `items-align` / `valign` / `align` | `HLayout` | 子项对齐；支持单值或双值，单值如 `top`、`middle`，双值如 `top left`、`middle left`、`bottom center`；默认可按 `middle left` 理解，`mid` 可兼容 |
| `tabindex` / `pageindex` | `TabLayout` | 当前页签索引 |

补充规则：

- `items-align` 是 `hbox` / `vbox` 设置容器内控件对齐方式的统一写法；`item-align` 是旧属性名，现已弃用。
- `items-align` 支持单值或双值；双值可同时表达垂直和水平方向，例如 `items-align="top left"`。
- 未显式设置时，`vbox` 默认从上到下排列且水平方向居中，整体可按 `top center` 理解；`hbox` 默认从左到右排列且垂直方向居中，整体可按 `middle left` 理解；整体语义按 Qt 的 `QVBoxLayout` / `QHBoxLayout` 理解更稳。

### RadioButton / TabControl 约定（项目规则）

- 在同一父控件下，所有 `RadioButton` 会自动组成互斥组。
- 如果同一父控件下同时使用了 `RadioButton` 和 `TabControl`，点击某个 `RadioButton` 时，会按它在“同级 `RadioButton` 集合”中的顺序索引切换到对应页面。
- 这里只统计同级 `RadioButton`，自动忽略同级里的 `Label`、`Spacer`、`Button` 等其他类型控件。
- 索引从 `0` 开始，按出现顺序计算。

### TextBox 属性补充（源码确认）

以下来自你提供的 `TextBox::SetAttribute(...)` 源码：

| 属性 | 控件 | 说明 |
| --- | --- | --- |
| `text-align` | `TextBox` | 优先使用双值写法，同时设置垂直和水平，例如 `top left`、`middle center`、`bottom right` |
| `align` / `halign` / `valign` | `TextBox` | 都会进入同一套文本对齐逻辑；值按源码确认支持 `top`、`bottom`、`mid`、`left`、`right`、`center` |
| `passwordchar` | `TextBox` | 调用 `SetPasswordChar(value)` |
| `placeholder` | `TextBox` | 调用 `SetPlaceholderText(value)` |
| `text` / `value` | `TextBox` | 两者都调用 `SetText(value)` |
| `readonly` | `TextBox` | 支持 `true` / `false`，调用 `SetReadOnly(...)` |
| `multiline` | `TextBox` | 支持 `true` / `false`，按源码直接切换 `m_multiLine` |

补充规则：

- 对 `TextBox` 来说，优先使用 `text-align="top left"` 这种双值写法，一次同时设置垂直和水平对齐。
- `align`、`halign`、`valign` 不是三套完全独立的 XML 接口；源码里三者都会走同一个分支，再根据值决定是改水平还是垂直对齐。
- 水平值：`left`、`right`、`center`。
- 垂直值：`top`、`bottom`、`mid`。
- 如果写成双值组合，可按“先垂直、后水平”理解，例如 `top left`、`middle center`、`bottom right`。
- `text` 和 `value` 在 XML 层等价，都会写入文本框当前文本。

### 通用 Control 属性补充（源码确认）

| 属性 | 控件 | 说明 |
| --- | --- | --- |
| `action="close"` | `Control` | `ControlAction::Close`，关闭 |
| `action="mini"` | `Control` | `ControlAction::Mini`，最小化 |
| `action="max"` | `Control` | `ControlAction::Max`，最大化/恢复 |
| `action="title"` | `Control` | `ControlAction::Title`，具有移动窗口和双击最大化窗口的行为 |
| `action="move"` | `Control` | `ControlAction::MoveWindow`，移动窗口 |
| `action="movewindow"` | `Control` | `ControlAction::MoveWindow`，与 `move` 等价 |

补充规则：

- `move` 和 `movewindow` 在 XML 层等价，都会映射到 `ControlAction::MoveWindow`。
- `title` 不是普通的移动行为，它带有标题栏语义：既能移动窗口，也支持双击最大化窗口。

### Label 属性补充（代码推断 + 项目规则）

| 属性 | 控件 | 说明 |
| --- | --- | --- |
| `align` | `Label` | 文字对齐；常用值 `left`、`top`、`mid`、`center` |
| `underline` | `Label` | 下划线 |
| `ellipsis` | `Label` | 文本溢出省略，常用值 `...` |

补充规则：

- `Label` 文字默认居中对齐。
- 需要改对齐时，优先使用 `align="left"`、`align="top"`、`align="mid"`、`align="center"` 这类写法。
- 大字号 `Label` 优先使用 `height="auto"`，让控件跟着文字高度一起撑开，避免放大字号后继续沿用旧高度导致裁切。
- 如果需求是单行超长省略，就不要把这个 `Label` 写成垂直 autosize；保留稳定单行高度，再加 `ellipsis="..."`。
- 如果 `Label` 在某个方向上是 autosize（如 `width="auto"` 或 `height="auto"`），该方向上的 `align` 通常不会产生额外效果，因为控件尺寸已经按内容收紧。
- 水平和垂直是同理的：`width="auto"` 时，`left/center/right` 往往看不出区别；`height="auto"` 时，`top/mid/bottom` 往往也看不出区别。
- 如果 autosize 文本放在 `vbox` 里，而 `vbox` 没指定子元素对齐方式，子元素默认会按 `items-align="top center"` 摆放；只有想让这组文字整体靠左或靠右时，才需要显式给外层 `vbox` 写 `items-align="top left"` 或 `items-align="top right"`，或者去掉子 `Label` 的 `width="auto"`，让它填满 `vbox` 后再用水平方向的 `Label align`。
- 同理，如果 autosize 文本放在 `hbox` 里，而 `hbox` 没指定子元素对齐方式，子元素默认会按 `items-align="middle left"` 摆放；只有想让这组文字整体靠上、靠下或主轴居中时，才需要显式给外层 `hbox` 写 `items-align="top left"`、`items-align="bottom left"`、`items-align="middle center"`，或者去掉子 `Label` 的 `height="auto"`，让它填满 `hbox` 后再用垂直方向的 `Label align`。

## 14. 颜色语法

### 代码推断

来自 `EzUI/UIStyle.h` 和项目示例：

| 格式 | 示例 | 说明 |
| --- | --- | --- |
| 十六进制 | `#FF5500`、`#FF5500FF` | 6 位或 8 位（含 alpha） |
| `rgb()` | `rgb(255,85,0)` | RGB 函数 |
| `rgba()` | `rgba(255,85,0,0.5)` | RGBA 含透明度 |
| 命名颜色 | `red`、`yellow`、`blue`、`black`、`white`、`green`、`orange`、`purple`、`gray`、`transparent` | 内建颜色名 |

## 15. 回调签名速查

### 代码推断

| 控件 | 回调名 | 签名 |
| --- | --- | --- |
| `CheckBox` | `CheckedChanged` | `std::function<void(CheckBox*, bool)>` |
| `ComboBox` | `SelectedChanged` | `std::function<void(ComboBox*, int)>` |
| `TextBox` | `TextChanged` | `std::function<void(TextBox*, const UIString&)>` |
| `Slider` | `ValueChanged` | `std::function<void(Slider*, float)>` |
| `ScrollBar` | `ValueChanged` | `std::function<void(ScrollBar*, int)>` |
| `TrayIcon` | `EventHandler` | `std::function<void(const MouseEventArgs&)>` |
| `PagedListView` | `NextPaging` | `std::function<bool(PagedListView*, int)>` |
| `PopupMenu` | `MenuClick` | `std::function<void(Control*)>` |
| `SystemMenu` | `MenuClick` | `std::function<void(UINT_PTR)>` |
| `Animation` | `ValueChanged` | `std::function<void(float)>` — 已有但补全签名 |

## 16. 全局工具函数

### 代码推断

来自 `EzUI/EzUI.h` 和各工具头文件：

| 函数 | 说明 |
| --- | --- |
| `LoadCursor(path)` | 从文件加载光标 |
| `FreeCursor(cursor)` | 释放光标 |
| `LoadIcon(path)` | 从文件加载图标 |
| `InstallFont(path)` | 安装字体文件 |
| `SetGlobalToolTip(tip)` | 设置全局提示 |
| `GetGlobalToolTip()` | 获取全局提示 |
| `CopyToClipboard(text)` | 复制到剪贴板 |
| `GetClipboardData()` | 获取剪贴板内容 |
| `GetMonitor(hwnd)` | 获取窗口所在显示器 |
| `GetMonitorRefreshRate()` | 获取显示器刷新率 |
| `GetTickCountMs()` | 毫秒级计时 |
| `SleepMs(ms)` | 毫秒级睡眠 |
| `Color::Make(r,g,b,a)` | 构造颜色值 |
| `Color::HSVtoColor(h,s,v)` | HSV 转颜色 |
| `Image::Make(path)` | 从文件创建图片 |
| `Format(fmt, ...)` | 格式化字符串 |
| `ToString(value)` | 转字符串 |
