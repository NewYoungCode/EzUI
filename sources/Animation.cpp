#include "Animation.h"
namespace ezui {

	Animation::Animation(Object* parentObject)
		: Object(parentObject)
	{
		m_timer = new Timer(this);
		m_timer->Tick = [this](Timer* t) {
			auto now = std::chrono::steady_clock::now();
			auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_lastTime).count();
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
		m_timer->Stop();
	}

	void Animation::SetStartValue(float value)
	{
		m_startValue = value;
	}

	void Animation::SetEndValue(float value)
	{
		m_endValue = value;
	}

	void Animation::Start(int durationMs, int fps)
	{
		m_timer->Stop();

		m_currValue = m_startValue;
		float distance = m_endValue - m_startValue;
		m_speedPerMs = distance / durationMs;

		m_lastTime = std::chrono::steady_clock::now(); //记录开始时间
		m_timer->Interval = 1000 / fps; //频率
		m_timer->Start();
	}
	bool Animation::IsStopped() {
		return m_timer->IsStopped();
	}
	void Animation::Stop()
	{
		m_timer->Stop();
	}
}
