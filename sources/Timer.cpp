#include "Timer.h"
namespace ezui {
	Timer::Timer(Object* parentObject) :Object(parentObject)
	{
	}
	bool Timer::IsStopped() {
		return m_bStop;
	}
	void Timer::Start() {
		if (m_task == NULL) {
			m_task = new Task([this]() {
				while (true)
				{
					Sleep(this->Interval);
					{
						std::unique_lock<std::mutex> autoLock(m_mtx);
						m_condv.wait(autoLock, [this]() {
							return  this->m_bExit || !this->m_bStop;
							});
						if (this->m_bExit) {
							break;
						}
					}
					if (this->Tick) {
						this->Tick(this);
					}
				}
				});
		}
		std::unique_lock<std::mutex> autoLock(m_mtx);
		m_bStop = false;
		m_condv.notify_one();
	}
	void Timer::Stop() {
		std::unique_lock<std::mutex> autoLock(m_mtx);
		m_bStop = true;
		m_condv.notify_one();
	}
	Timer::~Timer() {
		{
			std::unique_lock<std::mutex> autoLock(m_mtx);
			m_bExit = true;
			m_condv.notify_one();
		}
		if (m_task) {
			delete m_task;
		}
	}
};