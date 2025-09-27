#pragma once
#include "EzUI.h"
#include "Task.h"

namespace ezui {

	//使用线程的计时器 不与主进程同步(启动的时候就直接开始执行回调函数)
	class UI_EXPORT Timer :public Object {
		bool m_bExit = false;
		bool m_bPause = true;
		Task* m_task = NULL;
		mutex m_mtx;
		condition_variable m_condv;
		std::chrono::steady_clock::time_point m_lastTickTime;
		mutex m_mtx2;
		condition_variable m_condv2;
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
	private:
		//负责等待
		void WaitTime();
	public:
		Timer(Object* ownerObject = NULL);
		bool IsStopped();
		void Start();
		void Stop();
		virtual ~Timer();
	};

	//高精度计时器
	class UI_EXPORT TimerClock :public Object {
		std::atomic<bool> m_stop = false;
		std::chrono::steady_clock::time_point m_stopRequestTime;
		UINT m_id = 0;
		static void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);
		void KillTimer();
	public:
		std::function<void(TimerClock*)> Tick = NULL;
		int Interval = 1;
		TimerClock(Object* ownerObject = NULL);
		virtual ~TimerClock();
		bool IsStopped()const;
		void Stop();
		void Start();
	};
};