#include "Animation.h"
namespace ezui {

	Animation::Animation(Object* parentObject)
		: Object(parentObject)
	{
		m_timer.Tick = [this](Timer* t) {
			ULONGLONG now = ::GetTickCount64();
			ULONGLONG delta = now - m_lastTime;
			m_lastTime = now;

			m_currValue += m_speedPerMs * delta;

			bool finished = false;
			if ((m_speedPerMs > 0 && m_currValue >= m_endValue) || (m_speedPerMs < 0 && m_currValue <= m_endValue)) {
				m_currValue = m_endValue;
				finished = true;
			}

			if (ValueChanged) {
				ValueChanged(m_currValue);
			}

			if (finished) {
				t->Stop();
			}
			};
	}

	Animation::~Animation()
	{
		m_timer.Stop();
	}

	void Animation::SetStartValue(double value)
	{
		m_startValue = value;
	}

	void Animation::SetEndValue(double value)
	{
		m_endValue = value;
	}

	void Animation::Start(int_t durationMs, int_t fps)
	{
		m_timer.Stop();

		m_currValue = m_startValue;
		double distance = m_endValue - m_startValue;
		m_speedPerMs = distance / durationMs;

		m_lastTime = ::GetTickCount64(); //记录开始时间
		m_timer.Interval = 1000 / fps; //频率
		m_timer.Start();
	}
	bool Animation::IsStopped() {
		return m_timer.IsStopped();
	}
	void Animation::Stop()
	{
		m_timer.Stop();
	}
}
