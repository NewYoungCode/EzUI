#pragma once
#include "EzUI.h"
namespace EzUI {
	namespace Thread {
		class UI_EXPORT Timer //另开线程做Timer 不建议处理UI相关的操作 请自行控制线程同步
		{
		private:
			bool brun = false;
			std::thread* task = NULL;
			void taskFunc();
		public:
			std::function<void(Timer*)> Tick;
			size_t Interval = -1;
		public:
			Timer() {}
			void Start();
			void Stop();
			virtual ~Timer();
		};
	}
	namespace Windows {
		class UI_EXPORT Timer {//在操作UI的时候建议使用此Timer类
		public:
			std::function<void(Timer*)> Tick;
			size_t Interval = -1;
			UINT_PTR TimerId =NULL;
		public:
			void Stop();
			void Start();
			Timer() {}
			virtual ~Timer();
		};
	};
	using Timer = Thread::Timer;
};