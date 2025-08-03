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
					{
						m_condv.Wait([this]() {
							return  this->m_bExit || !this->m_bStop;
							});
						m_condv.Lock();
						if (this->m_bExit) {
							m_condv.Unlock();
							break;
						}
						m_condv.Unlock();
					}
					Sleep(this->Interval);
					if (this->Tick) {
						this->Tick(this);
					}
				}
				});
		}
		{
			m_condv.Lock();
			m_bStop = false;
			m_condv.Unlock();
		}
		m_condv.Notify();
	}
	void Timer::Stop() {
		{
			m_condv.Lock();
			m_bStop = true;
			m_condv.Unlock();
		}
		m_condv.Notify();
	}
	Timer::~Timer() {
		{
			m_condv.Lock();
			m_bExit = true;
			m_condv.Unlock();
		}
		m_condv.Notify();
		if (m_task) {
			delete m_task;
		}
	}
};