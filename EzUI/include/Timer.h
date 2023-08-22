#pragma once
#include "EzUI.h"
namespace EzUI {
	namespace Thread {
		class UI_EXPORT Timer {
			MMRESULT timer = 0;
		public:
			std::function<void(Timer*)> Tick;
			size_t Interval = -1;
		public:
			Timer();
			void Start();
			void Stop();
			virtual ~Timer();
		};
	}
	namespace Windows {
		//在操作UI的时候建议使用此Timer类
		class UI_EXPORT Timer {
		public:
			std::function<void(Timer*)> Tick;
			size_t Interval = -1;
			UINT_PTR TimerId = NULL;
		public:
			void Stop();
			void Start();
			Timer() {}
			virtual ~Timer();
		};
	};
	using Timer = Thread::Timer;
};