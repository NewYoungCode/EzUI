#pragma once
#include "UILoader.h"

namespace ezui {
	//控件链式选择器
	class UI_EXPORT UISelector
	{
	private:
		ControlCollection m_controls;//锁定的控件
	public:
		UISelector(const ControlCollection& controls);
		virtual ~UISelector();
		//排除单个控件
		UISelector& Not(Control* fiterCtl);
		//排除一个集合中的全部控件
		UISelector& Not(const ControlCollection& fiterControls);
		//循环最终选定的控件
		void Each(const std::function<void(Control* it)>& eachFunc);
	};
	//用于简便操作
#define $ UISelector
};
