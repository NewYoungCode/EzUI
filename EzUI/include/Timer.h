#pragma once
#include "EzUI.h"
namespace EzUI {
	namespace Thread {
		class Timer //另开线程做Timer 不建议处理UI相关的操作 请自行控制线程同步
		{
		private:
			bool brun = false;
			std::thread* task = NULL;
			void taskFunc() {
				while (brun)
				{
					if (Tick) {
						Tick();
					}
					Sleep(Interval);
				}
			}
		public:
			std::function<void()> Tick;
			size_t Interval = -1;
		public:
			Timer() {}
			void Start() {
				if (task) {
					brun = true;
					return;
				}
				if (Tick) {
					brun = true;
					task = new std::thread(&Timer::taskFunc, this);
				}
			}
			void Stop() {
				if (task) {
					brun = false;
					task->join();
					delete task;
					task = NULL;
				}
			}
			virtual ~Timer() {
				Stop();
			}
		};
	}
	namespace Windows {
		static std::map<UINT_PTR, UINT_PTR> _timers;
		static std::mutex _timerMtx;
		class Timer {//在操作UI的时候建议使用此Timer类
		private:
			static void InsertTimer(UINT_PTR TimerId, UINT_PTR timer) {
				_timerMtx.lock();
				_timers.insert(std::pair<UINT_PTR, UINT_PTR>(TimerId, timer));
				_timerMtx.unlock();
			};
			static void Erase(UINT_PTR iTimerID) {
				_timerMtx.lock();
				_timers.erase(iTimerID);
				_timerMtx.unlock();
			};
		public:
			std::function<void(Timer*)> Tick;
			size_t Interval = -1;
			UINT_PTR TimerId = 0;
			bool started = false;
			EString Name;
		private:
			static void  CALLBACK TimeProc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
			{
				_timerMtx.lock();
				Timer* timer = (Timer*)_timers[iTimerID];
				if (timer && timer->Tick) {
					timer->Tick(timer);
				}
				_timerMtx.unlock();
			}
		public:
			void Stop() {
				if (TimerId) {
					auto ret = ::KillTimer(NULL, TimerId);
					//_timers.erase(TimerId);
					Erase(TimerId);
					TimerId = NULL;
					started = false;
				}
			}
			void Start() {
				if (started) {
					return;
				}
				Stop();

				TimerId = ::SetTimer(NULL, TimerId, Interval, Timer::TimeProc);
				//_timers.insert(std::pair<UINT_PTR, UINT_PTR>(TimerId, (UINT_PTR)this));
				InsertTimer(TimerId, (UINT_PTR)this);
				started = true;
			}
			Timer() {
			}
			virtual ~Timer() {
				Stop();
			}
		};

	};
	using Timer = Thread::Timer;
};