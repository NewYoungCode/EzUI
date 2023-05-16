#pragma once
#include "EzUI.h"
namespace EzUI {
	namespace Thread {
		class UI_EXPORT Timer //���߳���Timer �����鴦��UI��صĲ��� �����п����߳�ͬ��
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
		class UI_EXPORT Timer {//�ڲ���UI��ʱ����ʹ�ô�Timer��
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