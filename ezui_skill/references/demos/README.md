# EzUI Local Demos

这个目录放你自己维护的本地 EzUI demo。

推荐放这些内容：

- `.htm` / `.xml` 页面骨架
- `.cpp` / `.h` 小型窗口实现
- 单独验证某个属性、布局规则或控件行为的最小示例

命名建议：

- `quickui/mainForm.cpp`
- `wechat-main-window.htm`
- `borderless-titlebar.cpp`
- `vbox-items-align-demo.htm`

使用约定：

- 一个文件只表达一个清晰主题，不要把太多无关模式揉在同一个 demo 里。
- 如果 demo 依赖特殊前提，直接写在文件开头注释或配套说明里。
- 如果 demo 已经足够稳定、适合反复复用，再同步整理到 `assets/templates/`。

当前收录：

- `quickui/mainForm.cpp`：QuickUI 主窗口入口，适合借鉴 `LoadXml("res/mainForm.htm")`、搜索过滤和 `TabControl` 联动逻辑。
- `quickui/res/mainForm.htm`：QuickUI 主布局页，适合借鉴控制台式导航、侧栏检索和右侧文档舞台布局。
- `quickui/res/ctrl_docs/*.htm`：按控件拆分的文档子页，适合借鉴 `iframe` 子页、示例卡片和说明文案的组织方式。
