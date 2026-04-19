---
name: ezui
ezui_version: "2.0"
ezui_commit: "68d8e05"
description: EzUI C++ 桌面界面开发技能。用于构建、修改、评审或调试基于 EzUI 的 Windows UI，包括 Application 启动流程、Window/BorderlessWindow/PopupWindow 窗口结构、通过 BorderlessWindow::EnableAlphaBlending() 启用分层窗口效果、通过 UILoader 或 Window::LoadXml 加载的 XML 控件树布局、类似 CSS 的样式块和伪状态、通过 Resource::Package 与 Application::SetResource 管理的资源包、自定义控件注册、PopupMenu/SystemMenu 右键菜单、ToolTip 浮动提示、TrayIcon 系统托盘、Slider/RichTextView/PagedListView 等扩展控件、UISelector 链式控件选择器、Gradient 渐变绘制，以及通过 OnPaint 与 Graphics 进行的 Direct2D 自绘。
---

# EzUI

```cpp
//// ---------------------------------------------------------------------------
//// EzUI 框架
//// 版权所有 (c) 2021-N  yang/NewYoungCode (https://github.com/NewYoungCode/EzUI)
////
//// EzUI 是一个基于 Win32 消息机制和 Direct2D 渲染的轻量级桌面 UI 框架，
//// 支持弹性布局、伪类样式、事件分发、高分辨率适配、分层窗口等特性。
////
//// Author: yang/NewYoungCode
//// Email: 19980103ly@gmail.com / 718987717@qq.com
//// ---------------------------------------------------------------------------
```

> 特别鸣谢 `LingGuGu`：基于 demo 参与了这份 `ezui` skill 的训练整理。

## 概览

本技能适用于两种场景：

- **基于 EzUI 的应用开发**：用 EzUI 框架构建桌面应用界面（主要场景）。
- **EzUI 框架源码开发**：修改或扩展 EzUI 框架本身（需要源码仓库）。

EzUI 本质上是一套基于 Win32 消息机制和 Direct2D 的桌面 UI 框架，核心能力包括声明式 XML 布局、CSS 风格样式、状态伪类、资源包、线程同步、动画和自定义绘制。此外还包括 Shell 级组件（右键菜单、浮动提示、系统托盘）、链式控件选择器、渐变绘制、丰富的回调体系和工具类。

> 本技能基于 EzUI **2.0**编写。如果目标项目使用的 EzUI 版本与此不同，部分 API 或枚举值可能存在差异。

## 本地参考目录

这个 skill 现在约定了三类可扩展目录：

- `references/demos/`：放可读的本地 demo，适合 `.htm`、`.xml`、`.cpp`、`.h` 参考页或小型完整示例。
- `assets/templates/`：放可直接复用的骨架模板，适合登录页、主窗口、列表项、标题栏等通用起手文件。
- `scripts/`：放辅助脚本，例如批量替换、模板生成、预处理或检查脚本。

读取顺序按下面理解：

1. 先看 `references/demos/` 里是否已经有同类页面或同类窗口。
2. 再看 `assets/templates/` 里是否有更适合直接复用的骨架。
3. 再看项目页面和 `references/*.md` 文档。

使用规则：

- 不要一次性把 `references/demos/` 全部读完；先按文件名和任务类型挑最相关的 1-2 个。
- 新增 demo 后，优先在 `references/demo-patterns.md` 里补一行索引说明；否则别的任务不一定会主动读到它。
- 如果某个 demo 是“推荐起手模板”，不要只放在 `references/demos/`，最好同时整理一份到 `assets/templates/`。

## 方言警告

