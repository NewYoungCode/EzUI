#pragma once
#include "EzUI.h"
#include "Task.h"
namespace ezui {

	//使用线程的计时器 不与主进程同步(启动的时候就直接开始执行回调函数)
	class UI_EXPORT Timer :public Object {
		bool m_bExit = false;
		bool m_bPause = true;
		Task* m_task = NULL;
		ConditionVariable m_condv;
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
		Timer(Object* parentObject = NULL);
		bool IsStopped();
		void Start();
		void Stop();
		virtual ~Timer();
	};
};