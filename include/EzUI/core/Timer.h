#pragma once
#include "UICore.h"
#include "Thread.h"

namespace ezui {

	//精度稍高的倒数计时器
	class EZUI_API WaitableTimer EZUI_FINAL {
		HANDLE m_hTimer;
	public:
		WaitableTimer();
		~WaitableTimer();
		void Wait(int milliseconds);
	};

	//使用线程的计时器 不与主进程同步
	class EZUI_API Timer :public Object {
		atomic<bool> m_bReset;
		atomic<bool> m_bExit;
		atomic<bool> m_bStop;
		atomic<bool> m_bImmediate;  // 新增：是否立即执行标志
		bool m_clockMode;
		Thread* m_task;
		mutex* m_mtx;
		mutex m_funcMtx;
		condition_variable* m_condv;
		WaitableTimer* m_waitableTimer;
		std::function<void(Timer*)>* m_tickFunc;
		atomic<int> m_intervalMs;
	private:
		void CreateTask();
		void ExecuteTick();
		bool ShouldWakeTask() const;
		int Wait(int milliseconds);
	public:
#ifdef EZUI_COMPILER_VS2010
		//Timeout干啥的不用我多说什么了吧
		template<class Func>
		static void Timeout(int msec, Func&& f) {
			auto func = std::make_shared<std::function<void()>>(f);
			Timer* timer = new Timer;
			timer->m_intervalMs = 0;
			timer->SetTickHandler([msec, func](Timer* t) {
				t->Stop();
				ezui::SleepMs(msec);
				(*func)();
				t->DeleteLater();
				});
			timer->Start();
		};
#else
		//Timeout干啥的不用我多说什么了吧
		template<class Func, class... Args>
		static void Timeout(int msec, Func&& f, Args&& ...args) {
			auto func = std::make_shared<std::function<void()>>(std::bind(f, args...));
			Timer* timer = new Timer;
			timer->m_intervalMs = 0;
			timer->SetTickHandler([msec, func](Timer* t) {
				t->Stop();
				ezui::SleepMs(msec);
				(*func)();
				t->DeleteLater();
				});
			timer->Start();
		};
#endif

	public:
		Timer(Object* ownerObject = NULL);

		//是否采用时钟模式(启用之后,执行回调间隔不受执行回调的耗时影响)
		void SetFixedRateMode(bool bEnable = true);

		//当前计时器是否正在运行
		bool IsRunning();

		//设置计时间隔(毫秒)
		void SetInterval(int msec);

		//获取计时间隔(毫秒)
		int GetInterval()const;

		//设置Tick回调函数(回调非UI线程 请自行处理线程同步)
		void SetTickHandler(const std::function<void(Timer*)>& handler);

		//启动计时器
		//immediate = true 表示立即执行一次Tick回调函数(时钟模式下此参数无效)
		void Start(bool immediate = false);

		//重置计时器已等待时间
		//immediate = true 表示立即执行一次Tick回调函数
		void Restart(bool immediate = false);

		//停止计时器
		void Stop();

		virtual ~Timer();
	};

	// 防抖计时器：多次触发会不断重新计时，仅在最后一次触发后的指定时间到达时执行回调
	class EZUI_API DebounceTimer EZUI_FINAL : private Timer{
	public:
		DebounceTimer(Object * ownerObject = NULL) : Timer(ownerObject) {}
		virtual ~DebounceTimer() {}

#ifdef EZUI_COMPILER_VS2010
		// 触发防抖：设置新的回调并重新开始计时，只有在后续一段时间内不再触发时才会执行
		template<class Func>
		void Trigger(int msec, Func && f)
		{
			auto func = std::forward<Func>(f);
			this->SetTickHandler([func](Timer* t) {
				t->Stop();   // 到达延迟时间后仅执行一次
				func();
				});
			this->SetInterval(msec);
			this->Restart(false);   // 重新开始等待时间
			Timer::Start(false);    // 启动计时
		}
#else
		// 触发防抖：设置新的回调并重新开始计时，只有在后续一段时间内不再触发时才会执行
		template<class Func, class... Args>
		void Trigger(int msec, Func && f, Args&& ...args)
		{
			auto func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
			this->SetTickHandler([func](Timer* t) {
				t->Stop();   // 到达延迟时间后仅执行一次
				func();
				});
			this->SetInterval(msec);
			this->Restart(false);   // 重新开始等待时间
			Timer::Start(false);    // 启动计时
		}
#endif

		// 取消等待中的回调（若尚未触发则不会执行）
		void Cancel()
		{
			Timer::Stop();
		}

		// 是否正在等待触发
		using Timer::IsRunning;

		// 重置已等待时间，从当前时刻重新开始计时
		using Timer::Restart;
	};

};
