#pragma once
#include "Control.h"
namespace EzUI {
	//添加弹簧无需用户手动释放,
	class UI_EXPORT Spacer :public Control {
	public:
		virtual ~Spacer();
	};
	//具有绝对高度的 的弹簧
	class UI_EXPORT VSpacer :public Spacer {
	private:
		VSpacer();
	public:
		virtual ~VSpacer();
		VSpacer(int_t fixedHeight);
	};
	//具有绝对宽度的 的弹簧
	class UI_EXPORT HSpacer :public Spacer {
	private:
		HSpacer();
	public:
		virtual ~HSpacer();
		HSpacer(int_t fixedWidth);
	};
};
