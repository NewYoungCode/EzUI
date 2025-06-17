#include "TabLayout.h"

namespace EzUI {
	TabLayout::TabLayout()
	{
		timer.Tick = [this](ThreadTimer* sender) {

			Invoke([this, sender]() {
				stepAcc += stepPerFrame;
				int_t stepMove = stepAcc;
				stepAcc -= stepMove;
				nowOffset += stepMove;

				// 检查是否到达终点
				if ((offset > 0 && nowOffset >= offset) ||
					(offset < 0 && nowOffset <= offset)) {
					nowOffset = offset;
					sender->Stop();
					this->Sort(); // 对齐页面
					this->Invalidate();
					return;
				}
				for (size_t i = 0; i < GetControls().size(); ++i) {
					Control* ctl = GetControls()[i];
					ctl->SetRect(Rect(initialX[i] - nowOffset, 0, Width(), Height()));
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
			int_t x = Width() * (pos - _pageIndex);//子控件索引-当前所在页索引
			(*itor)->SetRect(Rect(x, 0, Width(), Height()));
			(*itor)->SetVisible(true);
			++pos;
		}
	}
	void TabLayout::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		if (key == "tabindex") {
			this->SetPageIndex(std::atoi(value.c_str()));
		}
	}
	void TabLayout::OnLayout()
	{
		timer.Stop();
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
		this->_pageIndex = index;
		this->TryPendLayout();
	}


	void TabLayout::SlideToPage(int_t index)
	{
		Sort();//先直接归位
		int_t pageWidth = Width();
		int_t offsetTotal = (index - _pageIndex) * pageWidth;
		if (offsetTotal == 0) {
			return;
		}

		// 记录初始坐标
		int_t controlCount = GetControls().size();

		initialX.clear();
		initialX.resize(controlCount);
		for (int_t i = 0; i < controlCount; ++i) {
			initialX[i] = GetControls()[i]->X();
		}

		nowOffset = 0;
		offset = offsetTotal;
		_pageIndex = index;

		int_t ANIMATION_DURATION_MS = 200;
		int_t FRAME_INTERVAL_MS = 16;
		int_t totalFrames = ANIMATION_DURATION_MS / FRAME_INTERVAL_MS;
		if (totalFrames <= 0) {
			totalFrames = 1;
		}

		stepPerFrame = offsetTotal * 1.0f / totalFrames;
		timer.Stop();
		timer.Interval = FRAME_INTERVAL_MS;
		timer.Start();
	}



	void TabLayout::SetPage(Control* ctl)
	{
#ifdef _DEBUG
		ASSERT(ctl->Parent == this);
#endif // _DEBUG
		this->_pageIndex = this->IndexOf(ctl);
		this->TryPendLayout();
	}

	Control* TabLayout::GetPage() {
		int_t pos = 0;
		for (auto& it : GetControls()) {
			if (pos == this->_pageIndex) {
				return it;
			}
			++pos;
		}
		return NULL;
	}
	int_t TabLayout::GetPageIndex()
	{
		return _pageIndex;
	}
};