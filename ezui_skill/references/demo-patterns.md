# EzUI Demo 模式

## 本地 demo 优先级

如果 `references/demos/` 里已经有你自己放进去的同类 demo，优先读那个，再读下面这些框架级参考模式。

建议维护方式：

- demo 文件名直接表达用途，例如 `quickui/mainForm.cpp`、`wechat-main-window.htm`、`borderless-titlebar.cpp`。
- 每加一个值得复用的 demo，就在本文件补一行索引，说明它最适合借鉴什么。
- 如果某个 demo 已经稳定到可以当脚手架，不要只放 demo，最好再整理一份到 `assets/templates/`。

当前本地 demo 索引：

| 文件 | 最适合借鉴的场景 | 说明 |
| --- | --- | --- |
| `references/demos/quickui/mainForm.cpp` | `Window + LoadXml` 入口和搜索联动 | 展示 QuickUI 主窗口如何加载 `res/mainForm.htm`，并在左侧 `VListView` / `RadioButton` 与右侧 `TabControl` 之间做搜索过滤和页签同步 |
| `references/demos/quickui/res/mainForm.htm` | 控件导航台 / 文档工作台布局 | 大型两栏界面，包含 hero 区、搜索卡片、`VListView` 导航和 `TabControl + iframe` 内容舞台 |
| `references/demos/quickui/res/ctrl_docs/Control.htm` | 单控件文档页模板 | 代表 `ctrl_docs/*.htm` 的写法，适合借鉴说明卡片、示例区、API 速查和局部样式组织 |
| `references/demos/quickui/res/ctrl_docs/frame.htm` | `Frame` / `iframe` 子页面正例 | 展示 `src="res/ctrl_docs/frame_child.htm"` 的相对路径用法和子页面样式隔离 |

## 已验证的参考模式

| 模式来源 | 最适合借鉴的场景 | 应该抄的模式 |
| --- | --- | --- |
| EzUI 官方 helloWorld 示例 | code-first 页面和 API 快速摸底 | 手写控件树、`SetLayout(...)`、`CheckedStyle`、模态窗口 |
| EzUI 官方 QQ 示例（启动流程） | 真实应用启动流程 | `Application`、资源包、自定义控件注册、登录到主界面切换 |
| EzUI 官方 QQ 示例（登录表单） | XML-first 表单 | `UILoader`、`SetupUI`、按 ID 查找控件、统一事件路由 |
| EzUI 官方 QQ 示例（主窗口） | 透明桌面壳层、tab、运行时插入列表 | `BorderlessWindow`、`EnableTransparency()`、`LoadXml`、`Append(...)`、`Invoke(...)` |
| EzUI 官方 QQ 示例（会话列表） | 列表行样式 | class 复用、hover 态、嵌套布局 |
| EzUI 官方 ResPackage 示例 | 从内存加载 XML、后台任务进度 | `ui.LoadXml(data, size)`、`Task`、`Invoke` |
| EzUI 官方 XMLPreview 工具 | 快速调 XML 布局 | 文件监听、实时预览、低成本试样式 |
| EzUI 官方 kugou 示例 | 音乐播放器 UI、媒体控件布局 | 播放控制栏、进度条（Slider）、歌曲列表、专辑封面展示、自定义绘制 |

## 已验证的实现套路

### 0. 先验证 htm/style 方言

在改任何 `.htm` 文件前，先做这四步：

1. 找 `README.md` 和项目中同类页面里的正例。
2. 对陌生属性或陌生值做项目精确搜索。
3. 把目标页面当成 EzUI DSL，而不是网页模板。
4. 如果页面是固定尺寸窗体，先做尺寸预算再开始设计。

这一步没有完成，就不要直接上新风格。

### 1. XML-first 表单 + code-behind

界面结构明显、交互逻辑中等时优先这么写：

```cpp
class LoginForm : public Form {
    UILoader ui;
public:
    LoginForm() : Form(320, 448) {
        ui.LoadXml("res/loginForm.htm");
        ui.SetupUI(this);
    }
};
```

然后把行为收敛到 `OnNotify(...)`：

- 用 `FindControl("user")`、`FindControl("pwd")`、`FindControl("btnLogin")` 定位控件。
- 根据 `args.EventType` 判断触发方式。
- 读写控件状态，不要在 XML 中塞过多逻辑。

补充一条硬规则：

- 示例里的 `Form(320, 448)` 只是当时示例的窗口尺寸，不要把它当成可无限承载新设计的固定模板。

