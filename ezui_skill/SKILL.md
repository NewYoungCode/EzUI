---
name: ezui
description: 构建、修改、评审或调试基于 EzUI 2.x 的 Windows C++ 桌面 UI，涵盖 .htm 控件树与样式、窗口和控件、资源、线程、动画、自绘及框架源码。仅在项目实际使用 EzUI 时启用。
metadata:
  ezui_version: "2.0"
---

# EzUI

用于 EzUI 应用开发和框架源码修改。本文档按 EzUI 2.0 编写；若目标项目版本不同，以目标项目的公开头文件、实现和现有正例为准。

## 核心约束

1. **区分应用层与框架层**
   - 应用、demo 和业务代码只使用公开 API，不依赖 `ezui::detail::*`。
   - 只有明确修改框架内部机制时才能调整 `detail::`，且不得把内部依赖扩散到公开接口或示例。
   - 先复用现有 API、控件和工具；确认缺失后再补公共能力。

2. **把 `.htm` 当成 EzUI DSL**
   - 标签以 `RegisterControl<T>(...)` 注册表为准；属性和样式以源码、README 与项目正例为准。
   - 不推断浏览器 DOM、CSS、盒模型、自动内容流或 Web 属性兼容性。
   - 修改布局或样式前读取 [htm-css-dialect.md](references/htm-css-dialect.md)。

3. **固定骨架与动态数据分离**
   - 固定结构和占位容器写在 `.htm/.html`；C++ 负责数据、状态和运行时内容。
   - 每类动态列表项或卡片封装为独立控件，通过 `AddChild(...)` / `InsertChildAt(...)` 插入。
   - 动态控件不会自动匹配加载时的 `<style>`；用 `SetStyle(..., VisualState)` 设置状态样式。
   - 常规界面使用布局器；绝对定位只用于自绘、画布或拖拽等确有需要的场景。

4. **遵守线程和生命周期**
   - 后台任务和 `Timer` 回调不直接操作 UI，使用 `Invoke(...)` / `BeginInvoke(...)`。
   - 视觉变化通常调用 `Invalidate()`；结构变化或确需重排时才调用 `RefreshLayout()`。
   - 异步捕获对象使用弱引用；`ownerObject` 只表示生命周期绑定。

5. **最小改动并验证**
   - 先搜索同类页面、控件和 API，再实施范围最小的改动。
   - 不直观的线程切换、自绘、布局计算和资源生命周期保留简短“为什么”注释。
   - 框架改动若改变稳定语义，同步更新对应参考文档，不记录临时调试过程。

## 工作流

1. 确认 EzUI 版本，并判断是应用开发还是框架修改。
2. 搜索目标项目和框架源码中的同类正例。
3. 只读取当前任务需要的参考文档。
4. 实施最小改动并构建。
5. 布局任务真实运行、预览或 dump，检查裁切、溢出、图标、交互状态和线程安全。

## 按需读取

| 任务 | 参考 |
| --- | --- |
| `.htm`、样式、尺寸、对齐和状态 | 必读 [htm-css-dialect.md](references/htm-css-dialect.md)；查精确标签、属性、枚举或回调时再读 [syntax-quick-reference.md](references/syntax-quick-reference.md) |
| 窗口、动态 Item、Frame、列表、动画和常见 UI | [demo-patterns.md](references/demo-patterns.md)；仅需贯通正例时再读 [references/demos/quickui/](references/demos/quickui/) |
| 模块、头文件、窗口、控件、Shell、资源和自绘 | [library-map.md](references/library-map.md) |
| 线程、Timer、生命周期、刷新、透明窗口和拖放 | [runtime-rules.md](references/runtime-rules.md) |
| 自动检查控件树、截图和交互 | [dump-server.md](references/dump-server.md) |
| Launcher / Updater / App 更新闭环 | [application-update.md](references/application-update.md) |

## 交付底线

- 应用层没有 `detail::` 依赖。
- 没有把 EzUI DSL 当成浏览器 HTML/CSS。
- 动态内容、线程切换和生命周期边界清楚。
- 布局经过真实验证；不使用 emoji 代替图标，不通过 hover 改尺寸制造布局抖动。