- EzUI 的 `.htm` 不是浏览器 HTML，而是控件树声明语言。
- EzUI 的 `<style>` 不是标准 CSS，而是框架自定义的样式子集。
- `margin`、`padding`、`border`、`background-color` 这类内联样式必须写进节点的 `style` 属性里，不要直接写成节点属性。
- EzUI 的 `rgba(r,g,b,a)` 里，`a` 是 `0..1` 的透明度比例；如需百分比必须显式写成 `18%`。不要写 `rgba(255,255,255,24)` 这种 0-255 alpha，`Color::Make` 会把它 clamp 到 `1.0`，结果变成完全不透明。
- 标签名以框架实际 `RegisterControl<T>(...)` 注册表为准；碰到 `select`、`edit`、`image`、`frame` 这类别名时，不要因为它们长得像 Web 标签就误判语义。
- 不要默认浏览器级兼容性：不要默认支持标准盒模型、自动内容流、图标自动反色、`flex`、`position`、`filter`、`backdrop`、`transform`、`overflow` 等 Web 语义。
- 如果某个标签、属性或写法没有在框架 `README.md`、已有项目正例或 `UIStyle.h`/`Control.h`/相关头文件中出现过，就先不要把它当成稳定能力。
- 凡是构造函数带有 `Object* ownerObject` 参数，都要严格按“生命周期绑定”来理解：如果传入非 `NULL` 的 `ownerObject`，当前对象会在 `ownerObject` 销毁时被自动销毁。这个参数只负责绑定销毁时机，不影响对象的其他任何行为。
- 也就是说，`ownerObject` 不是布局父子关系、事件路由、逻辑归属或窗口宿主的通用替代品；不要因为“顺手”就传一个非空 owner，只有在你明确想把当前对象的销毁时机绑定到 owner 上时才传。
- 修改 `.htm` 或 `<style>` 前，优先阅读 `references/htm-css-dialect.md`。

## 快速起手

新建 EzUI 窗口时，优先从下面这个骨架开始：

```cpp
#include "EzUI/Application.h"
#include "EzUI/Window.h"

using namespace ezui;

class MainForm : public Window {
public:
    MainForm() : Window(1024, 720) {
        LoadXml("res/mainForm.htm");
    }
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int) {
    Application app(hInstance);
    app.EnableHighDpi();
    app.SetResource("my_res");

    MainForm form;
    form.Show();
    return app.Exec();
}
```

## 工作流

### 1. 先确认方言边界

- 先看 skill 自带的 `references/demos/` 和 `assets/templates/`，判断是否已经有同类正例或可复用骨架。
- 先把目标文件当成 EzUI DSL，而不是网页模板。
- 先搜索项目里同类页面的真实写法，再决定能不能复用某种”像 CSS”的表达。
- 对陌生样式值，不要因为”看起来像 CSS”就直接使用；先找项目正例，找不到就降级成更保守写法。
- 对深色主题，优先检查 PNG 图标是否会直接融入背景。

### 2. 先决定组合方式

- 视图结构明确、样式比重大、表单型页面多时，优先使用 XML-first。
- 行为逻辑复杂、运行时生成节点多、自绘较多时，优先使用 code-first。
- 大多数正式页面优先使用 hybrid：XML 骨架 + 代码事件绑定 + 必要时运行时创建控件树。
- 新增控件时，优先直接构造控件并插入已知父容器；不要把 `Append(...)` / `Prepend(...)` 当成常规手段，因为它们会触发额外的控件解析和样式解析，性能成本更高。

### 3. 先选对窗口类型

- `Window`：系统标题栏和边框都保留，适合传统桌面程序。
- `BorderlessWindow`：自定义标题栏和阴影，适合现代桌面壳层；如果需要透明、异形、圆角、淡入淡出或窗口透明度控制，优先调用 `EnableAlphaBlending()`，它会把当前无边框窗口切换到 Alpha 混合/分层窗口模式。
- 判断窗口当前是否已经启用 Alpha 混合时，使用 `IsAlphaBlendingEnabled()`。
- `PopupWindow`：下拉、临时面板、浮层提示。

如果需要的是 `Frame` / `<iframe>`，要先分清它不属于 window 层。`Frame` 本质上仍然是一个控件，只是这个控件会隔离子树中的 ID、class 和样式作用域，适合嵌入独立子页面或局部工作区；不要把它当成顶层窗口类型。

### 4. 先把布局做对，再谈美化

