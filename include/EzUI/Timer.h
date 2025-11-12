#pragma once
#include "EzUI.h"
#include "Task.h"

namespace ezui {

	//精度稍高的倒数计时器
	class UI_EXPORT WaitableTimer {
		HANDLE m_hTimer = NULL;
	public:
		WaitableTimer();
		void Wait(int milliseconds);
		virtual ~WaitableTimer();
	};

	//使用线程的计时器 不与主进程同步(启动的时候就直接开始执行回调函数)
	class UI_EXPORT Timer :public Object {
		bool m_bExit = false;
		bool m_bPause = true;
		bool m_clockMode = false;
		Task* m_task = NULL;
		mutex* m_mtx = NULL;
		condition_variable* m_condv = NULL;
		WaitableTimer* m_waitableTimer = NULL;
	public:
		std::function<void(Timer*)> Tick = NULL;
		int Interval = 0;
	public:
		//Timeout干啥的不用我多说什么了吧
		template<class Func, class... Args>
		static void Timeout(int msec, Func&& f, Args&& ...args) {
			std::function<void()>* func = new std::function<void()>(std::bind(f, args...));
			Timer* timer = new Timer;
			timer->Interval = 0;
			timer->Tick = [msec, func](Timer* t) {
				t->Stop();
				Sleep(msec);
				(*func)();
				delete func;
				t->DeleteLater();
				};
			timer->Start();
		};
	public:
		Timer(Object* ownerObject = NULL);
		//是否采用时钟模式(启用之后,执行回调函数会较为频繁)
		void SetClockMode(bool bEnable = true);
		//负责等待
		void Wait(int milliseconds);
		bool IsStopped();
		bool IsRunning();
		void Start();
		void Stop();
		virtual ~Timer();
	};
};