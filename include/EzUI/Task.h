#pragma once
#include "EzUI.h"
namespace ezui {
	namespace detail {
		//互斥锁
		class UI_EXPORT mutex {
		private:
			CRITICAL_SECTION m_mtx;  // 互斥锁
			std::atomic<int> m_lockState = 0;
			mutex(const mutex&) = delete;
		public:
			mutex();
			virtual ~mutex();
			// 锁定互斥锁
			void lock();
			// 解锁互斥锁
			void unlock();
			bool isLocked();
		};

		template<typename Mutex>
		class unique_lock {
			Mutex& m_mtx;
		public:
			explicit unique_lock(Mutex& mtx) : m_mtx(mtx) {
				m_mtx.lock();
			}
			void lock() {
				m_mtx.lock();
			}
			void unlock() {
				m_mtx.unlock();
			}
			~unique_lock() {
				unlock();
			}
			// 禁止拷贝
			unique_lock(const unique_lock&) = delete;
			unique_lock& operator=(const unique_lock&) = delete;
			// 支持移动
			unique_lock(unique_lock&& other) noexcept : m_mtx(other.m_mtx) {}
			unique_lock& operator=(unique_lock&&) = delete;
		};

		//带互斥锁的条件变量类
		class UI_EXPORT condition_variable {
		private:
			HANDLE m_codv = NULL;     // 事件对象（模拟条件变量）
			condition_variable(const condition_variable&) = delete;
		public:
			condition_variable();
			virtual ~condition_variable();
			// 唤醒等待的线程(唤醒单个线程)
			void notify_one();
			// 可选条件的等待(不可以多个线程使用此函数)
			void wait(unique_lock<mutex>& lock, const std::function<bool()>& condition_cb);
			//不想多说 懂得都懂
			bool wait_until(unique_lock<mutex>& lock, const std::chrono::steady_clock::time_point& abs_time, const std::function<bool()>& condition_cb);
		};
	};

#if 0
	using condition_variable = std::condition_variable;
	using mutex = std::mutex;
	template<typename T>
	using unique_lock = std::unique_lock<T>;
#else //兼容在高版本工具集编译时候对win7的支持
	using condition_variable = detail::condition_variable;
	using mutex = detail::mutex;
	template<typename T>
	using unique_lock = detail::unique_lock<T>;
#endif // 1

};
namespace ezui {
	class UI_EXPORT Task {
		bool m_finished = false;
		std::thread* m_thread = NULL;
		bool m_bJoin = false;
	private:
		Task(const Task&) = delete;
		void DoWork(std::function<void()>* func);
	public:
		template<class Func, class... Args>
		Task(Func&& f, Args&& ...args) {
			std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			m_thread = new std::thread([this, func]() mutable {
				DoWork(func);
				});
		}
		void Wait();
		//当前任务是否已经停止
		bool IsStopped();
		virtual ~Task();
	};

	class UI_EXPORT TaskFactory {
		bool m_bStop = false;
		std::list<Task*> m_tasks;
		std::list<std::function<void()>> m_funcs;
		std::mutex m_mtx;
		std::condition_variable m_codv;
		//用于等待任务清空的锁和条件变量
		std::mutex m_mtx2;
		std::condition_variable m_codv2;
	private:
		TaskFactory(const TaskFactory&) = delete;
	public:
		TaskFactory(int maxTaskCount = 50);
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
		void AddToFrist(Func&& f, Args&& ...args) {
			{
				std::unique_lock<std::mutex> autoLock(m_mtx);
				m_funcs.emplace_front(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			}
			m_codv.notify_one();
		}

		//等待所有任务被取走
		void WaitAll();
		virtual ~TaskFactory();
	};

};