- 优先使用 `VLayout`、`HLayout`、`TabLayout`、`VListView`、`HListView`、`TileListView`、`TreeView`、`Slider`。
- 默认不要为了“包一层”而额外加一个类似 `id="stage"` 的最外层容器。如果页面本身只需要一个根布局控件，就直接让真实内容容器作为根节点。只有在确实需要整页背景、统一 `padding`、整页事件命中、命名锚点或后续代码查找入口时，才增加这种外层包装。
- 根控件通常不要写 `margin`。一般情况下，根控件会直接按窗口客户区去绘制；如果只是想让内容离窗口边缘留白，优先在根控件里面再包一层真实内容容器，由这层内容容器负责 `margin` / `padding` / 卡片背景。
- 调整根控件的 `border`、`border-radius`、`background-color` 时，要意识到你改的是窗口最外层可见壳，而不只是某一块内容卡片。若目标只是做页面内部的面板、卡片或留白效果，不要直接改根控件边框，优先包一层内层容器去承载这些视觉样式。
- 还要记住根控件尺寸和窗口尺寸是联动的：如果根控件显式指定了 `width` 和 `height`，那么载入这份界面文件的窗口宽高会和根控件宽高一致。也就是说，根控件写了多大，窗口本身就多大；不是“窗口里面再放一个这么大的内容区”。
- 先记住默认规则：控件默认没有宽高；在 `vbox` / `hbox` 这类布局控件下，如果子控件既没有固定宽高、也没有 `auto`、也没有百分比尺寸，就会由布局器平分可分配空间。
- `width` 和 `height` 优先使用固定值、百分比或 `auto`，必要时配合 `min-*` 和 `max-*`。固定宽高等同于 `SetFixedWidth` / `SetFixedHeight`。
- 要特别分清“比例尺寸”和“布局剩余空间”是两套机制：`SetRateWidth`、`SetRateHeight`、`SetRateSize` 以及 `width="100%"`、`height="100%"` 这类百分比写法，都是按父控件的完整宽高直接换算，不会自动改成“扣掉兄弟控件之后的剩余空间”。
- 尺寸策略不用额外声明，直接由宽高写法决定；优先级按 `fixedsize > autosize > ratesize > 未指定（布局器分配）` 理解。
- 只要父控件某个方向用了 `auto`，就要立刻检查直接子控件在这个方向上的尺寸是否都可确定。也就是说，子控件要么给固定值、要么也明确写 `auto`、要么本身就是可稳定推出该方向尺寸的写法；如果子控件在这个方向上既没定宽/高、也不是明确 autosize，父控件就可能算不出自己的尺寸，最终出现宽高为 `0`、尺寸统计错误或布局抖动。宽度和高度同理。
- 如果控件已经写了 `width="100%"`，就不要同时叠加左右 `margin`；如果已经写了 `height="100%"`，也不要同时叠加上下 `margin`。这类写法很容易导致内容出界。水平和垂直同理。
- `margin`、`padding` 这类长度值在样式里尽量显式写 `px` 单位，例如 `margin: 0px 28px 0px 28px;`；避免写成 `margin: 0 28 0 28` 这种省略单位的形式。
- 在 `vbox` / `hbox` 里，如果某个子控件只是要吃掉剩余空间，不要用 `height="100%"` 或 `width="100%"` 去表达。正确做法是省略对应维度，让布局器把剩余高度或剩余宽度分给它。`100%` 表示相对父容器的比例尺寸，不等于“剩余空间”。
- `vbox` 里的子控件在水平方向默认就会吃满可用宽度，通常不要习惯性补 `width="100%"`；`hbox` 里的子控件在垂直方向默认就会吃满可用高度，通常也不要习惯性补 `height="100%"`。只有真的要表达比例尺寸时，才再写对应的百分比。
- 但在 `vlist` / `hlist` 里，这条规则不同：如果就是要让子控件宽度或高度跟容器保持一致，可以直接写 `width="100%"` 或 `height="100%"`。根因是 `vbox` / `hbox` 属于布局器，关注的是子控件在主轴上的分配；`vlist` / `hlist` 属于列表控件，item 跟随列表容器同宽或同高是另一类语义。不要把这两类容器的尺寸规则混为一谈。
- 列表项如果已经在 `vlist` / `hlist` 里用 `width="100%"` 或 `height="100%"` 跟随列表容器，就不要用 item 自己的左右或上下 `margin` 去做人为缩进。更稳妥的做法是把这类内边距写到列表控件自身的 `padding` 上：`vlist` 通常把左右留白写进 `padding`，`hlist` 通常把上下留白写进 `padding`。这样既能保持 item 和列表容器同宽或同高，也能减少滚动条遮住子控件的风险。水平和垂直同理。
- 要把 `padding` 理解成“容器各条边界向内收缩的距离”：容器自身尺寸不变，但内部可用空间会相应缩小。
- `padding` 的简写规则按顺时针理解：`padding: 20px` 表示上右下左各收缩 `20px`；`padding: 10px 20px` 表示上下各 `10px`、左右各 `20px`；`padding: 10px 20px 30px` 表示上 `10px`、左右 `20px`、下 `30px`；`padding: 10px 20px 30px 40px` 表示上 `10px`、右 `20px`、下 `30px`、左 `40px`。
- `padding-top`、`padding-right`、`padding-bottom`、`padding-left` 表示单边向内收缩；例如 `padding-top: 20px` 就只收缩上边界 `20px`。
- 一个直观例子：容器是 `500x500px`，如果设置 `padding: 20px`，容器本身还是 `500x500px`，但内部可用区域会变成 `460x460px`。
- `hbox` / `vbox` 使用 `items-align` 时，采用严格的两轴对齐语义。水平布局器没有指定子控件对齐方式时，内部默认采用 `middle left`；垂直布局器没有指定子控件对齐方式时，内部默认采用 `top center`。设置容器内控件对齐时，统一使用 `items-align`。
- `items-align` 的可用方向值只有这 `6` 个：水平方向只能从 `left`、`center`、`right` 中选一个；垂直方向只能从 `top`、`middle`、`bottom` 中选一个。双值写法使用“垂直值 + 水平值”的组合，例如 `top left`、`middle center`、`bottom right`。
- 要特别注意单值语义是“按轴生效”的，不是“两个方向一起居中”。对 `hbox` 来说，`top/middle/bottom` 只影响垂直方向，`left/center/right` 只影响水平方向；所以 `items-align="middle"` 不会水平居中，想让单子项在 `hbox` 里居中，应该写 `center` 或 `middle center`。对 `vbox` 来说则相反，`left/center/right` 只影响水平方向，`top/middle/bottom` 只影响垂直方向；所以 `items-align="center"` 不会垂直居中，想让单子项在 `vbox` 里居中，应该写 `middle` 或 `middle center`。
- 但语义上仍要分清主次：如果需求是“这几个控件彼此底对齐 / 顶对齐 / 左对齐 / 右对齐”，优先先把这层关系用局部 `hbox` / `vbox` 的 `items-align` 表达清楚；如果还需要做视觉补偿，再在此基础上追加子控件自己的 `margin`。不要把本来该由布局器表达的关系完全退化成只靠 `margin` 硬摆。
- 只有在需要占位拉伸、吃剩余空间时才优先使用 `Spacer`；固定间隙尽量少用 `Spacer`，更推荐直接给相邻子控件写 `margin` 做边距。
- 但有一个常见例外：在固定高度的 `hbox` / `vbox` 里，如果中间放的是一组 autosize 文本，整体视觉容易偏向一侧，这时可以在文本组前后各放一个 `Spacer` 做占位，把这组内容推到视觉居中。这个用法属于“占位校正”，不是拿 `Spacer` 代替常规固定间隙。
- 能用写在 `style` 里的 `margin`、`Spacer` 和显式尺寸表达的空间关系，不要先用手写坐标硬摆。
- `padding` 虽然在框架里出现过，但节点级内联时仍应写进 `style`；判断布局可用空间时，要把它按“缩小内部可用区域”的规则去理解，不要误当成会改变容器外框尺寸。
- 可交互列表项尽量包在同一个外层容器里，保证 `hover`、`checked`、`disabled` 的命中和样式切换稳定。
- 如果整个 item 需要由外层容器统一响应鼠标事件，内部子控件统一加 `event="none"`，避免 `Label`、`Spacer`、内层布局器把鼠标事件吃掉。按技能约定，`event="none"` 就按“忽略鼠标事件”理解即可。

