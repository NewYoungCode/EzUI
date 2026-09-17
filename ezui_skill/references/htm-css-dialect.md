# EzUI htm/style 方言

修改 `.htm`、`<style>`、尺寸或对齐时读取本文件。精确标签和属性见 [syntax-quick-reference.md](syntax-quick-reference.md)。

## 判断依据

EzUI 的 `.htm` 是控件树 DSL，`<style>` 是框架样式子集，不是浏览器 HTML/CSS。按以下顺序确认能力：

1. 目标项目中的同类正例。
2. 当前版本的 `RegisterControl<T>()`、相关控件实现和 `UIStyle.h`。
3. 框架 README 与官方 demo。

找不到依据的 Web 属性或值不要猜测。

## 样式与状态

- `margin`、`padding`、`border`、`background-color` 等写在 `<style>` 或节点 `style` 中，不直接作为节点属性。
- 框架底层没有通用的浏览器 specificity；同一属性以最后应用者为准。静态 `UILoader` 流程会先应用样式表、最后应用节点内联 `style`。
- `<style>` 只匹配加载时已有的控件树。之后通过 `AddChild` / `InsertChildAt` 加入的控件应直接调用 `SetStyle(..., VisualState)`。
- `VisualState::Normal` 是基础样式；`Hover`、`Active`、`Focus`、`Disabled`、`Checked` 只写差异项。
- hover 优先改变颜色、透明度、边框或图片，不改变尺寸制造重排抖动。
- `Frame` / `<iframe>` 是带命名和样式作用域隔离的控件，不是顶层窗口。

## 尺寸与布局

- 根控件显式宽高会决定加载窗口的宽高。根控件通常不写 `margin`；页面留白放在内部内容容器。
- 尺寸优先级可按“固定值 > `auto` > 百分比 > 未指定（布局器拉伸/分配）”理解。
- 百分比按父控件完整尺寸计算，不表示扣除兄弟控件后的剩余空间。
- `vbox` 主轴是高度，`hbox` 主轴是宽度。子控件要占剩余空间时省略主轴尺寸，不写 `100%`。
- `vbox` 子控件横向默认填满，`hbox` 子控件纵向默认填满；通常不需要再写对应的 `100%`。
- `vlist` / `hlist` 的 item 可用 `width="100%"` / `height="100%"` 跟随列表尺寸。此时缩进优先放到列表 `padding`，不要在 item 同方向叠加 `margin`。
- 不要组合 `width="100%"` 与左右 `margin`，或 `height="100%"` 与上下 `margin`；容易超界。
- 父控件某方向为 `auto` 时，直接子控件在该方向必须可测量，否则可能出现零尺寸或布局抖动。
- 长度值优先显式写 `px`，例如 `margin: 0px 12px;`。
- `Spacer` 用于弹性占位；固定间距通常用 `margin`。
- 普通表单、列表和弹窗使用布局器，不用负边距或坐标硬摆。

## 对齐与文本

- `items-align` 双值顺序为“垂直 + 水平”；垂直值为 `top/middle/bottom`，水平值为 `left/center/right`。
- 默认可按 `hbox = middle left`、`vbox = top center` 理解；`item-align` 是旧名，不再使用。
- `Label` 大字号或多行内容优先考虑 `height="auto"`；单行截断则使用固定行高和 `ellipsis="..."`，不要与垂直 autosize 混用。
- autosize 后若某方向没有剩余空间，该方向的文本 `align` 可能没有可见效果。
- 整行 item 由外层统一处理鼠标时，内部控件使用 `event="none"`。

## 颜色与视觉

- 最稳妥的颜色格式是 `#RRGGBB` 或 `#RRGGBBAA`；alpha 在末尾。
- `rgba(r,g,b,a)` 的 `a` 使用 `0..1` 或百分比，不使用 `0..255`。
- 不假设支持浏览器全部命名色、渐变语法、滤镜、transform、overflow 或自动图标反色。
- 样式渐变语法未明确确认；需要渐变时优先用 `Graphics` API 自绘。
- 深色背景必须实际检查 PNG 图标对比度。

## 验证

修改后至少真实运行、XMLPreview 或 dump 一次，检查：

- 文本是否裁切或叠加。
- 右侧、底部和滚动区域是否溢出。
- hover、focus、checked、disabled 是否有效。
- 图标是否可见。
- 动画是否造成二次布局或抖动。
