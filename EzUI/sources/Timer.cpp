#pragma once
#include "Timer.h"
namespace EzUI {
	namespace Thread {
		void Timer::taskFunc() {
			while (brun)
			{
				if (Tick) {
					Tick(this);
				}
				Sleep(Interval);
			}
		}
		void Timer::Start() {
			if (task) {
				brun = true;
				return;
			}
			if (Tick) {
				brun = true;
				task = new std::thread(&Timer::taskFunc, this);
			}
		}
		void Timer::Stop() {
			if (task) {
				brun = false;
				task->join();
				delete task;
				task = NULL;
			}
		}
		Timer::~Timer() {
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
			if (TimerId!=NULL) {
				return;
			}
			TimerId = ::SetTimer(NULL, TimerId, Interval, Windows::__TimeProc);
			__InsertTimer(TimerId, (UINT_PTR)this);
		}
		Timer::~Timer() {
			Stop();
		}
	}


};