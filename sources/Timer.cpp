#include "Timer.h"
namespace ezui {
	Timer::Timer(Object* parentObject) :Object(parentObject)
	{
	}
	bool Timer::IsStopped() {
		return m_bPause;
	}
	void Timer::Start() {
		if (m_task == NULL) {
			m_bExit = false;
			m_task = new Task([this]() {
				while (true)
				{
					{
						m_condv.Wait([this]() {
							return this->m_bExit || !this->m_bPause;
							});
						m_condv.Lock();
						if (this->m_bExit) {
							m_condv.Unlock();
							break;
						}
						m_condv.Unlock();
					}

					//当等待时间过长(超过100毫秒) 将等待时间拆分为片段
					auto start = std::chrono::steady_clock::now();
					if (this->Interval > 100) {
						while (!this->m_bExit) {
							auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
							if (elapsed >= this->Interval) {
								break;
							}
							DWORD remain = (DWORD)(this->Interval - elapsed);
							Sleep(remain > 100 ? 100 : remain);
						}
					}
					else {
						//等待时间过小 直接sleep
						Sleep(this->Interval);
					}

					if (this->m_bExit) {
						break;
					}

					if (this->Tick) {
						this->Tick(this);
					}
				}
				});
		}
		{
			m_condv.Lock();
			m_bPause = false;
			m_condv.Unlock();
		}
		m_condv.Notify();
	}
	void Timer::Stop() {
		{
			m_condv.Lock();
			m_bPause = true;//暂停
			m_condv.Unlock();
		}
		m_condv.Notify();
	}
	Timer::~Timer() {
		m_bExit = true;
		m_condv.Notify();
		if (m_task) {
			delete m_task;
		}
	}
};