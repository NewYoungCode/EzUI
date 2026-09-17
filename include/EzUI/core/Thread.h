#pragma once
#include "UIDef.h"
#include "core/UICore.h"
#include "MessageQueue.h"
namespace ezui {
	//线程类
	class EZUI_API Thread EZUI_FINAL {
		atomic<bool> m_finished;
		once_flag m_joinOnce;
		thread* m_thread;
	private:
		Thread(const Thread&);            // 禁止拷贝构造
		Thread& operator=(const Thread&); // 禁止拷贝赋值
		Thread(Thread&&);                 // 禁止移动构造
		Thread& operator=(Thread&&);      // 禁止移动赋值
		void DoWork(const std::function<void()>& func);
	public:
#ifdef EZUI_COMPILER_VS2010
		template<class Func>
		Thread(Func&& f)
			: m_finished(false), m_joinOnce(), m_thread(NULL)
		{
			auto func = std::make_shared<std::function<void()>>(std::bind(std::forward<Func>(f)));
			m_thread = new thread([this, func]() {
				DoWork(*func); // 执行
				});
		}
#else
		template<class Func, class... Args>
		Thread(Func&& f, Args&& ...args)
			: m_finished(false), m_joinOnce(), m_thread(NULL) {
			auto func = std::make_shared<std::function<void()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			m_thread = new thread([this, func]() {
				DoWork(*func);//执行
				});
		}
#endif

		void Wait();
		//当前线程中的任务是否已经执行完成
		bool IsFinished();
		~Thread();
	};

};

namespace ezui {
	//线程池
	class EZUI_API ThreadPool {
		bool m_bStop;
		ezui::atomic<bool> m_poolInitialized;
		size_t m_activeTaskCount;
		std::list<Thread*> m_tasks;
		std::list<std::function<void()>> m_funcs;
		mutex m_mtx;
		condition_variable m_codv;
		//用于等待任务队列清空和执行中的任务完成
		condition_variable m_codv2;
		int m_maxTaskCount;
	private:
		ThreadPool(const ThreadPool&);            // 禁止拷贝构造
		ThreadPool& operator=(const ThreadPool&); // 禁止拷贝赋值
		ThreadPool(ThreadPool&&);                 // 禁止移动构造
		ThreadPool& operator=(ThreadPool&&);      // 禁止移动赋值
		void CreatePool();
		bool HasQueuedTaskOrStopped() const;
		bool IsQueueIdle() const;
	public:
		ThreadPool(int maxTaskCount = 10);

#ifdef EZUI_COMPILER_VS2010
		//添加到任务队列中的末尾(先后顺序执行)
		template<class Func>
		void Add(Func&& f) {
			{
				unique_lock<mutex> autoLock(m_mtx);
				if (m_bStop) {
					return;
				}
				m_funcs.push_back(std::forward<Func>(f));
				CreatePool();
			}
			m_codv.notify_one();
		}
		//添加至任务队列的第一位(优先执行)
		template<class Func>
		void AddToFirst(Func&& f) {
			{
				unique_lock<mutex> autoLock(m_mtx);
				if (m_bStop) {
					return;
				}
				m_funcs.push_front(std::forward<Func>(f));
				CreatePool();
			}
			m_codv.notify_one();
		}
#else
		//添加到任务队列中的末尾(先后顺序执行)
		template<class Func, class... Args>
		void Add(Func&& f, Args&& ...args) {
			{
				unique_lock<mutex> autoLock(m_mtx);
				if (m_bStop) {
					return;
				}
				m_funcs.push_back(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
				CreatePool();
			}
			m_codv.notify_one();
		}
		//添加至任务队列的第一位(优先执行)
		template<class Func, class... Args>
		void AddToFirst(Func&& f, Args&& ...args) {
			{
				unique_lock<mutex> autoLock(m_mtx);
				if (m_bStop) {
					return;
				}
				m_funcs.push_front(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
				CreatePool();
			}
			m_codv.notify_one();
		}
#endif

		//等待任务队列清空并且执行中的任务完成
		void WaitQueueEmpty();

		virtual ~ThreadPool();
	};
};