### 5. 对固定尺寸窗体先做预算

- 调用过 `EnableAlphaBlending()` 的 `BorderlessWindow`、登录窗、弹窗、工具面板不要假设会像浏览器一样自动扩展内容区域。
- 如果要给窗口设置透明度，例如调用 `SetOpacity(...)`，必须先调用 `EnableAlphaBlending()` 启动 Alpha 混合；否则不要假设透明度设置会生效。
- 先统计固定高度、写在 `style` 里的 `margin-top`、`spacer`、标题栏和按钮区的高度总和，再决定窗口尺寸。
- 如果根控件已经显式写了宽高，就要把这组值直接算进窗口尺寸决策里，因为载入该 `.htm` 的窗口会和根控件宽高保持一致。
- 对独立 `Label` 文本块，先判断是否需要 `auto`；如果需要随文字自适应，就给 `width="auto"` / `height="auto"`，因为 `Label` 的 autosize 会按文字最终宽高调整自己。否则再显式给 `height`，避免在 `VLayout` 中被挤压后出现叠字或裁切。
- 对大字号 `Label`，优先先给 `height="auto"`，让控件随文字真实高度把自己撑开，避免出现“字号已经放大，但控件高度还停留在旧值”导致的裁切。
- 如果需求不是“随内容撑高”，而是“超出宽度后单行省略”，就不要让它在垂直方向 autosize；应给出稳定的单行高度，并使用 `ellipsis="..."` 表达截断。
- 如果内容已经逼近窗体边界，优先同时做两件事：增大窗体尺寸，缩小垂直节奏；不要只做其中一边。

