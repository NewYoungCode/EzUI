#pragma once
#include "EzUI.h"
#include <TimeAPI.h>
namespace EzUI {

	//使用window的计时器(精度较高) 不与主进程同步
	class UI_EXPORT ThreadTimer :public IControl {
		MMRESULT timer = 0;
	public:
		std::function<void(ThreadTimer*)> Tick = NULL;
		size_t Interval = -1;
	public:
		ThreadTimer();
		void Start();
		void Stop();
		virtual ~ThreadTimer();
	};

	//在操作UI的时候建议使用此Timer类 与主线程同步
	class UI_EXPORT Timer :public IControl {
	public:
		std::function<void(Timer*)> Tick = NULL;
		size_t Interval = -1;
		UINT_PTR TimerId = NULL;
	public:
		void Stop();
		void Start();
		Timer() {}
		virtual ~Timer();
	};
};