#pragma once
#include "EzUI.h"
namespace EzUI {
	class Task {
		bool bStop = false;
		std::thread* task = NULL;
		bool bJoin = false;
	private:
		Task(const Task&) = delete;
	public:
		template<class F, class... Args>
		Task(F&& f, Args&& ...args) {
			task = new std::thread([=]() mutable {
				std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
				bStop = true;
				});
		}
		void Wait() {
			while (!bJoin)
			{
				if (IsStop()) {
					task->join();
					bJoin = true;
					break;
				}
			}
		}
		bool IsStop() {
			return bStop;
		}
		virtual ~Task() {
			Wait();
			delete task;
		}
	};

	class TaskFactory {
		int maxCount = 0;
		std::list<Task*> task;
	private:
		TaskFactory(const TaskFactory&) = delete;
	public:
		TaskFactory(int maxTaskCount = 50) {
			this->maxCount = maxTaskCount;
		}
		template<class F, class... Args>
		bool Run(F&& f, Args&& ...args) {
			int miniSize = maxCount / 2;
			for (auto itor = task.begin(); itor != task.end();)
			{
				Task* thread = *itor;
				if (thread->IsStop()) {
					thread->Wait();
					delete thread;
					itor = task.erase(itor);
				}
				else {
					itor++;
				}
				if (task.size() <= miniSize) {
					break;
				}
			}
			if (task.size() >= maxCount) {
				return false;
			}
			Task* thread = new Task(std::forward<F>(f), std::forward<Args>(args)...);
			task.push_back(thread);
			return true;
		}
		
		//等待已有任务全部完成
		void Wait() {
			for (auto& it : task) {
				it->Wait();
			}
		}
		void Clear() {
			std::list<Task*>::iterator itor = task.begin();
			while (task.size() > 0)
			{
				if (itor == task.end()) {
					itor = task.begin();
				}
				if ((*itor)->IsStop()) {
					(*itor)->Wait();
					delete (*itor);
					itor = task.erase(itor);
				}
				else {
					itor++;
				}
			}
		}
		virtual ~TaskFactory() {
			Clear();
		}
	};
};