`RadioButton + TabControl` 常见骨架：

```xml
<vbox>
    <hbox height="36" items-align="middle left">
        <radiobutton text="Overview"></radiobutton>
        <radiobutton text="Security" style="margin: 0px 0px 0px 12px;"></radiobutton>
        <label text="ignored" style="margin: 0px 0px 0px 12px;"></label>
        <radiobutton text="About" style="margin: 0px 0px 0px 12px;"></radiobutton>
    </hbox>
    <tabcontrol pageindex="0">
        <vbox></vbox>
        <vbox></vbox>
        <vbox></vbox>
    </tabcontrol>
</vbox>
```

规则：

- 这 3 个同级 `RadioButton` 会自动组成互斥组。
- 中间的 `Label` 会被自动忽略，不参与索引计算。
- 三个 `RadioButton` 的索引分别是 `0`、`1`、`2`，点击后会切到 `TabControl` 的第 `0`、`1`、`2` 页。

### 2. 运行时插入列表项

数据来自接口、文件或后台任务时，优先复用这个模式：

```cpp
auto* sessionList = FindControl("sessionFrame")->FindControl("charList");
Control* ctrl = sessionList->Append(xml);
Attach(ctrl);
sessionList->Invalidate();
```

注意：

- 追加的 XML 片段要自包含。
- 需要跟随窗口生命周期释放时，记得 `Attach(...)`。
- 插入完成后刷新对应容器。
- 最外层尽量统一成一类 item 容器，方便 hover 和选中态统一作用。
- 如果整行 item 要统一响应点击或 hover，内部子控件统一加 `event="none"`，把命中留给最外层 item 容器。
- `event="none"` 就按“忽略鼠标事件”理解即可，适合把整块鼠标事件交给最外层 item 容器。
- 固定间隙尽量少用 `Spacer`；`Spacer` 更适合占位拉伸。常规留白优先放到子控件自己的 `margin`。
- 但在固定高度容器里，如果中间是一组 autosize 文本，为了把整组内容推到视觉居中，可以在文本组前后各放一个 `Spacer` 做占位。这属于合理使用 `Spacer` 的例外场景。
- 在 `vbox` / `hbox` 里做“头部 + 内容区”或“头像 + 内容区”这类布局时，如果某块只是要吃剩余空间，不要写 `height="100%"` 或 `width="100%"`；应省略对应维度，交给布局器分配。
- `vbox` 里的子控件在水平方向默认就会吃满可用宽度，通常不要再补 `width="100%"`；`hbox` 里的子控件在垂直方向默认就会吃满可用高度，通常也不要再补 `height="100%"`。只有在真的要表达比例尺寸时，才再写百分比。
- 如果某个控件已经在某个方向上写了 `100%`，就不要再给它叠加同方向的 `margin`；要留白优先交给父容器的 `padding` 或其他布局控件处理，否则很容易显示出界。水平和垂直同理。
- `margin`、`padding` 这类长度值统一显式写 `px`，不要再写成 `margin: 0 28 0 28` 这种省略单位的形式。
- 大字号 `Label` 优先先给 `height="auto"`，避免字体放大后还沿用旧高度导致裁切。
- 如果 `Label` 需要单行超长省略，优先用 `ellipsis="..."`，并保持稳定单行高度；不要把“自动撑高”和“单行省略”混成一套写法。
- 如果 item 放在 `vlist` / `hlist` 里，并且你要让它和列表容器保持同宽或同高，可以直接给 item 写 `width="100%"` 或 `height="100%"`。这里要先区分父控件类型：列表控件的 item 尺寸保持，不等于布局器的主轴剩余空间分配。
- 如果 item 已经跟 `vlist` / `hlist` 保持同宽或同高，缩进优先放到列表控件自己的 `padding`，不要再给 item 本身写对应方向的左右或上下 `margin`；这样可以减少滚动条遮住子控件的情况。水平和垂直同理。
- 需要做标题栏拖动或窗口按钮时，优先直接使用控件的 `action` 属性：`title`、`move` / `movewindow`、`mini`、`max`、`close` 都是源码确认支持的值。
- 如果一组文本 `Label` 都是 `autosize`，先按 HBox / VBox 的默认对齐去理解，再决定是否需要补单值或双值 `items-align`。放在 `vbox` 里时，默认就会按 `items-align="top center"` 摆放，只有要改成靠左或靠右时才显式写 `items-align="left"`、`items-align="right"` 或 `items-align="top left"` / `items-align="top right"`；放在 `hbox` 里时，默认就会按 `items-align="middle left"` 摆放，只有要改成靠上、靠下或主轴居中时才显式写 `items-align="top"`、`items-align="bottom"`、`items-align="center"` 或 `items-align="top left"`、`items-align="bottom left"`、`items-align="middle center"`。设置容器内控件对齐时统一写 `items-align`；`item-align` 是旧属性名，现已弃用。

