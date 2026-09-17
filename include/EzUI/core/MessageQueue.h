#pragma once
#include "UIDef.h"
#include "core/UICore.h"
namespace ezui {
	//消息队列，提供线程安全的消息发布和消费机制
	class EZUI_API MessageQueue
	{
	private:
		std::queue<std::function<void()>> m_queue;
		mutex                        m_mutex;
		condition_variable           m_cv;
		bool                              m_stop;
	protected:
		//进入队列
		virtual bool Queue(std::function<void()> task);
		//消费消息,等待并获取队列中的任务，如果队列为空则阻塞等待，直到有任务可用或队列被停止
		bool Pop(std::function<void()>& task);
	public:
		MessageQueue();
		virtual ~MessageQueue();

#ifdef EZUI_COMPILER_VS2010
		//发布消息，接受一个可调用对象和其参数，并将其封装为一个任务放入队列中
		template<class Func>
		bool Post(Func&& f)
		{
			return Queue(std::function<void()>(std::forward<Func>(f)));
		}
#else
		//发布消息，接受一个可调用对象和其参数，并将其封装为一个任务放入队列中
		template<class Func, class... Args>
		bool Post(Func&& f, Args&&... args)
		{
			return Queue(std::function<void()>(std::bind(std::forward<Func>(f), std::forward<Args>(args)...)));
		}
#endif
		
		//停止消息队列，设置停止标志并通知所有等待的线程，使其能够退出
		virtual void Quit();
		//执行消息队列中的任务，持续从队列中获取并执行任务，直到队列被停止
		virtual void Exec();
	};
};
