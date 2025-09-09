#pragma once
#include "EzUI.h"
#include "Timer.h"

namespace ezui {

	class UI_EXPORT Animation : public Object {
	private:
		Timer *m_timer;
		float m_startValue = 0;
		float m_endValue = 0;
		float m_currValue = 0;
		float m_speedPerMs = 0;
		std::chrono::steady_clock::time_point m_lastTime;
	public:
		//当值更改的时候发生的事件(请绑定此函数进行回调,请自行线程同步)
		std::function<void(float)> ValueChanged;
		Animation(Object* parentObject = NULL);
		virtual ~Animation();
		void SetStartValue(float value);
		void SetEndValue(float value);
		//开始动画
		void Start(int durationMs, int fps = 90);
		//动画是否已经停止
		bool IsStopped();
		void Stop();
	};

};
