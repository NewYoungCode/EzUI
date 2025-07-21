#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <future>
#include <list>

class Thread {
	bool _bStop = false;
	std::thread* _task = NULL;
	bool _bJoin = false;
	std::mutex _mtx;
	std::condition_variable _codv;
private:
	Thread(const Thread&) = delete;
public:
	template<class Func, class... Args>
	Thread(Func&& f, Args&& ...args) {
		std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		_task = new std::thread([this, func]() mutable {
			(*func)();
			delete func;
			{
				std::unique_lock<std::mutex> autoLock(_mtx);
				_bStop = true;
			}
			_codv.notify_all();
			});
	}
	void Wait() {
		if (!_bJoin)
		{
			_bJoin = true;
			std::unique_lock<std::mutex> autoLock(_mtx);
			_codv.wait(autoLock, [this]() ->bool {
				return _bStop;
				});
			_task->join();
		}
	}
	bool IsStopped() {
		std::unique_lock<std::mutex> autoLock(_mtx);
		return _bStop;
	}
	virtual ~Thread() {
		Wait();
		delete _task;
	}
};

class ThreadPool {
	bool bStop = false;
	std::list<Thread*> tasks;
	std::list<std::function<void()>> funcs;
	std::mutex mtx;
	std::condition_variable codv;
	//用于等待任务清空的锁和条件变量
	std::mutex mtx2;
	std::condition_variable codv2;
private:
	ThreadPool(const ThreadPool&) = delete;
public:
	ThreadPool(int maxTaskCount = 50) {
		for (size_t i = 0; i < maxTaskCount; ++i)
		{
			tasks.push_back(new Thread([this]() {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> autoLock(this->mtx);
						this->codv.wait(autoLock, [this]()->bool {
							return this->bStop || !this->funcs.empty();
							});
						if (funcs.empty()) {
							this->codv2.notify_all();
						}
						if (this->bStop && funcs.empty()) {
							break;
						}
						task = std::move(*funcs.begin());
						funcs.pop_front();
					}
					task();
				}
				}));
		}
	}
	void WaitAll() {
		codv.notify_all();
		std::unique_lock<std::mutex> lock2(this->mtx2);
		this->codv2.wait(lock2, [this]()->bool {
			return funcs.empty();
			});
	}
	virtual ~ThreadPool() {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			bStop = true;
		}
		WaitAll();
		while (!tasks.empty())
		{
			for (auto itor = tasks.begin(); itor != tasks.end(); )
			{
				if ((*itor)->IsStopped()) {
					(*itor)->Wait();
					delete (*itor);
					itor = tasks.erase(itor);
				}
				else {
					++itor;
				}
			}
		}
	}
	//添加到任务队列中的末尾(先后顺序执行)
	template<class Func, class... Args>
	void Add(Func&& f, Args&& ...args) {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			funcs.emplace_back(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		}
		codv.notify_one();
	}
	//添加至任务队列的第一位(优先执行)
	template<class Func, class... Args>
	void AddToFrist(Func&& f, Args&& ...args) {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			funcs.emplace_front(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
		}
		codv.notify_one();
	}
};
