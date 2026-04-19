#pragma once
#include "core/UIDef.h"

namespace ezui {
	namespace detail {
		//线程类
		class EZUI_API Thread final {
			std::atomic<bool> m_finished = false;
			std::once_flag m_joinOnce;
			std::thread* m_thread = NULL;
		private:
			Thread(const Thread&) = delete;            // 禁止拷贝构造
			Thread& operator=(const Thread&) = delete; // 禁止拷贝赋值
			Thread(Thread&&) = delete;                 // 禁止移动构造
			Thread& operator=(Thread&&) = delete;      // 禁止移动赋值
			void DoWork(const std::function<void()>& func);
		public:
			template<class Func, class... Args>
			Thread(Func&& f, Args&& ...args) {
				auto func = std::make_shared<std::function<void()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
				m_thread = new std::thread([this, func]() {
					DoWork(*func);//执行
					});
			}
			void Wait();
			//当前线程中的任务是否已经执行完成
			bool IsFinished();
			~Thread();
		};

		//线程池
		class EZUI_API ThreadPool {
			bool m_bStop = false;
			size_t m_activeTaskCount = 0;
			std::list<Thread*> m_tasks;
			std::list<std::function<void()>> m_funcs;
			std::mutex m_mtx;
			std::condition_variable m_codv;
			//用于等待任务队列清空和执行中的任务完成
			std::condition_variable m_codv2;
		private:
			ThreadPool(const ThreadPool&) = delete;            // 禁止拷贝构造
			ThreadPool& operator=(const ThreadPool&) = delete; // 禁止拷贝赋值
			ThreadPool(ThreadPool&&) = delete;                 // 禁止移动构造
			ThreadPool& operator=(ThreadPool&&) = delete;      // 禁止移动赋值
		public:
			ThreadPool(int maxTaskCount = 10);
			//添加到任务队列中的末尾(先后顺序执行)
			template<class Func, class... Args>
			void Add(Func&& f, Args&& ...args) {
				{
					std::unique_lock<std::mutex> autoLock(m_mtx);
					m_funcs.emplace_back(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
				}
				m_codv.notify_one();
			}
			//添加至任务队列的第一位(优先执行)
			template<class Func, class... Args>
			void AddToFirst(Func&& f, Args&& ...args) {
				{
					std::unique_lock<std::mutex> autoLock(m_mtx);
					m_funcs.emplace_front(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
				}
				m_codv.notify_one();
			}

			//等待任务队列清空并且执行中的任务完成
			void WaitQueueEmpty();

			virtual ~ThreadPool();
		};
	};

	//任务类(线程类)
	using Task = detail::Thread;
	//任务工厂(线程池)
	using TaskFactory = detail::ThreadPool;
};
