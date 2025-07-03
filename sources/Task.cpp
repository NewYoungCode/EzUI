#include "Task.h"
namespace ezui {
	void Task::Wait() {
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
	bool Task::IsStopped() {
		std::unique_lock<std::mutex> autoLock(_mtx);
		return _bStop;
	}
	Task::~Task() {
		Wait();
		delete _task;
	}

	TaskFactory::TaskFactory(int maxTaskCount) {
		for (size_t i = 0; i < maxTaskCount; ++i)
		{
			tasks.push_back(new Task([this]() {
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

	void TaskFactory::WaitAll() {
		codv.notify_all();
		std::unique_lock<std::mutex> lock2(this->mtx2);
		this->codv2.wait(lock2, [this]()->bool {
			return funcs.empty();
			});
	}
	TaskFactory::~TaskFactory() {
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
};