### 6. 按 EzUI 的状态模型组织样式

- 可复用规则优先放在 `style` 块或 `SetStyleSheet(...)` 中。
- 内联 `style` 只做最后一级覆盖，不要把整套主题都堆在行内。
- 明确考虑 `Static`、`Hover`、`Active`、`Focus`、`Disabled`、`Checked` 六类状态。
- 单点控件优先用 ID，重复结构优先用 class，需要隔离作用域时优先用 `Frame`。
- `Label` 文字默认居中；要改对齐，优先用 `text-align="left"`、`text-align="top"`、`text-align="mid"`、`text-align="center"` 这类属性，而不是先按网页文本排版去猜。
- 但要注意 `Label` 的尺寸语义：如果 `Label` 在某个方向上已经 autosize（例如 `width="auto"` 或 `height="auto"`），那么该方向上的文字对齐通常不会产生额外效果，因为控件在这个方向上已经按内容收紧了。不要把“写了 `text-align`”误当成“这个方向一定会出现对齐变化”。
- 水平和垂直是同理的：`width="auto"` 时，不要指望 `text-align="left"` / `text-align="center"` / `text-align="right"` 一定有可见区别；`height="auto"` 时，也不要指望 `text-align="top"` / `text-align="mid"` / `text-align="bottom"` 一定有可见区别。
- 如果一组 autosize 文本放在 `vbox` 里，而 `vbox` 又没有指定子元素对齐方式，子元素默认会按 `items-align="top center"` 摆放。只有在你想让这组文字整体靠左或靠右时，才需要显式给外层 `vbox` 写 `items-align="top left"` 或 `items-align="top right"`；另一种做法是不要给子 `Label` 写 `width="auto"`，让 `Label` 吃满 `vbox` 宽度后，再让 `Label` 自己用水平方向的 `text-align` 控制文字位置。
- 同理，如果一组 autosize 文本放在 `hbox` 里，而 `hbox` 没指定子元素对齐方式，子元素默认会按 `items-align="middle left"` 摆放。只有在你想让这组文字整体靠上、靠下或改变主轴分布时，才需要显式给外层 `hbox` 写 `items-align="top left"`、`items-align="bottom left"`、`items-align="middle center"` 等组合；另一种做法是不要给子 `Label` 写 `height="auto"`，让 `Label` 先拿到可分配高度，再让 `Label` 自己用垂直方向的 `text-align` 控制文字位置。
- 要分清布局器 `items-align` 的默认值和覆盖关系：`hbox` 默认是 `middle left`，所以单写 `center` 通常已经够用来让单子项在 `hbox` 中落到 `middle center`；`vbox` 默认是 `top center`，所以单写 `middle` 通常已经够用来让单子项在 `vbox` 中落到 `middle center`。如果你要显式表达两个方向，优先写完整组合值，例如 `middle center`、`top right`。
- 像“大字 wordmark + 小字 caption”这种组合，如果目标是两者底对齐，就应该先让它们在一个局部 `hbox` 里做 `items-align="bottom left"`，再分别控制字号；如果底对齐之后还需要做轻微视觉补偿，可以继续给小字追加少量 `margin-top` 或 `margin-bottom`。不要误把需求理解成“把这两个字在整行里水平居中”。
- `TextBox` 的 XML 属性至少支持：`text-align`、`passwordchar`、`placeholder`、`text` / `value`、`readonly`、`multiline`；不要把输入框能力按网页表单语义去猜。
- 通用控件的 `action` 属性按源码确认支持：`close`、`mini`、`max`、`title`、`move`、`movewindow`。其中 `title` 对应 `ControlAction::Title`，具有移动窗口和双击最大化窗口的行为；`move` 与 `movewindow` 都对应 `ControlAction::MoveWindow`；`mini` 对应最小化；`max` 对应最大化/恢复；`close` 对应关闭。
- 在同一父控件下，所有 `RadioButton` 会自动组成互斥组；如果同一父控件下同时使用了 `TabControl`，点击某个 `RadioButton` 时，会按它在“同级 `RadioButton` 集合”里的顺序索引切换到对应页面。索引从 `0` 开始，只统计同级 `RadioButton`，自动忽略其他类型控件。
- 对“像 CSS 但不确定”的值一律保守：先用纯色、纯边框、固定圆角、已存在图标资源，把页面做稳，再考虑更强的视觉效果。

