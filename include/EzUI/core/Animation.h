#pragma once
#include "UICore.h"
#include "Timer.h"

namespace ezui {

	//动画缓动类型
	enum class EasingCurve :int16_t {
		Linear,  //线性平滑
		Damping, //阻尼衰减
		EaseIn,  //加速
		EaseOut, //减速
		Cubic,   //三次方缓动
		InOut    //先加速后减速
	};

	//动画类(包含 线性动画 阻尼动画)
	class EZUI_API Animation : public Object {
	private:
		float m_startValue = 0;
		float m_endValue = 0;
		float m_currValue = 0;
		int m_duration = 0;
		int m_elapsedTime = 0;
		bool m_finished = true;
		std::atomic<bool> m_tickPending = false;
		EasingCurve m_easingCurve = EasingCurve::Linear;
		int64_t m_startTime = 0;
		Timer* m_timer;
		std::atomic<int64_t> m_lastTickTime = 0;
	public:
		//当值更改的时候发生的回调事件(请绑定此函数进行回调,已处理线程同步)
		std::function<void(float currValue)> ValueChanged;

		Animation(Object* ownerObject = NULL);
		virtual ~Animation();

		//设置缓动类型
		void SetEasingCurve(EasingCurve easingCurveType);

		//设置起始值
		void SetStartValue(float value);

		//设置结束值
		void SetEndValue(float value);

		//获取结束值
		float GetEndValue();

		//获取起始值
		float GetStartValue();

		//获取当前值
		float GetValue();

		//获取当前动画完成的百分比
		float GetRatio();

		//开始动画(起始值步进至结束值)
		void Start(int durationMs);

		//动画是否正在运行(判断的是计时器状态)
		bool IsRunning();

		//动画是否已跑完(判断起始值和结束值)
		bool IsFinished();

		//停止动画
		void Stop();
	};

};
