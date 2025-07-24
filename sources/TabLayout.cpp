#include "TabLayout.h"

namespace ezui {
	TabLayout::TabLayout(Object* parentObject) :Control(parentObject)
	{
		Init();
	}
	void TabLayout::Init()
	{
		m_timer.Tick = [this](Timer* sender) {

			Invoke([this, sender]() {
				m_stepAcc += m_stepPerFrame;
				int_t stepMove = m_stepAcc;
				m_stepAcc -= stepMove;
				m_nowOffset += stepMove;

				// 检查是否到达终点
				if ((m_offset > 0 && m_nowOffset >= m_offset) ||
					(m_offset < 0 && m_nowOffset <= m_offset)) {
					m_nowOffset = m_offset;
					sender->Stop();
					this->Sort(); // 对齐页面
					this->Invalidate();
					return;
				}
				for (size_t i = 0; i < GetControls().size(); ++i) {
					Control* ctl = GetControls()[i];
					if (m_dlideDirection == SlideDirection::Horizontal) {
						ctl->SetRect(Rect(m_initial[i] - m_nowOffset, 0, Width(), Height()));
					}
					else {
						ctl->SetRect(Rect(0, m_initial[i] - m_nowOffset, Width(), Height()));
					}
				}
				this->Invalidate();
				});

			};
	}
	TabLayout::~TabLayout()
	{
	}
	void TabLayout::Remove(Control* ctl, bool freeCtl)
	{
		size_t index = this->IndexOf(ctl);
		__super::Remove(ctl, freeCtl);
		int_t newIndex = this->GetControls().size() - 1;
		newIndex = newIndex < 0 ? 0 : newIndex;
		this->SetPageIndex(newIndex);
		TryPendLayout();
	}
	void TabLayout::Sort()
	{
		int_t pos = 0;
		for (auto itor = GetControls().begin(); itor != GetControls().end(); ++itor)
		{
			if (m_dlideDirection == SlideDirection::Horizontal) {
				int_t x = Width() * (pos - m_pageIndex);//子控件索引-当前所在页索引
				(*itor)->SetRect(Rect(x, 0, Width(), Height()));
			}
			else {
				int_t y = Height() * (pos - m_pageIndex);//子控件索引-当前所在页索引
				(*itor)->SetRect(Rect(0, y, Width(), Height()));
			}
			(*itor)->SetVisible(true);
			++pos;
		}
	}
	void TabLayout::SetAttribute(const UIString& key, const UIString& value) {
		__super::SetAttribute(key, value);
		if (key == "tabindex") {
			this->SetPageIndex(std::atoi(value.c_str()));
		}
	}
	void TabLayout::OnLayout()
	{
		m_timer.Stop();
		Sort();
		this->EndLayout();
	}

	Control* TabLayout::Add(Control* childCtl)
	{
		auto ret = __super::Add(childCtl);
		this->TryPendLayout();
		return ret;
	}

	void TabLayout::SetPageIndex(int_t index)
	{
		this->m_pageIndex = index;
		this->TryPendLayout();
	}


	void TabLayout::SlideToPage(int_t index, SlideDirection dlideDirection, int_t durationMs, int_t fps)
	{
		//滑动方向
		m_dlideDirection = dlideDirection;

		Sort();//先直接归位
		int_t offsetTotal = (index - m_pageIndex) * (m_dlideDirection == SlideDirection::Horizontal ? Width() : Height());
		if (offsetTotal == 0) {
			return;
		}

		// 记录初始坐标
		int_t controlCount = GetControls().size();

		m_initial.clear();
		m_initial.resize(controlCount);
		for (int_t i = 0; i < controlCount; ++i) {
			if (m_dlideDirection == SlideDirection::Horizontal) {
				m_initial[i] = GetControls()[i]->X();
			}
			else {
				m_initial[i] = GetControls()[i]->Y();
			}
		}

		m_nowOffset = 0;
		m_offset = offsetTotal;
		m_pageIndex = index;

		int_t FRAME_INTERVAL_MS = 1000 / fps;
		int_t totalFrames = durationMs / FRAME_INTERVAL_MS;
		if (totalFrames <= 0) {
			totalFrames = 1;
		}

		m_stepPerFrame = offsetTotal * 1.0f / totalFrames;
		m_timer.Stop();
		m_timer.Interval = FRAME_INTERVAL_MS;
		m_timer.Start();
	}



	void TabLayout::SetPage(Control* ctl)
	{
#ifdef _DEBUG
		ASSERT(ctl->Parent == this);
#endif // _DEBUG
		this->m_pageIndex = this->IndexOf(ctl);
		this->TryPendLayout();
	}

	Control* TabLayout::GetPage() {
		int_t pos = 0;
		for (auto& it : GetControls()) {
			if (pos == this->m_pageIndex) {
				return it;
			}
			++pos;
		}
		return NULL;
	}
	int_t TabLayout::GetPageIndex()
	{
		return m_pageIndex;
	}
};