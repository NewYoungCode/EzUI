#include "Timer.h"
namespace ezui {
	Timer::Timer() {
		_task = new Task([this]() {
			while (true)
			{
				{
					std::unique_lock<std::mutex> autoLock(_mtx);
					_condv.wait(autoLock, [this]() {
						return  this->_bExit || !this->_bStop;
						});
					if (this->_bExit) {
						break;
					}
				}
				if (this->Tick) {
					this->Tick(this);
				}
				Sleep(this->Interval);
			}
			});
	}
	bool Timer::IsStopped() {
		return _bStop;
	}
	void Timer::Start() {
		std::unique_lock<std::mutex> autoLock(_mtx);
		_bStop = false;
		_condv.notify_one();
	}
	void Timer::Stop() {
		std::unique_lock<std::mutex> autoLock(_mtx);
		_bStop = true;
		_condv.notify_one();
	}
	Timer::~Timer() {
		{
			std::unique_lock<std::mutex> autoLock(_mtx);
			this->Tick = NULL;
			_bExit = true;
			_condv.notify_one();
		}
		delete _task;
	}
};