列表项推荐骨架：

```xml
<hbox class="session-item">
    <label text="PD" class="avatar" event="none"></label>
    <vbox items-align="top left" event="none">
        <hbox items-align="middle left" event="none">
            <label text="Product Team" event="none"></label>
            <spacer event="none"></spacer>
            <label text="11:24" event="none"></label>
        </hbox>
        <label text="Latest message" event="none"></label>
    </vbox>
</hbox>
```

### 3. 用 `Frame` 做隔离子页面

当子页面会复用很多 class 或 ID，或者需要单独维护 XML 时，优先用 `Frame`。这里也要记住：`Frame` / `<iframe>` 本质上仍然是控件，不属于 window 层；它只是额外提供了子树隔离效果。

适合场景：

- 左侧导航点开一个独立内容面板。
- 一个大控件内部再加载一份 XML。
- 需要隔离样式作用域，避免父页面选择器污染子页面。

现成参考：

- `EzUI/Frame.h`
- EzUI 官方 QQ 示例的主窗口头文件
- EzUI 官方 QQ 示例的主窗口 .htm 中的 `<iframe id="sessionFrame" src="res/session.htm"></iframe>`

### 4. 后台任务 + 安全 UI 更新

文件打包、批量加载、解析耗时数据时，优先参考 `ResPackage`：

```cpp
task = new Task([this]() {
    bool ok = Resource::Package(dir, outFile, [=](const UIString& file, int index, int count) {
        Invoke([&]() {
            labelTips->SetText(...);
            labelTips->Invalidate();
        });
    });
});
```

规则：

- 耗时逻辑交给 `Task`。
- 所有 UI 更新统一回到 `Invoke(...)` 或 `BeginInvoke(...)`。
- 进度回调不要过细，避免 UI 线程被刷爆。

### 5. 轻量动效

QQ 示例的主窗口淡入就是标准范式：

```cpp
Animation* ani = new Animation(this);
ani->ValueChanged = [this](float value) {
    SetOpacity(value);
    Invalidate();
};
ani->SetStartValue(0);
ani->SetEndValue(1);
ani->Start(1000);
```

适合：

- 淡入淡出
- tab 或面板滑动
- 滚动条或浮层的补间动画

不建议：

- hover 改尺寸
- 长时间纯装饰性动画
- 影响输入和阅读稳定性的动效

更稳的 EzUI 动画接入点：

- `OnShow()`：窗口初次显示时做淡入或上浮。
- `OnClose()`：需要退场动画时先拦截关闭，再在动画结束后真正退出。
- `OnNotify()`：菜单、tab、列表点击时触发切页或局部过渡。
- `TabLayout::SlideToPage(...)`：用于页面切换，比手写批量位移动画更稳。

### 6. Slider 进度控制

音乐播放器、视频、音量调节等场景，优先参考 kugou 示例：

```cpp
auto* slider = FindControl("progressBar");
// Slider 的 ValueChanged 回调
slider->ValueChanged = [this](Slider* s, float value) {
    // 更新当前播放进度
    UpdateProgress(value);
};
slider->SetMin(0);
slider->SetMax(100);
slider->SetValue(currentProgress);
```

适合：
- 音乐/视频播放进度
- 音量调节
- 亮度、透明度等数值控制

### 7. RichTextView 富文本展示

需要展示格式化文本时（如关于页面、帮助文档），使用 `RichTextView`：

```cpp
auto* richText = FindControl("aboutText");
richText->SetHtml("<p>版本 <span style='color:blue'>1.0.0</span></p><p>作者：<a href='https://example.com'>EzUI</a></p>");
```

支持的 HTML 子集标签：`<p>`、`<a>`、`<span>`（含内联 style）。

### 8. PopupMenu 右键菜单

需要右键菜单时：

```cpp
PopupMenu menu;
menu.AddItem("复制", [](Control*) { /* ... */ });
menu.AddItem("粘贴", [](Control*) { /* ... */ });
auto* subMenu = menu.AddSubMenu("更多");
subMenu->AddItem("设置", [](Control*) { /* ... */ });
menu.Show(x, y);
```

