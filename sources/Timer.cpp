#include "Timer.h"
namespace ezui {
	Timer::Timer(Object* ownerObject) :Object(ownerObject)
	{
	}
	bool Timer::IsStopped() {
		return m_bPause.load();
	}
	void Timer::Start() {
		if (m_task == NULL) {
			m_event = CreateEvent(NULL, FALSE, FALSE, NULL);  // 创建事件对象
			m_bExit = false;
			m_task = new Task([this]() {
				//等待信号与条件的lambda函数
				const auto wait = [this]() {
					while (!(this->m_bExit || !(this->m_bPause.load()))) {
						WaitForSingleObject(m_event, INFINITE);
					}
					};
				while (true)
				{
					wait();//等待信号与条件

					if (this->m_bExit) {
						break;//退出循环
					}

					//当等待时间过长(超过100毫秒) 将等待时间拆分为片段
					auto start = std::chrono::steady_clock::now();
					if (this->Interval > 100) {
						while (!(this->m_bExit)) {
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
						break;//退出循环
					}

					if (this->Tick) {
						this->Tick(this);
					}
				}
				});
		}
		//通知计时器继续运行
		m_bPause.store(false);
		if (m_event) {
			SetEvent(m_event);  // 设置事件, 唤醒等待的线程
		}
	}
	void Timer::Stop() {
		//暂停计时器的运行
		m_bPause.store(true);
	}
	Timer::~Timer() {
		m_bExit = true;
		if (m_event) {
			SetEvent(m_event);  // 设置事件, 唤醒等待的线程
		}
		if (m_task) {
			delete m_task; //结束掉轮询任务
		}
		if (m_event) {
			CloseHandle(m_event);  // 关闭句柄, 释放资源
		}
	}
};