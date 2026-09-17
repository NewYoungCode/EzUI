#pragma once
#include "UIDef.h"
#include <functional>

namespace ezui {
	namespace detail {
		//启动 UI 线程任务调度器(框架内部使用)
		extern EZUI_API void StartUIThreadDispatcher(WindowHandle hWnd);
		//停止 UI 线程任务调度器并清理尚未执行的任务
		extern EZUI_API void StopUIThreadDispatcher();
		//向 UI 线程投递任务，失败时进入重试队列
		extern EZUI_API bool PostUIThreadTask(std::function<void()>* callback);
	}
}
