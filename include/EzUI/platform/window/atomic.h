#pragma once
#include "core/UIDef.h"

#ifdef EZUI_COMPILER_VS2010

#include <windows.h>
namespace ezui {

	template<typename T>
	class atomic
	{
	private:
		mutable CRITICAL_SECTION m_cs;
		T m_value;

		atomic(const atomic&);
		atomic& operator=(const atomic&);

	public:
		atomic()
			: m_value()
		{
			InitializeCriticalSection(&m_cs);
		}

		atomic(T value)
			: m_value(value)
		{
			InitializeCriticalSection(&m_cs);
		}

		~atomic()
		{
			DeleteCriticalSection(&m_cs);
		}

		T load() const
		{
			EnterCriticalSection(&m_cs);
			T value = m_value;
			LeaveCriticalSection(&m_cs);
			return value;
		}

		void store(T value)
		{
			EnterCriticalSection(&m_cs);
			m_value = value;
			LeaveCriticalSection(&m_cs);
		}

		T exchange(T value)
		{
			EnterCriticalSection(&m_cs);
			T oldValue = m_value;
			m_value = value;
			LeaveCriticalSection(&m_cs);
			return oldValue;
		}

		operator T() const
		{
			return load();
		}

		T operator=(T value)
		{
			store(value);
			return value;
		}

		T operator++()
		{
			EnterCriticalSection(&m_cs);
			T value = ++m_value;
			LeaveCriticalSection(&m_cs);
			return value;
		}

		T operator++(int)
		{
			EnterCriticalSection(&m_cs);
			T value = m_value++;
			LeaveCriticalSection(&m_cs);
			return value;
		}

		T operator--()
		{
			EnterCriticalSection(&m_cs);
			T value = --m_value;
			LeaveCriticalSection(&m_cs);
			return value;
		}

		T operator--(int)
		{
			EnterCriticalSection(&m_cs);
			T value = m_value--;
			LeaveCriticalSection(&m_cs);
			return value;
		}
	};

};

#endif // EZUI_COMPILER_VS2010
