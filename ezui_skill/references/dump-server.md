# EzUI dump server 自查

仅在需要自动检查窗口状态、控件树、截图、点击或键盘输入时读取本文件。

## 1. 启动

使用 Debug 构建，并在程序命令行显式追加：

```text
--dump-server=<port>
```

例如 `--dump-server=9631`。没有该参数时不会启动服务。服务监听 `127.0.0.1:<port>`。

## 2. 标准接口

| 方法与路径 | 用途 |
| --- | --- |
| `GET /dumpState` | 获取可用窗口列表 |
| `GET /dumpLayout?hwnd=<hwnd>` | 获取当前可见控件树 |
| `GET /dumpScreenshot?hwnd=<hwnd>` | 下载窗口客户区 BMP 截图 |
| `GET /dumpMouseClick?hwnd=<hwnd>&x=<x>&y=<y>` | 模拟单击，可加 `button=right` |
| `GET /dumpMouseDoubleClick?hwnd=<hwnd>&x=<x>&y=<y>` | 模拟双击 |
| `GET /dumpKeyClick?hwnd=<hwnd>&code=<vk>` | 模拟一次按键按下和释放 |
| `POST /dumpInsertText` | 向当前已聚焦控件插入文本；表单 body 为 `hwnd=<hwnd>&text=...` |

除截图成功时返回 BMP 二进制外，JSON 成功和错误响应统一使用：

```json
{ "code": 0, "data": {}, "message": "" }
```

JSON 接口以 `code == 0` 为成功。`/dumpScreenshot` 以 HTTP 200 为成功，返回原始 BMP，不是 JSON 或 Base64。

在 PowerShell 中调用 JSON 接口统一使用 `Invoke-RestMethod`；下载截图可使用 `Invoke-WebRequest -OutFile`。

## 3. 标准时序

1. 启动程序后等待页面和数据初始化。
2. 每隔约 2 秒轮询 `/dumpState`，直到 `code == 0` 且 `data` 数组非空。
3. 从目标窗口记录 `hwnd`；后续每个接口都显式传入该值。
4. 调用 `/dumpLayout` 取得最新控件树。
5. 调用 `/dumpScreenshot` 检查真实视觉。
6. 需要交互时，从当前 layout 中取目标控件的 `rectInWindow`，使用其有效区域或中心点点击。
7. 页面稳定后重新获取 layout 和 screenshot；不要复用旧页面或旧状态的点击坐标。

多页面检查优先“一次启动，连续切页，连续 dump”，不要为每页重复启停程序。

推荐顺序：

```text
dumpState -> dumpLayout -> dumpScreenshot
          -> dumpMouseClick -> dumpLayout -> dumpScreenshot
          -> ...
```

键盘输入顺序：

```text
dumpMouseClick 聚焦 -> dumpKeyClick / dumpInsertText
                    -> dumpLayout -> dumpScreenshot
```

`/dumpKeyClick` 的 `code` 是 Win32 虚拟键码，例如 Enter 为 `13`。`/dumpInsertText` 使用 `application/x-www-form-urlencoded`；多字符、空格或中文必须正确编码。

## 4. 重点字段

`/dumpState` 的 `data` 是窗口数组，元素通常包含 `hwnd`、`title`、`x`、`y`、`width`、`height`。

`/dumpLayout` 重点查看：

- `data.layout`
- `path`
- `type`
- `name`
- `classes`
- `rect`
- `rectInWindow`
- `scaledValue`
- `text`
- `children`

字段定义以项目中的 `sources/core/dump/DumpLayout.hpp` 为准。`rectInWindow` 是相对窗口客户区的位置，命中、对齐检查和自动点击优先使用它。

控件树只包含视觉可见控件。不要把隐藏 Tab 页面、折叠区域、未激活页签或滚动裁剪区域中的 `0x0` / 负坐标节点直接判定为布局 bug。

## 5. 落盘规则

layout JSON 和 BMP 写到用户可写的临时目录，不写程序目录、构建目录或 demo 可执行文件旁边。推荐：

```text
%TEMP%/EzUI_dump/<应用名>/<会话时间戳>/<序号_页面名>/layout.json
%TEMP%/EzUI_dump/<应用名>/<会话时间戳>/<序号_页面名>/screenshot.bmp
```

同一轮自查的所有页面放在同一会话目录，便于连续复查。

## 6. 如何判断问题

必须结合 layout 和 screenshot：

- layout 用于确认控件树、坐标、尺寸、命名和命中关系。
- screenshot 用于确认真实视觉、字体、图标、边框、圆角、阴影、留白和层级。

优先检查：

- 文本或图片是否裁切、叠加或溢出父控件。
- 同组控件是否真正对齐，是否依赖可疑的硬编码 `margin-left/top`。
- hover、checked、focus 等状态背景是否完整。
- 信息层级、间距节奏、列表密度和模块间视觉连接是否合理。
- 图标与背景对比度是否足够。
- layout 坐标与截图观感不一致时，继续排查字体字形、边框、内外边距、样式和裁剪链路。

修改 XML、样式或布局后，重新构建并运行 Debug 程序，再按标准时序复验。只要当前会话状态正常，就继续复用该进程检查其他页面。