#include "Timer.h"
namespace EzUI {
	ThreadTimer::ThreadTimer() {
		_task = new Task([this]() {
			while (true)
			{
				{
					std::unique_lock<std::mutex> autoLock(_mtx);
					_condv.wait(autoLock, [this]() {
						return  this->_bExit || !this->_bStop;
						});
					if (this->_bExit) {
						break;
					}
				}
				if (this->Tick) {
					this->Tick(this);
				}
				Sleep(this->Interval);
			}
			});
	}
	bool ThreadTimer::IsStopped() {
		return _bStop;
	}
	void ThreadTimer::Start() {
		std::unique_lock<std::mutex> autoLock(_mtx);
		_bStop = false;
		_condv.notify_one();
	}
	void ThreadTimer::Stop() {
		std::unique_lock<std::mutex> autoLock(_mtx);
		_bStop = true;
		_condv.notify_one();
	}
	ThreadTimer::~ThreadTimer() {
		{
			std::unique_lock<std::mutex> autoLock(_mtx);
			_bExit = true;
			_condv.notify_one();
		}
		delete _task;
	}

	std::map<UINT_PTR, UINT_PTR>  __EzUI__Timers;
	void __EzUI__InsertTimer(UINT_PTR TimerId, UINT_PTR timer) {
		__EzUI__Timers.insert(std::pair<UINT_PTR, UINT_PTR>(TimerId, timer));
	};
	void __EzUI__Erase(UINT_PTR iTimerID) {
		__EzUI__Timers.erase(iTimerID);
	};
	void  CALLBACK __EzUI__TimeProc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
	{
		Timer* timer = (Timer*)__EzUI__Timers[iTimerID];
		if (timer && timer->Tick) {
			timer->Tick(timer);
		}
	}
	void Timer::Stop() {
		if (TimerId) {
			auto ret = ::KillTimer(NULL, TimerId);
			__EzUI__Erase(TimerId);
			TimerId = NULL;
		}
	}
	void Timer::Start() {
		if (TimerId != NULL) {
			return;
		}
		if (Interval < USER_TIMER_MINIMUM)Interval = USER_TIMER_MINIMUM;
		if (Interval > USER_TIMER_MAXIMUM)Interval = USER_TIMER_MAXIMUM;
		TimerId = ::SetTimer(NULL, TimerId, Interval, __EzUI__TimeProc);
		ASSERT(TimerId);
		__EzUI__InsertTimer(TimerId, (UINT_PTR)this);
	}
	Timer::~Timer() {
		Stop();
	}
};