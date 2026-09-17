#pragma once
#include "core/UIDef.h"
#ifdef EZUI_COMPILER_VS2010

namespace ezui {

	class EZUI_API thread
	{
	private:
		HANDLE m_handle;
		DWORD m_threadId;

		struct thread_data_base
		{
			virtual ~thread_data_base();
			virtual void run() = 0;
		};

		template<typename Func>
		struct thread_data : public thread_data_base
		{
			Func m_func;

			explicit thread_data(Func func)
				: m_func(func)
			{
			}

			virtual void run()
			{
				m_func();
			}
		};

		static DWORD WINAPI thread_proc(LPVOID param);
		void start(thread_data_base* data);

		thread(const thread&);
		thread& operator=(const thread&);

	public:
		class EZUI_API id
		{
		private:
			DWORD m_id;

		public:
			id();
			explicit id(DWORD value);

			bool operator==(const id& other) const;
			bool operator!=(const id& other) const;
			bool operator<(const id& other) const;
		};

		thread();

		template<typename Func>
		explicit thread(Func func)
			: m_handle(NULL), m_threadId(0)
		{
			start(new thread_data<Func>(func));
		}

		~thread();

		bool joinable() const;
		void join();
		void detach();
		id get_id() const;

		static unsigned int hardware_concurrency();
	};

	namespace this_thread {

		EZUI_API thread::id get_id();
		EZUI_API void yield();
		EZUI_API void sleep_for(unsigned long milliseconds);

	} // namespace this_thread

};
#endif