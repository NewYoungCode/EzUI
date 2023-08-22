#pragma once
#include "Timer.h"
#pragma comment(lib,"Winmm.lib")
namespace EzUI {
	namespace Thread {
		Timer::Timer() {}
		void Timer::Start() {
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
		void Timer::Stop() {
			if (timer) {
				MMRESULT ret = ::timeKillEvent(timer);
				ASSERT(!ret);
				timer = NULL;
			}
		}
		Timer:: ~Timer() {
			Stop();
		}
	}
	namespace Windows {
		std::map<UINT_PTR, UINT_PTR>  __timers;
		std::mutex __timerMtx;
		void __InsertTimer(UINT_PTR TimerId, UINT_PTR timer) {
			__timerMtx.lock();
			__timers.insert(std::pair<UINT_PTR, UINT_PTR>(TimerId, timer));
			__timerMtx.unlock();
		};
		void __Erase(UINT_PTR iTimerID) {
			__timerMtx.lock();
			__timers.erase(iTimerID);
			__timerMtx.unlock();
		};
		void  CALLBACK __TimeProc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
		{
			Timer* timer = (Timer*)__timers[iTimerID];
			if (timer && timer->Tick) {
				timer->Tick(timer);
			}
		}
		void Timer::Stop() {
			if (TimerId) {
				auto ret = ::KillTimer(NULL, TimerId);
				__Erase(TimerId);
				TimerId = NULL;
			}
		}
		void Timer::Start() {
			if (TimerId != NULL) {
				return;
			}
			if (Interval < USER_TIMER_MINIMUM)Interval = USER_TIMER_MINIMUM;
			if (Interval > USER_TIMER_MAXIMUM)Interval = USER_TIMER_MAXIMUM;
			TimerId = ::SetTimer(NULL, TimerId, Interval, Windows::__TimeProc);
			ASSERT(TimerId);
			__InsertTimer(TimerId, (UINT_PTR)this);
		}
		Timer::~Timer() {
			Stop();
		}
	}
};