# EzUI 常用模式

仅在任务需要窗口入口、动态 Item、Frame、异步更新、动画或 Shell 组件示例时读取。布局方言见 [htm-css-dialect.md](htm-css-dialect.md)，精确 API 见 [syntax-quick-reference.md](syntax-quick-reference.md)。

## 最小贯通 demo

普通任务优先搜索目标项目中的同类代码；需要一套完整正例时再读取：

| 文件 | 内容 |
| --- | --- |
| `demos/quickui/main.cpp` | `Application`、`Window::LoadXml`、`TextBox::TextChanged`、筛选与切页 |
| `demos/quickui/res/mainForm.htm` | 固定控件树、样式、`RadioButton + TabControl` |
| `demos/quickui/res/frame_child.htm` | `Frame` 子页面及样式隔离 |

完整应用示例直接查看 EzUI 源码仓库的 `demo/`，不要把其资源和控件文档复制进 skill。

## 布局 + code-behind

固定结构放在 `.htm`，C++ 加载页面并绑定行为：

```cpp
class LoginWindow : public Window {
public:
    LoginWindow() {
        SetSize(320, 448);
        LoadXml("res/loginForm.htm");
        if (auto* input = FindControl<TextBox>("query")) {
            input->TextChanged = [this](TextBox*, const UIString& text) {
                ApplyFilter(text);
            };
        }
    }
};
```

窗口尺寸用 `SetSize(...)` 设置。动态数据只在布局中预留容器，不在页面类里拼整段 XML。

`RadioButton` 自动切页时，每个按钮都设置 `tabcontrol="<TabControl id>"`：

```xml
<hbox>
    <radiobutton text="概览" tabcontrol="pages" checked="true"></radiobutton>
    <spacer></spacer>
    <radiobutton text="设置" tabcontrol="pages"></radiobutton>
</hbox>
<tabcontrol id="pages">
    <vbox></vbox>
    <vbox></vbox>
</tabcontrol>
```

页面索引按按钮在父控件中的顺序计算，只忽略 `Spacer`；不要在按钮之间插入 `Label`、`Button` 等普通控件。

## 动态 Item

每类列表项封装为独立控件，由页面创建并插入预留容器：

```cpp
auto* list = FindControl<VListView>("sessions");
auto* item = new SessionItem(data);
item->SetStyle("height: 56px;", VisualState::Normal);
list->AddChild(item, true);
list->RefreshLayout();
list->Invalidate();
```

- 指定位置时使用 `InsertChildAt(...)`。
- Item 自己保存数据、构建内部控件并绑定 item 级事件。
- 动态控件不会自动匹配加载时的 `<style>`；用 `SetStyle(..., VisualState)` 设置状态。
- 整行统一响应鼠标时，内部子控件设置 `event="none"`。

## Frame 子页面

需要独立 XML、复用大量 ID/class 或隔离样式时使用：

```xml
<iframe id="details" src="res/details.htm"></iframe>
```

`Frame` 是控件而不是窗口。外层 `FindControl` 不穿透其子树；先取得 `Frame`，再从 `GetLayout()` 或 Frame 内部查找。

## 后台任务更新 UI

后台任务不直接访问 UI；窗口或控件使用弱引用，并通过 `BeginInvoke(...)` 回到 UI 线程：

```cpp
class MainForm : public Window {
    ThreadPool workers{1};

    void StartWork() {
        auto self = GetWeakPtr<MainForm>();
        workers.Add([self] {
            auto result = DoWork();
            BeginInvoke([self, result] {
                if (!self) return;
                if (auto* status = self->FindControl<Label>("status")) {
                    status->SetText(result);
                    status->Invalidate();
                }
            });
        });
    }
};
```

一次性任务可用 `Thread`；排队或限制并发时使用长期存活的 `ThreadPool`。更完整规则见 [runtime-rules.md](runtime-rules.md)。

## 动画

```cpp
auto* animation = new Animation(this);
animation->ValueChanged = [this](float value) {
    SetOpacity(value);
    Invalidate();
};
animation->SetStartValue(0);
animation->SetEndValue(1);
animation->Start(200);
```

窗口透明度动画前先启用 `EnableAlphaBlending()`。切页优先使用 `TabControl::SlideToPage(...)`。

## Slider

```cpp
auto* slider = FindControl<Slider>("progress");
slider->SetMinValue(0);
slider->SetMaxValue(100);
slider->SetValue(currentValue);
slider->ValueChanged = [this](Slider*, float value) {
    UpdateProgress(value);
};
```

## RichTextView

```cpp
auto* view = FindControl<RichTextView>("about");
view->SetText("<p>版本 <strong>1.0.0</strong></p>");
```

仅使用框架已实现的 HTML 子集；需要复杂排版时查看当前 `RichTextView` 源码，不按浏览器能力推断。

## 菜单

自定义菜单：

```cpp
auto* menu = new PopupMenu(this);
menu->Append("复制");
menu->CreateMenu("更多")->Append("设置");
menu->MenuClick = [](PopupMenu::Item* item) { /* ... */ };
menu->SetPosition(x, y);
menu->Show();
```

系统原生菜单：

```cpp
SystemMenu menu;
auto openId = menu.Append("打开");
menu.MenuClick = [openId](UINT_PTR id) {
    if (id == openId) { /* ... */ }
};
menu.Show(GetWindowHandle(), Point(x, y));
```

## TrayIcon

```cpp
class MainForm : public Window {
    TrayIcon tray;

    void InitTray() {
        tray.SetIcon(hIcon);
        tray.SetToolTipText("程序仍在后台运行");
        tray.EventHandler = [this](const MouseEventArgs& e) {
            if (e.Button() == MouseButton::Left) Show();
        };
        tray.ShowBalloonTip("提示", "程序已最小化到托盘");
    }
};
```

`TrayIcon` 构造时已注册托盘图标，没有 `Show()` API；把它作为窗口成员，或传入 owner 绑定生命周期。
