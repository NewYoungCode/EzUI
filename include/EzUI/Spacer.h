#pragma once
#include "Control.h"

namespace ezui {
	//添加弹簧无需用户手动释放(不可在栈上创建弹簧对象)
	class UI_EXPORT Spacer :public Control {
	public:
		Spacer();
		virtual bool IsSpacer()override final;
		virtual ~Spacer();
	};
	//具有绝对高度的 的弹簧
	class UI_EXPORT VSpacer :public Spacer {
	public:
		virtual ~VSpacer();
		VSpacer(int fixedHeight = 0);
	};
	//具有绝对宽度的 的弹簧
	class UI_EXPORT HSpacer :public Spacer {
	public:
		virtual ~HSpacer();
		HSpacer(int fixedWidth = 0);
	};
};