### 7. 线程、计时器和动画要按 UI 线程规则来

- `Animation` 用于透明度、滑动、数值插值。
- `Timer` 用于周期任务或延迟行为。
- `DebounceTimer` 用于搜索框等需要防抖的场景，比普通 `Timer` 更语义化。
- `Task` 用于后台耗时逻辑。
- `MessageQueue` 用于线程间消息传递（Push/Pop 模型）。
- 只要框架现成能力能覆盖需求，优先使用框架提供的线程类和异步工具，例如 `Task`、`TaskFactory`、`Timer`、`DebounceTimer`、`MessageQueue`；不要先习惯性写裸 `std::thread`、Win32 线程或自己再包一套线程管理。
- 后台线程里不要直接改 UI，统一通过 `Invoke(...)` 或 `BeginInvoke(...)` 回到 UI 线程。
- 刷新控件时，默认优先使用 `Invalidate()` 触发重绘；不要把 `Refresh()` 或 `RefreshLayout()` 当成常规刷新手段，只有在确实需要立即同步刷新或强制重排时才使用。
- 如果只是文本、图片、颜色、选中态这类可视状态变化，通常改完属性后调用 `Invalidate()` 就够了。
- 交互反馈优先改颜色、透明度、位置，不要轻易用 hover 改宽高导致 relayout 抖动。
- 入场动画优先挂在 `OnShow()` 或页面首次显示之后；关闭动画优先放 `OnClose()` 控制流程；切页动画优先使用 `TabLayout::SlideToPage(...)`。

### 8. 资源路径和打包策略要提前定

- 需要发布时，优先使用 `Application::SetResource(...)` 加载资源包。
- 资源打包走 `Resource::Package(...)` 或框架自带的 `ResPackage` 流程。
- XML 中图片和子页面路径尽量和项目页面保持同一约定，常见做法是 `res/...`。
- 深色主题下，不要默认 PNG 图标可见；必要时更换浅图、加底板、改成文字按钮，或转为自绘。

### 8.5. 用 `--dump-server=<port>`、`/dumpState`、`/dumpLayout`、`/dumpScreenshot`、`/dumpMouseClick` 做布局自查

