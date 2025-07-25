#pragma once
#include "EzUI.h"
#include <condition_variable>

namespace ezui {
	class UI_EXPORT Task {
		bool m_finished = false;
		std::thread* m_thread = NULL;
		bool m_bJoin = false;
		std::mutex m_mtx;
		std::condition_variable m_codv;
	private:
		Task(const Task&) = delete;
	public:
		template<class Func, class... Args>
		Task(Func&& f, Args&& ...args) {
			std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			m_thread = new std::thread([this, func]() mutable {
				(*func)();
				delete func;
				{
					std::unique_lock<std::mutex> autoLock(m_mtx);
					m_finished = true;
				}
				m_codv.notify_all();
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
