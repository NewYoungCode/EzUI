# QuickUI Demo

这个目录收录从 `D:\C++\EzUI\demo\QuickUI` 同步过来的参考 demo，并保留了接近原项目的目录结构，方便直接按入口文件和 `res/` 资源去理解布局。

目录说明：

- `main.cpp`：应用入口，展示 `Application`、高 DPI 和资源初始化流程。
- `mainForm.cpp` / `mainForm.h`：主窗口 code-behind，包含 `LoadXml("res/mainForm.htm")`、搜索过滤和页签同步逻辑。
- `res/mainForm.htm`：QuickUI 的主界面布局，适合借鉴导航台式的两栏结构和视觉分区。
- `res/ctrl_docs/*.htm`：控件级说明页，适合借鉴单控件文档的说明卡片、示例区和代码区编排。
- `res/ctrl_docs/assets/`：`PictureBox` 页面依赖的图片与 GIF 资源。

说明：

- 这里保留的是源码和布局资源，不包含编译产物 `QuickUI.res`。
- 如果后续 QuickUI 源目录有更新，优先按这个目录结构继续同步，避免把入口代码和 `res/` 布局拆散。
