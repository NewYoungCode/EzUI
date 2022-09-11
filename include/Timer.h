#pragma once
#include <functional>
#include <thread>
#include <Windows.h>
namespace EzUI {

	class Timer
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
};