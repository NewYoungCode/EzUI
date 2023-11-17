#pragma once
#include "EzUI.h"
#include "Task.h"
namespace EzUI {

	//使用线程的计时器 不与主进程同步(启动的时候就直接开始执行回调函数)
	class UI_EXPORT ThreadTimer :public IControl {
		bool bExit = false;
		bool bStop = true;
		Task* task = NULL;
		std::mutex mtx;
		std::condition_variable condv;
	public:
		std::function<void(ThreadTimer*)> Tick = NULL;
		size_t Interval = -1;
	public:
		ThreadTimer();
		void Start();
		void Stop();
		virtual ~ThreadTimer();
	};

	//在操作UI的时候建议使用此Timer类 与主线程同步(启动的时候第一次会间隔才会启动)
	class UI_EXPORT Timer :public IControl {
	public:
		std::function<void(Timer*)> Tick = NULL;
		size_t Interval = -1;
		UINT_PTR TimerId = NULL;
	public:
		void Stop();
		void Start();
		Timer() {}
		virtual ~Timer();
	};

};