如果要使用系统原生菜单风格：

```cpp
SystemMenu sysMenu;
sysMenu.AddItem(1001, "打开文件");
sysMenu.AddItem(1002, "退出");
sysMenu.MenuClick = [](UINT_PTR id) {
    if (id == 1001) { /* ... */ }
};
sysMenu.Show(hwnd, x, y);
```

### 9. TrayIcon 系统托盘

需要最小化到托盘时：

```cpp
TrayIcon tray;
tray.SetIcon(hTrayIcon);
tray.SetTips("程序仍在后台运行");
tray.EventHandler = [this](const MouseEventArgs& e) {
    if (e.Button == MouseButton::Left) {
        Show(); // 点击托盘图标恢复窗口
    }
};
tray.ShowMessage("提示", "程序已最小化到托盘");
```

## 把 UI/UX 规则翻译成 EzUI 做法

| 目标 | 在 EzUI 中的推荐落地方式 |
| --- | --- |
| 交互稳定 | hover 优先改颜色、透明度、边框、图片，不轻易改宽高 |
| 可点击感明确 | 所有可点击控件都给 hover 或 active 反馈，必要时加 `cursor:pointer` |
| 输入可读 | 通过 `style` 给 `TextBox` 足够的 `padding`，边框和 placeholder 对比度要清楚 |
| 层级清晰 | 通过布局嵌套、留白和一个主色建立层级，不要堆过多强饱和色 |
| 作用域安全 | 复杂子页面优先放进 `Frame`，不要把所有选择器摊平在一棵树上 |
| 列表易用 | 保证列表项高度、滚动条可发现性和选中态区分度 |
| 图标一致 | 优先图片资源、字体图标或自绘，不用 emoji |
| 方言安全 | 先用项目里已验证的 `.htm/.style` 写法，再尝试更冒险的视觉表达 |

## EzUI 桌面界面的默认设计取向

除非现有页面或用户要求明确相反，否则默认遵循：

- 用中性浅色或低噪音桌面底色起稿。
- 整页只保留一个主强调色和一个次级文本色。
- 圆角适中，不要到处大圆角。
- 输入框和主按钮写在 `style` 里的 `padding` 要偏充足。
- 动效服务于反馈和连续性，不服务于炫技。
- 模态框、下拉框、右键菜单和主页面应共享同一套间距和主色系统。

## 构建与运行注意点

> 以下构建信息仅适用于 EzUI 框架源码开发场景，使用 EzUI 作为依赖库的项目可跳过本节。

| 说明 | 为什么要看 |
| --- | --- |
| EzUI 源码仓库根目录的 `CMakeLists.txt` | `BUILD_EZUI`、`BUILD_SHARED_LIBS`、`BUILD_DEMO`、`add_resource_package(...)` 都在这里 |
| EzUI 源码仓库的 demo `CMakeLists.txt` | 哪些 demo 可直接运行，以及资源打包怎么挂到目标上 |
| `说明(构建时不要包含中文路径).txt` | 编码和路径注意事项 |

当前项目的两个实务点：

- Visual Studio 路径或编码处理不当时，中文路径可能导致编译失败。
- 资源打包标准流程是 `ResPackage.exe -package ...`。

再加三条页面修改时必须检查的实务点：

- 固定尺寸窗体会裁切超界内容，不要默认它会自动适应。
- 深色主题下，PNG 图标极易和背景融在一起。
- 独立文本 `Label` 在垂直布局里先判断是否更适合 `auto`；只有不适合随文字自适应时再显式给 `height`。

## 做事决策表

| 任务类型 | 优先路径 |
| --- | --- |
| 修改现有页面视觉 | 先改 XML 和样式块，再补行为代码 |
| 新增复用控件 | 定义自定义控件并 `RegisterControl<T>()` |
| 嵌入独立小页面 | 用 `Frame` 或 XML 中的 `iframe` |
| 运行时拼装列表 | 在列表容器上用 `Append(...)` 插入 XML 片段 |
| 做图表或复杂图形控件 | 继承 `Control` 或 `Label`，走 `Graphics` 自绘 |

## 交付前验证

改完 `.htm` 或主题后，至少做一次真实验证：

1. 运行目标程序或用 `XMLPreview` 看页面。
2. 检查文本是否叠加。
3. 检查控件是否超出框体被裁切。
4. 检查图标在当前底色上是否可见。
5. 检查动画是否造成二次布局抖动。
