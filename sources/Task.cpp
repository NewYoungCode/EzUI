#include "Task.h"
namespace ezui {
	void Task::Wait() {
		if (!m_bJoin)
		{
			m_bJoin = true;
			std::unique_lock<std::mutex> autoLock(m_mtx);
			m_codv.wait(autoLock, [this]() ->bool {
				return m_finished;
				});
			m_thread->join();
		}
	}
	bool Task::IsStopped() {
		std::unique_lock<std::mutex> autoLock(m_mtx);
		return m_finished;
	}

	void Task::DoWork(std::function<void()>* func) {
		(*func)();
		delete func;
		{
			std::unique_lock<std::mutex> autoLock(m_mtx);
			m_finished = true;
		}
		m_codv.notify_all();
	}

	Task::~Task() {
		Wait();
		delete m_thread;
	}

	TaskFactory::TaskFactory(int maxTaskCount) {
		for (size_t i = 0; i < maxTaskCount; ++i)
		{
			m_tasks.push_back(new Task([this]() {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> autoLock(this->m_mtx);
						this->m_codv.wait(autoLock, [this]()->bool {
							return this->m_bStop || !this->m_funcs.empty();
							});
						if (m_funcs.empty()) {
							this->m_codv2.notify_all();
						}
						if (this->m_bStop && m_funcs.empty()) {
							break;
						}
						task = std::move(*m_funcs.begin());
						m_funcs.pop_front();
					}
					task();
				}
				}));
		}
	}

	void TaskFactory::WaitAll() {
		m_codv.notify_all();
		std::unique_lock<std::mutex> lock2(this->m_mtx2);
		this->m_codv2.wait(lock2, [this]()->bool {
			return m_funcs.empty();
			});
	}
	TaskFactory::~TaskFactory() {
		{
			std::unique_lock<std::mutex> autoLock(m_mtx);
			m_bStop = true;
		}
		WaitAll();
		while (!m_tasks.empty())
		{
			for (auto itor = m_tasks.begin(); itor != m_tasks.end(); )
			{
				if ((*itor)->IsStopped()) {
					(*itor)->Wait();
					delete (*itor);
					itor = m_tasks.erase(itor);
				}
				else {
					++itor;
				}
			}
		}
	}
};
