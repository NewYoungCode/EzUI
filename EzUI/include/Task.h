#pragma once
#include "EzUI.h"
#include <condition_variable>
namespace EzUI {
	class Task {
		bool bStop = false;
		std::thread* task = NULL;
		bool bJoin = false;
		std::mutex mtx;
		std::condition_variable codv;
	private:
		Task(const Task&) = delete;
	public:
		template<class Func, class... Args>
		Task(Func&& f, Args&& ...args) {
			task = new std::thread([=]() mutable {
				std::invoke(std::forward<Func>(f), std::forward<Args>(args)...);
				{
					std::unique_lock<std::mutex> autoLock(mtx);
					bStop = true;
				}
				codv.notify_one();
				});
		}
		void Wait();
		//��ǰ�����Ƿ��Ѿ�ֹͣ
		bool IsStopped();
		virtual ~Task();
	};

	class TaskFactory {
		bool bStop = false;
		std::list<Task*> tasks;
		std::list<std::function<void()>> funcs;
		std::mutex mtx;
		std::condition_variable codv;
		//���ڵȴ�������յ�������������
		std::mutex mtx2;
		std::condition_variable codv2;
	private:
		TaskFactory(const TaskFactory&) = delete;
	public:
		TaskFactory(int maxTaskCount = 50);
		template<class Func, class... Args>
		void Add(Func&& f, Args&& ...args) {
			{
				std::unique_lock<std::mutex> autoLock(mtx);
				std::function<void()> func(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
				funcs.push_back(func);
			}
			codv.notify_one();
		}
		//�ȴ������������
		void WaitAll();
		virtual ~TaskFactory();
	};

};
