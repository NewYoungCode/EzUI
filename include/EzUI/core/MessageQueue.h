#pragma once
#include "core/UIDef.h"
namespace ezui {
	//消息队列，提供线程安全的消息发布和消费机制
	class EZUI_API MessageQueue
	{
	private:
		std::queue<std::function<void()>> m_queue;
		std::mutex                        m_mutex;
		std::condition_variable           m_cv;
		bool                              m_stop = false;
	public:
		MessageQueue() = default;
		~MessageQueue()
		{
			Quit();
		}

		//发布消息，接受一个可调用对象和其参数，并将其封装为一个任务放入队列中
		template<class Func, class... Args>
		bool Post(Func&& f, Args&& ...args)
		{
			auto task = std::make_shared<std::function<void()>>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (m_stop) {
					return false;
				}

				m_queue.push([task] { (*task)(); });
			}
			m_cv.notify_one();
			return true;
		}
		//消费消息，等待并获取队列中的任务，如果队列为空则阻塞等待，直到有任务可用或队列被停止
		bool Pop(std::function<void()>& task)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_cv.wait(lock, [this] {
				return m_stop || !m_queue.empty();
				});

			if (m_stop) {
				return false;
			}
			task = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}
		//停止消息队列，设置停止标志并通知所有等待的线程，使其能够退出
		void Quit()
		{
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (m_stop) return;
				m_stop = true;
			}
			m_cv.notify_all();
		}
		//执行消息队列中的任务，持续从队列中获取并执行任务，直到队列被停止
		void Exec()
		{
			while (true)
			{
				std::function<void()> task;
				if (!Pop(task)) {
					break;
				}
				task();
			}
		}
	};

};
