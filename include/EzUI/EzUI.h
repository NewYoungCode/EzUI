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
//
#pragma once

// ===== 核心相关 =====
#include "core/UIDef.h"
#include "core/UICore.h"
#include "core/UIEvent.h"
#include "core/Timer.h"
#include "core/UIStyle.h"
#include "core/Animation.h"
#include "core/Application.h"
#include "core/UISelector.h"
#include "core/UILoader.h"
#include "core/MessageQueue.h"
#include "core/String.h"
#include "core/Utility.h"
#include "core/SmartPtr.h"
#include "core/Resource.h"
#include "core/Thread.h"

// ===== 绘图相关 =====
#include "graphics/GraphicsTypes.h"
#include "graphics/Bitmap.h"
#include "graphics/Image.h"
#include "graphics/Graphics.h"
#include "graphics/GraphicsFont.h"
#include "graphics/GraphicsShapes.h"

// ===== 基础控件 =====
#include "control/Control.h"
#include "control/Label.h"
#include "control/Button.h"
#include "control/CheckBox.h"
#include "control/RadioButton.h"
#include "control/TextBox.h"
#include "control/ComboBox.h"
#include "control/PictureBox.h"
#include "control/Spacer.h"
#include "control/Slider.h"

// ===== 容器 / 布局 =====
#include "control/Frame.h"
#include "control/VLayout.h"
#include "control/HLayout.h"
#include "control/TabControl.h"

// ===== 滚动条 =====
#include "control/ScrollBar.h"
#include "control/VScrollBar.h"
#include "control/HScrollBar.h"

// ===== 数据展示类 =====
#include "control/VListView.h"
#include "control/HListView.h"
#include "control/TileListView.h"
#include "control/TreeView.h"
#include "control/DataGridView.h"
#include "control/PagedListView.h"
#include "control/RichTextView.h"

// ===== 窗口体系 =====
#include "window/Window.h"
#include "window/BorderlessWindow.h"
#include "window/PopupWindow.h"

// ===== 系统功能控件 =====
#include "shell/Menu.h"
#include "shell/TrayIcon.h"
#include "shell/ToolTip.h"




