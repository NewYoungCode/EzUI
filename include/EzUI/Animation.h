#pragma once
#include "EzUI.h"
#include "Timer.h"

namespace ezui {

	//动画缓动类型
	enum class EasingCurve {
		Linear,  //线性平滑
		Damping  //阻尼衰减
	};

	//动画类
	class UI_EXPORT Animation : public Object {
	private:
		TimerClock m_timer;
		float m_startValue = 0;
		float m_endValue = 0;
		float m_currValue = 0;
		std::atomic<bool> m_tickPending = false;
		std::shared_ptr<std::atomic<bool>> m_alive;
		bool m_finished = true;
		float m_damping;
		float m_stepMove;
		EasingCurve m_easingCurve = EasingCurve::Linear;
	public:
		//当值更改的时候发生的事件(请绑定此函数进行回调,已处理线程同步)
		std::function<void(float)> ValueChanged;
		Animation(Object* ownerObject = NULL);
		virtual ~Animation();
		//设置动画缓动类型
		void SetEasingCurve(EasingCurve easingCurveType);
		void SetStartValue(float value);
		void SetEndValue(float value);
		float GetEndValue();
		float GetStartValue();
		//开始动画
		void Start(int durationMs, int fps = 90);
		//动画是否已经停止(判断的是计时器状态)
		bool IsStopped()const;
		//动画是否已跑完(判断起始值和结束值)
		bool IsFinished()const;
		void Stop();
	};

};
