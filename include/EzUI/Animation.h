#pragma once
#include "EzUI.h"
#include "Timer.h"

namespace ezui {

	class UI_EXPORT Animation : public Object {
	private:
		Timer m_timer;
		double m_startValue = 0;
		double m_endValue = 0;
		double m_currValue = 0;
		double m_speedPerMs = 0;
		ULONGLONG m_lastTime = 0;
	public:
		//当值更改的时候发生的事件(请绑定此函数进行回调,请自行线程同步)
		std::function<void(double)> ValueChanged;
		Animation(Object* parentObject = NULL);
		virtual ~Animation();
		void SetStartValue(double value);
		void SetEndValue(double value);
		void Start(int_t durationMs, int_t fps = 90);
		void Stop();
	};

};
