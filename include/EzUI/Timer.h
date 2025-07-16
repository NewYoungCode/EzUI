#pragma once
#include "EzUI.h"
#include "Task.h"
namespace ezui {

	//使用线程的计时器 不与主进程同步(启动的时候就直接开始执行回调函数)
	class UI_EXPORT Timer :public IControl {
		bool m_bExit = false;
		bool m_bStop = true;
		Task* m_task = NULL;
		std::mutex m_mtx;
		std::condition_variable m_condv;
	public:
		std::function<void(Timer*)> Tick = NULL;
		int_t Interval = -1;
	public:
		Timer();
		bool IsStopped();
		void Start();
		void Stop();
		virtual ~Timer();
	};
};