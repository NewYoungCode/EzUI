#include "Task.h"
namespace EzUI {
	void Task::Wait() {
		if (!bJoin)
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			codv.wait(autoLock, [this]() ->bool {
				return bStop;
				});
			task->join();
			bJoin = true;
		}
	}
	bool Task::IsStopped() {
		std::unique_lock<std::mutex> autoLock(mtx);
		return bStop;
	}
	Task::~Task() {
		Wait();
		delete task;
	}

	TaskFactory::TaskFactory(int maxTaskCount) {
		for (size_t i = 0; i < maxTaskCount; ++i)
		{
			tasks.push_back(new Task([this]() {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(this->mtx);
						this->codv.wait(lock, [this]()->bool {
							return this->bStop || !this->funcs.empty();
							});
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

	void TaskFactory::Clear() {
		codv.notify_all();
		while (tasks.size() > 0)
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
	TaskFactory::~TaskFactory() {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			bStop = true;
		}
		Clear();
	}
};
