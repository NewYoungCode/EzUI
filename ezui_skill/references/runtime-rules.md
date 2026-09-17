# EzUI 运行时与应用结构规则

仅在任务涉及 C++ 运行时代码、窗口、线程、生命周期、动态控件或拖放时读取本文件。

## 1. 应用结构

- 固定页面骨架、标题栏、导航、面板、固定按钮和动态区域占位容器放在 `.htm/.html` 中。
- 只有运行时才能确定的内容由 C++ 创建，例如列表项、数据卡片、动态角标和数据状态。
- 每一种动态 item 使用独立控件类；Item 自己保存数据、创建或绘制内部控件、更新状态并绑定 item 级事件。窗口或页面只负责取数据、找容器、创建/更新/销毁 Item 和处理页面级交互。
- 动态 Item 通过 `AddChild(...)` / `InsertChildAt(...)` 插入已知容器。不要把 `Append(...)` / `Prepend(...)` 当作常规插入方式，因为它们会解析 XML 字符串。
- 动态控件不会重新匹配布局文件中的 `<style>`；直接调用 `SetStyle(styleProperties, VisualState)` 设置 `Normal` 及所需的 `Hover`、`Active`、`Focus`、`Disabled`、`Checked` 差异样式。
- 自绘控件、画布、预览区和拖拽定位区可以在独立控件类里用 C++ 实现，但其宿主页面和固定容器仍优先放在布局文件中。
- 常规表单、列表和弹窗先用布局器表达父子关系和对齐；除非绝对定位本身就是业务语义，否则不用 `x/y`、重叠控件、负 `margin` 或手写 `SetPosition` 硬摆。

## 2. 窗口类型与 Alpha 混合

- `Window`：保留系统标题栏和边框。
- `BorderlessWindow`：自定义标题栏和阴影。需要透明、异形、圆角、淡入淡出或窗口透明度时，先调用 `EnableAlphaBlending()`。
- `PopupWindow`：下拉、临时面板和浮层。
- `Frame` / `<iframe>` 是带 ID、class 和样式作用域隔离的控件，不是顶层窗口。

调用 `EnableAlphaBlending()` 后：

- 用 `IsAlphaBlendingEnabled()` 判断是否已启用。
- `SetOpacity(...)` 只有在启用 Alpha 混合后才应使用。
- 根布局 alpha 为 `0` 的区域可能用于鼠标穿透；如果只想视觉透明但仍需命中、拖拽或响应鼠标，应给根布局非零 alpha 背景，例如 `#00000001`。
- 固定尺寸或分层窗口不会像浏览器一样自动扩展内容区域；先做好尺寸预算并真实验证裁切情况。

## 3. `ownerObject` 与弱引用

- 构造函数中的 `Object* ownerObject` 只表示生命周期绑定：传入非空 owner 后，对象会在 owner 销毁时自动销毁。
- `ownerObject` 不代表布局父子、事件路由、逻辑归属或窗口宿主。只有明确需要绑定销毁时机时才传非空 owner。
- 在回调、异步任务或 `BeginInvoke` 中捕获 `GetWeakPtr<T>()` 后，先判断 `if (!weakThis) return;`，再直接使用 `weakThis->Method()` 或成员。
- 不要写 `(T*)weakThis`、`static_cast<T*>(weakThis)`，也不要在检查存活前先缓存裸指针。只有外部 API 确实要求裸指针时才临时获取。

## 4. 线程、Timer 与异步派发

优先使用框架已有的 `Thread`、`ThreadPool`、`Timer`、`DebounceTimer` 和 `MessageQueue`，不要先写裸 `std::thread`、Win32 线程或重复的线程封装。

- `Timer` tick 默认按非 UI 线程回调处理；不得直接操作窗口、控件树、动画对象或其他线程不安全状态。
- `Timer::Restart(false)` 只重置等待时间，不立即执行 Tick；需要立即执行一次时使用 `Restart(true)`，不要使用旧的 `Reset()`。
- 后台任务或 Timer 的结果统一通过 `Invoke(...)` / `BeginInvoke(...)` 回到 UI 线程。
- Timer 与其他线程共享数据时，使用锁、原子变量、快照拷贝或明确的生命周期保护；不要因为回调很短就假定不会竞争。
- `DebounceTimer` 用于搜索等防抖场景；`MessageQueue` 使用公开的 `Post(...)` / `Exec()` 模型在线程间发布和消费任务。
- `Thread` / `ThreadPool` 任务边界会报告未处理的 C++ 异常和 Windows SEH；准确源码定位要求目标模块携带与二进制匹配的 PDB。普通 C++ 异常会结束当前任务，访问冲突等原生崩溃仍可能终止进程。

`BeginInvoke(...)` 的运行语义：

- 正常情况下，每个回调独立投递到通讯窗口。
- `PostMessage` 暂时失败时，任务进入 FIFO 重试队列；存在积压时，后续任务排在队尾，避免越过先前任务。
- 成功投递或进入重试队列时返回 `true`；调度器无效、正在退出或任务未被接收时返回 `false`。
- 回调进入 `Window::ShowModal()` 等嵌套消息循环后，后续回调仍可能执行；不要假设不同 `BeginInvoke` 回调绝不会嵌套。

## 5. 刷新与动画

- 普通视觉状态变化优先调用 `Invalidate()` 触发重绘；不要把 `Refresh()` 或 `RefreshLayout()` 当作常规刷新手段。
- 在代码中修改文字、图片、颜色、选中态、透明度、宽高、边距或内边距后，不要假设 UI 会自动刷新；默认补 `Invalidate()`。
- 新增、删除或改变会影响控件树和布局的结构时，再按需要调用 `RefreshLayout()`，随后 `Invalidate()`。
- hover 反馈优先改颜色、透明度、边框或位置，不改尺寸制造 relayout 抖动。
- 入场动画放在 `OnShow()` 或首次显示后；关闭动画由 `OnClose(bool& allowClose)` 控制，设为 `false` 可延迟关闭；切页优先使用 `TabControl::SlideToPage(...)`。

## 6. 文件拖放

应用层不要直接处理 `WM_DROPFILES` 或自行实现 `IDropTarget`：

1. 对目标控件调用 `SetDropEnabled(true)`。
2. 绑定 `Event::DragEnter`、`DragOver`、`DragLeave`、`Drop`、组合事件 `DragEvent`，或重写 `OnDragEnter`、`OnDragOver`、`OnDragLeave`、`OnDragDrop`。
3. 使用 `FileDragEventArgs::Files()` 读取文件，使用 `Position()` 读取相对当前控件的坐标。
4. 目标必须主动调用 `Accept()` 才表示接受拖放；默认拒绝。

框架只向 `IsDropEnabled()` 为 `true` 的命中控件及启用拖放的父控件冒泡。需要窗口级入口时重写 `Window::OnDragEnter/Over/Leave/Drop`；若仍要继续分发到控件树，调用基类对应实现。

## 7. 资源与公开能力

- 发布资源优先通过 `Application::SetResource(...)` 挂载资源包，打包使用 `Resource::Package(...)` 或项目现有 `ResPackage` 流程。
- XML 图片和子页面路径遵循项目已有约定，常见为 `res/...`。
- Shell 组件、UISelector、渐变和自绘 API 见 [library-map.md](library-map.md)；不要为了业务需求直接调用 `detail::`。
