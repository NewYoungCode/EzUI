#pragma once
#include "core/UIDef.h"

#ifdef EZUI_COMPILER_VS2010

#include "platform/window/mutex.h"
namespace ezui {

	class EZUI_API condition_variable
	{
	private:
		CONDITION_VARIABLE m_cv;

		condition_variable(const condition_variable&);
		condition_variable& operator=(const condition_variable&);

	public:
		condition_variable();
		~condition_variable();

		void notify_one();
		void notify_all();
		void wait(unique_lock<mutex>& lock);

		template<typename Predicate>
		void wait(unique_lock<mutex>& lock, Predicate pred)
		{
			while (!pred()) {
				wait(lock);
			}
		}
	};

};
#endif