- 布局自查的标准入口是以 `Debug` 构建启动程序，并在命令行里显式追加 `--dump-server=<port>`；只有出现这条参数时，框架才会启动本地 dump server。
- dump 自查的命令行写法使用 `--dump-server=<port>`，例如 `--dump-server=9631`。
- dump server 启动后，会在 `127.0.0.1:<port>` 上提供这些标准接口：`GET /dumpState`、`GET /dumpLayout`、`GET /dumpScreenshot`、`GET /dumpMouseClick`、`GET /dumpMouseDoubleClick`、`GET /dumpKeyClick`、`POST /dumpInsertText`。
- 标准时序是固定的：先轮询 `GET /dumpState`，确认 UI 已准备好；准备好之后先调用一次 `GET /dumpLayout` 拉取当前页的控件树信息(控件树中只包含视觉可见控件)；如果需要看真实视觉，再调用 `GET /dumpScreenshot` 拉当前窗口客户区截图；如果需要切页、展开面板或点击某个控件，就先从这次 `layout` 里的 `rectInWindow` 取目标位置，再调用 `GET /dumpMouseClick` 或 `GET /dumpMouseDoubleClick` 模拟点击；点击完成并稳定后，再次调用 `GET /dumpLayout`，必要时再补一次 `GET /dumpScreenshot`。
- 只要是在 **PowerShell** 环境里调用“返回 JSON 的 dump 接口”，统一使用 `Invoke-RestMethod`。
- 对所有“返回 JSON 的 dump 接口”，成功判定统一使用返回体里的 `code=0`。
- `/dumpState` 返回体里，只要 `code=0`，就直接说明当前窗口和 UI 线程都已经进入可自查状态；如果 `code!=0`，就说明当前还没稳定，不要急着拉布局。
- 更稳的做法是：程序启动后先等待页面和数据加载一段时间，再每隔约 `2` 秒轮询一次 `/dumpState`；如果暂时还是 `code!=0`，就继续等，不要在首帧、登录弹窗阻塞中或页面刚切换时立即下结论。
- 多页面自查时，优先采用“一次启动，连续切页，连续 dump”的方式：程序启动并且 `/dumpState` 返回 `code=0` 之后，不要为了每个页面都重新启动和关闭一次程序，而是直接在同一个会话里按“点击 -> dumpLayout -> dumpScreenshot -> 再点击 -> 再 dumpLayout -> 再 dumpScreenshot”的顺序连续完成多个页面的自查。
- `GET /dumpLayout` 在 `code=0` 时表示调用成功；它返回布局快照 JSON，不包含截图，也不会返回 `Base64` 图片数据。
- `/dumpLayout` 返回体里，至少关注这些字段：`code`、`layout`。其中只有 `code=0` 才表示本次布局抓取成功，`layout` 是控件树 JSON。
- `GET /dumpScreenshot` 就是一个下载截图图片的接口，成功判定使用 HTTP `200`。它返回的是 `BMP` 位图二进制数据，不是 JSON，也不是 Base64；需要落盘时，直接按 `.bmp` 原样写文件。
- 如果需要把 `layout` JSON 和 `/dumpScreenshot` 返回的 `BMP` 落盘做复查、归档或发给用户看，统一写到用户可写的临时目录，而不是程序目录、构建目录或 demo 可执行文件旁边。优先使用用户的 `%TEMP%` / `$env:TEMP`，不要假设系统级临时目录一定可写。
- dump 落盘路径要有稳定规则，推荐使用类似 `%TEMP%/EzUI_dump/<应用名>/<会话时间戳>/<页面序号_页面名>/layout.json` 和 `%TEMP%/EzUI_dump/<应用名>/<会话时间戳>/<页面序号_页面名>/screenshot.bmp` 这样的结构；同一轮自查里的所有页面都归到同一个会话目录下，方便连续查看和回溯。
- 做自查时，不要只看 `layout` 或只看截图，必须把两者结合起来看：`layout` 用来确认控件树、坐标、尺寸、命名和命中关系，`/dumpScreenshot` 返回的截图用来确认真实视觉、层级、留白和对齐是否符合预期。
- `layout` 里，优先关注这些字段：`path`、`type`、`name`、`classes`、`rect`、`rectInWindow`、`scaledValue`、`text`、`children`。字段定义以 `sources/core/dump/DumpLayout.hpp` 为准。
- dump 出来的每个控件都带有 `rectInWindow`；它表示控件相对窗口客户区的矩形位置，做命中、自查和自动化点击时优先使用这个字段。
- 实际切页或点控件时，优先先 dump 当前页拿到最新控件树，再用目标控件的 `rectInWindow` 算出点击坐标，然后调用 `/dumpMouseClick`；不要在旧页面、旧状态或上一次 dump 的坐标基础上连续盲点。
- 如果要检查控件彼此是否对齐，先用 `layout` 里的 `rectInWindow` 找出横向或纵向没对齐的节点，再回到截图里确认用户最终看到的效果是否也不对；不要只凭肉眼猜，也不要只凭坐标下结论。布局自查不止是“横平竖直”，还要继续判断整体观感是否顺眼。
- 如果需要点击某个控件、切换页签或切到别的页面，优先通过 `GET /dumpMouseClick?x=<x>&y=<y>` 发送点击；默认按左键点击处理，必要时可以追加 `button=right` 发右键点击。点击坐标优先取目标控件 `rectInWindow` 范围内的有效区域，通常取中心点最稳。
- 如果需要对某个控件发送键盘按键或输入文本，必须先调用一次 `/dumpMouseClick` 让目标控件获得焦点，然后再调用键盘相关接口；不要在未聚焦状态下直接调用 `/dumpKeyClick` 或 `/dumpInsertText`。
- `GET /dumpKeyClick?code=<vk>` 用于发送一次按键按下再弹起；这里的 `code` 是 Win32 虚拟键码，例如 `VK_RETURN` 对应 `13`。
- `POST /dumpInsertText` 用于直接向当前已聚焦控件插入文本；请求体使用表单参数写法，在 body 中传 `text=xxx`。需要多字符、空格或中文时，按 `application/x-www-form-urlencoded` 规则编码后再发送。
- 优先检查当前可见页面，不要把隐藏 `TabControl` 页面、折叠区域、未激活页签或滚动裁剪区域里的 `0x0` / 负坐标节点直接当成真实布局 bug。
- 用 `/dumpScreenshot` 返回的截图判断“是否符合预期”时，优先关注这些视觉问题：文本或图标是否看起来没显示全、同组控件是否真的居中或对齐、选中态或悬浮态背景是否完整、圆角、边框、阴影和留白是否符合设计预期。
- 美观性检查至少要额外看这些点：信息层级是否清楚、主次是否一眼可分、留白和间距节奏是否统一、列表或卡片密度是否舒服、文本块和图标块是否有明显头重脚轻或左重右轻、按钮和输入框是否显得拥挤或发飘、相邻模块之间是否存在视觉断裂或过于平均导致的“呆板感”。
- 纠正布局时，优先关注：子控件是否溢出父控件、文本或图片是否被裁切、同组控件是否依赖可疑的硬编码 `margin-left/top` 才“看起来对齐”、截图里是否出现和 `layout` 不一致的可见错位；如果两者不一致，要继续排查样式、字体字形、图标字符、边框、内外边距和裁剪链路。
- 改完 XML 或样式后，要重新构建并重新运行一次 `Debug` 程序，再重新按 `dumpState -> dumpLayout -> dumpScreenshot -> dumpMouseClick -> dumpLayout -> dumpScreenshot ...` 的顺序读取新的状态、布局和截图做复验；如需键盘输入，则在这条链路里插入“`dumpMouseClick` 聚焦 -> `dumpKeyClick` / `dumpInsertText` -> dumpLayout -> dumpScreenshot`”这一段；但在同一轮自查里，只要程序状态正常，就优先复用这一次启动的会话连续完成多个页面，不要为每个页面都重复启停程序。

### 9. Shell 组件按需引入

- `PopupMenu`：需要自定义多级右键菜单时使用，支持 `AddItem`、`AddSubMenu` 和 `MenuClick` 回调。
- `SystemMenu`：需要 Win32 原生右键菜单风格时使用，通过整数 ID 标识菜单项。
- `ToolTip`：控件提示文字，可通过 XML 的 `tips` 属性或代码 `SetGlobalToolTip(...)` 设置。
- `TrayIcon`：系统托盘类。按源码确认支持 `SetIcon(HICON)`、`SetTips(...)`、`SetMenu(SystemMenu*)`、`ShowMessage(...)` 和 `EventHandler` 鼠标事件回调。
- `WindowShadow`：`BorderlessWindow` 已内建阴影支持；如需手动控制阴影，可直接使用 `WindowShadow`。

### 10. 用 UISelector 批量操作控件

- `UISelector` 提供 jQuery 风格的链式控件选择器。
- 可按 ID、class、类型、属性组合查找控件并批量操作。
- 适合在运行时批量修改样式或状态，减少逐个 `FindControl` 的样板代码。

## 设计规则

- 修改已有界面时，优先继承既有视觉语言，不要无理由换一整套风格。
- 用户要求做全新视觉方向而项目里没有明确基调时，先确认目标风格再设计。
- 优先做“桌面应用”而不是“网页模板”式视觉：主次清楚、控件可读、间距克制、阴影和圆角不过量。
- 所有可点击控件都应有明确 hover 或 focus 反馈，必要时加 `cursor:pointer`。
- 不使用 emoji 充当图标，优先资源图片、字体图标或自绘。
- 深色底上的 PNG 图标必须人工检查对比度；看不清就视为设计错误。
- 动效保持克制，常规交互反馈优先落在 `150-300ms` 量级。
- 非明确需求下，不要让 hover 改变控件尺寸、边距或布局结构。
- 对输入框、滚动条、下拉框、列表行这些高频控件，优先保证可读性和可操作性。
- 深色和浅色都要看对比度，不要出现按钮可点击但边界不可见的情况。
- 修改 `.htm` 之后，至少做一次真实运行或 XML 预览验证，优先检查三件事：文本是否叠加、内容是否裁切、图标是否消失。

## 何时继续读取附加文档

- 需要找你自己沉淀的本地页面或窗口参考时，先读 `references/demos/`。
- 需要直接拿一个可复用骨架起步时，先读 `assets/templates/`。
- 需要快速理解库结构和头文件职责时，读取 `references/library-map.md`。
- 需要判断 `.htm/.style` 写法和真实 HTML/CSS 的差异时，读取 `references/htm-css-dialect.md`。
- 需要一份不依赖 demo 的 htm/style/animation 速查表时，读取 `references/syntax-quick-reference.md`。
- 需要查阅已验证的实现模式和代码片段时，读取 `references/demo-patterns.md`。
- 需要查找特定枚举值、回调签名、颜色格式或全局工具函数时，读取 `references/syntax-quick-reference.md` 的第 12-16 章。
- 需要查找 Shell 组件（菜单、托盘、提示）或工具类（UISelector、DebounceTimer、MessageQueue）的 API 时，读取 `references/library-map.md` 的"Shell 与弹出组件"和"基础设施和工具类"章节。
