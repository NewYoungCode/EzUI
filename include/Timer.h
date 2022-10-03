#pragma once
#include <functional>
#include <thread>
#include <Windows.h>
namespace EzUI {
	namespace Thread {
		class Timer //另开线程做Timer
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
		class Timer {//基于主线程Timer
		public:
			std::function<void()> Tick;
			size_t Interval = -1;
			UINT_PTR TimerId = 0;
			bool started = false;
		private:
			static void  CALLBACK TimeProc(HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
			{
				Timer* timer = (Timer*)_timers[iTimerID];
				if (timer && timer->Tick) {
					timer->Tick();
				}
			}
		public:
			void Stop() {
				if (TimerId) {
					auto ret = ::KillTimer(NULL, TimerId);
					_timers.erase(TimerId);
					TimerId = NULL;
					started = false;
				}
			}
			void Start() {
				if (started) {
					return;
				}
				Stop();
				 TimerId = ::SetTimer(NULL, NULL, Interval, Timer::TimeProc);
				_timers.insert(std::pair<UINT_PTR, UINT_PTR>(TimerId, (UINT_PTR)this));
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