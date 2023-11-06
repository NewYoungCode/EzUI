#pragma once
#include "EzUI.h"
#include <condition_variable>
namespace EzUI {
	class UI_EXPORT Task {
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
			std::function<void()>* func = new std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			task = new std::thread([this, func]() mutable {
				(*func)();
				delete func;
				{
					std::unique_lock<std::mutex> autoLock(mtx);
					bStop = true;
				}
				codv.notify_all();
				});
		}
		void Wait();
		//��ǰ�����Ƿ��Ѿ�ֹͣ
		bool IsStopped();
		virtual ~Task();
	};

	class UI_EXPORT TaskFactory {
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
		//��ӵ���������е�ĩβ(�Ⱥ�˳��ִ��)
		template<class Func, class... Args>
		void Add(Func&& f, Args&& ...args) {
			{
				std::unique_lock<std::mutex> autoLock(mtx);
				funcs.emplace_back(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			}
			codv.notify_one();
		}
		//�����������еĵ�һλ(����ִ��)
		template<class Func, class... Args>
		void AddToFrist(Func&& f, Args&& ...args) {
			{
				std::unique_lock<std::mutex> autoLock(mtx);
				funcs.emplace_front(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			}
			codv.notify_one();
		}

		//�ȴ������������
		void WaitAll();
		virtual ~TaskFactory();
	};

};
