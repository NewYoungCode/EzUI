#include "Task.h"
namespace ezui {

	Mutex::Mutex() {
		InitializeCriticalSection(&m_mtx);  // 初始化互斥锁
		m_bLocked = false;
	}
	Mutex::~Mutex() {
		DeleteCriticalSection(&m_mtx);  // 销毁互斥锁
	}
	void Mutex::Lock() {
		EnterCriticalSection(&m_mtx);
		m_bLocked = true;
	}
	void Mutex::UnLock() {
		LeaveCriticalSection(&m_mtx);
		m_bLocked = false;
	}

	ConditionVariable::ConditionVariable() {
		m_codv = CreateEvent(NULL, FALSE, FALSE, NULL);  // 创建事件对象
	}
	ConditionVariable::~ConditionVariable() {
		if (m_codv != NULL) {
			CloseHandle(m_codv);  // 关闭句柄，释放资源
		}
	}
	void ConditionVariable::Notify() {
		SetEvent(m_codv);  // 设置事件，唤醒等待的线程
	}
	void ConditionVariable::Wait(const std::function<bool()>& condition_cb) {
		m_mtx.Lock();// 先加锁
		// 如果没有条件回调的情况下
		if (!condition_cb) {
			m_mtx.UnLock();  // 必须先解锁再等待！
			WaitForSingleObject(m_codv, INFINITE);
			return;  //事件已触发 无需再检查 等待信号之前已经解锁 所以无需解锁 直接return
		}
		// 如果有条件回调 循环检查
		while (!condition_cb()) {
			m_mtx.UnLock();  // 解锁后再等待，避免死锁
			WaitForSingleObject(m_codv, INFINITE);
			m_mtx.Lock();    // 重新加锁检查条件
		}
		m_mtx.UnLock();  // 最终解锁
	}
	void ConditionVariable::Lock() {
		m_mtx.Lock();
	}
	void ConditionVariable::Unlock() {
		m_mtx.UnLock();
	}
}

namespace ezui {
	void Task::Wait() {
		if (!m_bJoin)
		{
			m_bJoin = true;
			// 直接调用 Wait() 会自动处理锁定和解锁
			m_mtx_codv.Wait([this]() -> bool {
				return m_finished; // 条件满足时退出等待
				});
			m_thread->join();  // 等待线程完成
		}
	}
	bool Task::IsStopped() {
		return m_finished;
	}
	void Task::DoWork(std::function<void()>* func) {
		(*func)();  // 执行任务
		delete func;
		{
			// 完成任务后更新状态
			m_mtx_codv.Lock();
			m_finished = true;
			m_mtx_codv.Unlock();
		}
		m_mtx_codv.Notify();  // 通知等待的线程
	}

	Task::~Task() {
		Wait();  // 等待任务完成
		delete m_thread;  // 删除线程对象
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
