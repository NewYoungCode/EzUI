#pragma once
#include "core/UIDef.h"

#ifdef EZUI_COMPILER_VS2010

namespace ezui {

	class EZUI_API mutex
	{
	private:
		CRITICAL_SECTION m_cs;

		mutex(const mutex&);
		mutex& operator=(const mutex&);

	public:
		mutex();
		~mutex();

		void lock();
		bool try_lock();
		void unlock();

		CRITICAL_SECTION* native_handle();
	};

	class EZUI_API once_flag
	{
	private:
		mutex m_mutex;
		bool m_called;

		once_flag(const once_flag&);
		once_flag& operator=(const once_flag&);

#ifdef EZUI_COMPILER_VS2010
		template<typename Func>
		friend void call_once(once_flag& flag, Func&& func);
#else
		template<typename Func, typename... Args>
		friend void call_once(once_flag& flag, Func&& func, Args&&... args);
#endif

	public:
		once_flag();
	};

	struct defer_lock_t {};
	struct adopt_lock_t {};
	struct try_to_lock_t {};

	static const defer_lock_t defer_lock = defer_lock_t();
	static const adopt_lock_t adopt_lock = adopt_lock_t();
	static const try_to_lock_t try_to_lock = try_to_lock_t();

	template<typename Mutex>
	class lock_guard
	{
	private:
		Mutex& m_mutex;

		lock_guard(const lock_guard&);
		lock_guard& operator=(const lock_guard&);

	public:
		explicit lock_guard(Mutex& m)
			: m_mutex(m)
		{
			m_mutex.lock();
		}

		lock_guard(Mutex& m, adopt_lock_t)
			: m_mutex(m)
		{
		}

		~lock_guard()
		{
			m_mutex.unlock();
		}
	};

	template<typename Mutex>
	class unique_lock
	{
	private:
		Mutex* m_mutex;
		bool m_owns;

		unique_lock(const unique_lock&);
		unique_lock& operator=(const unique_lock&);

	public:
		typedef Mutex mutex_type;

		unique_lock()
			: m_mutex(NULL), m_owns(false)
		{
		}

		explicit unique_lock(Mutex& m)
			: m_mutex(&m), m_owns(false)
		{
			lock();
		}

		unique_lock(Mutex& m, defer_lock_t)
			: m_mutex(&m), m_owns(false)
		{
		}

		unique_lock(Mutex& m, adopt_lock_t)
			: m_mutex(&m), m_owns(true)
		{
		}

		unique_lock(Mutex& m, try_to_lock_t)
			: m_mutex(&m), m_owns(false)
		{
			try_lock();
		}

		~unique_lock()
		{
			if (m_owns && m_mutex) {
				m_mutex->unlock();
			}
		}

		void lock()
		{
			if (m_mutex && !m_owns) {
				m_mutex->lock();
				m_owns = true;
			}
		}

		bool try_lock()
		{
			if (!m_mutex || m_owns) {
				return false;
			}

			m_owns = m_mutex->try_lock();
			return m_owns;
		}

		void unlock()
		{
			if (m_mutex && m_owns) {
				m_mutex->unlock();
				m_owns = false;
			}
		}

		bool owns_lock() const
		{
			return m_owns;
		}

		operator bool() const
		{
			return owns_lock();
		}

		Mutex* mutex() const
		{
			return m_mutex;
		}

		Mutex* release()
		{
			Mutex* old = m_mutex;
			m_mutex = NULL;
			m_owns = false;
			return old;
		}
	};

#ifdef EZUI_COMPILER_VS2010
	template<typename Func>
	void call_once(once_flag& flag, Func&& func)
	{
		unique_lock<mutex> lock(flag.m_mutex);
		if (flag.m_called) {
			return;
		}
		std::forward<Func>(func)();
		flag.m_called = true;
	}
#else
	template<typename Func, typename... Args>
	void call_once(once_flag& flag, Func&& func, Args&&... args)
	{
		unique_lock<mutex> lock(flag.m_mutex);
		if (flag.m_called) {
			return;
		}

		std::forward<Func>(func)(std::forward<Args>(args)...);
		flag.m_called = true;
	}
#endif

};
#endif