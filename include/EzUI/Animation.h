#pragma once
#include "EzUI.h"
#include "Timer.h"

namespace ezui {

	//动画缓动类型
	enum class EasingCurve :int16_t {
		Linear,  //线性平滑
		Damping  //阻尼衰减
	};

	//动画类(包含 线性动画 阻尼动画)
	class UI_EXPORT Animation : public Object {
	private:
		float m_startValue = 0;
		float m_endValue = 0;
		float m_currValue = 0;
		float m_damping;
		float m_stepMove;
		int m_interval;
		bool m_finished = true;
		std::atomic<bool> m_tickPending = false;
		EasingCurve m_easingCurve = EasingCurve::Linear;
		std::shared_ptr<std::atomic<bool>> m_alive;
		Timer* m_timer;
	public:
		//当值更改的时候发生的回调事件(请绑定此函数进行回调,已处理线程同步)
		std::function<void(float currValue)> ValueChanged;
		Animation(Object* ownerObject = NULL);
		virtual ~Animation();
		//设置动画缓动类型
		void SetEasingCurve(EasingCurve easingCurveType);
		//设置起始值
		void SetStartValue(float value);
		//设置结束值
		void SetEndValue(float value);
		float GetEndValue();
		float GetStartValue();
		//获取当前动画完成的百分比
		float GetRate();
		//开始动画(起始值步进至结束值)
		void Start(int durationMs, int fps = 90);
		//动画是否已经停止(判断的是计时器状态)
		bool IsStopped();
		//动画是否已跑完(判断起始值和结束值)
		bool IsFinished();
		void Stop();
	};

};
