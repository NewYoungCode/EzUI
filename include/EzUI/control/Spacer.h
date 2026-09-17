#pragma once
#include "Control.h"

namespace ezui {
	//添加弹簧无需用户手动释放(不可在栈上创建弹簧对象)
	class EZUI_API Spacer :public Control {
	public:
		Spacer();
		virtual bool IsSpacer()const EZUI_OVERRIDE EZUI_FINAL;
		virtual ~Spacer();
	};
	//具有绝对高度的 的弹簧
	class EZUI_API VSpacer :public Spacer {
	public:
		virtual ~VSpacer();
		VSpacer(int fixedHeight = 0);
	};
	//具有绝对宽度的 的弹簧
	class EZUI_API HSpacer :public Spacer {
	public:
		virtual ~HSpacer();
		HSpacer(int fixedWidth = 0);
	};
};
