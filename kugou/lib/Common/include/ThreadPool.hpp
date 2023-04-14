#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <future>
class ThreadPool
{
private:
	// need to keep track of threads so we can join them
	std::vector< std::thread > m_workers;
	// the task queue
	std::queue< std::function<void()> > m_tasks;
	// synchronization
	std::mutex m_queue_mutex;
	std::condition_variable m_condition;
	bool stop = false;
	size_t MaxtTreadCount = 10;
public:
	// the constructor just launches some amount of workers
	inline ThreadPool(size_t threads)
	{
		size_t pools = m_workers.size();
		for (; pools < MaxtTreadCount && threads > 0; --threads)
		{
			m_workers.emplace_back([this]() ->void {
				while (true)
				{
					std::function<void()> task;
					{

						std::unique_lock<std::mutex> lock(this->m_queue_mutex);

						this->m_condition.wait(lock, [this]()->bool {
							return this->stop || !this->m_tasks.empty();
						});

						if (this->stop && this->m_tasks.empty()) {
							return;
						}
						task = std::move(this->m_tasks.front());
						this->m_tasks.pop();
					}
					task();
				}
			});
		}
	}

	// add new work item to the pool
	template<class F, class... Args>
	inline auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();
		{

			std::unique_lock<std::mutex> lock(m_queue_mutex);

			// don't allow enqueueing after stopping the pool
			if (stop) {
				throw std::runtime_error("enqueue on stopped ThreadPool");
			}

			m_tasks.emplace([task]() {
				(*task)();
			});
		}
		m_condition.notify_one();
		return res;
	}

	inline ~ThreadPool()
	{
		m_queue_mutex.lock();
		stop = true;
		m_queue_mutex.unlock();

		m_condition.notify_all();
		for (std::thread& worker : m_workers) {
			worker.join();
		}
	}

};