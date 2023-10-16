#pragma once
#include "Timer.h"
#pragma comment(lib,"Winmm.lib")
namespace EzUI {
	ThreadTimer::ThreadTimer() {}
	void ThreadTimer::Start() {
		if (timer != NULL) {
			return;
		}
		timer = ::timeSetEvent(this->Interval, 0, [](UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2) {
			Timer* t = (Timer*)dwUser;
			if (t->Tick) {
				t->Tick(t);
			}
			}, (DWORD_PTR)this, TIME_PERIODIC);
		ASSERT(timer);
	}
	void ThreadTimer::Stop() {
		if (timer) {
			MMRESULT ret = ::timeKillEvent(timer);
			ASSERT(!ret);
			timer = NULL;
		}
	}
	ThreadTimer:: ~ThreadTimer() {
		Stop();
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