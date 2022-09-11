#pragma once
#include "Control.h"
namespace EzUI {
	class UI_EXPORT  ILayout {
	public:
		int Margin = 0;//如果是水平列表那就是 右边距 ,垂直滚动条就是下边距
		std::map<Control*, int> _controlsLocationX;
		std::map<Control*, int> _controlsLocationY;
	public:
		virtual void ResumeLayout() = 0;//重新布局 不会立即刷新
	};
	class UI_EXPORT  Layout :
		public Control, public ILayout
	{
	public:
		Layout();
		virtual void ResumeLayout() override;
		virtual ~Layout();
	};
	using Box = Layout;
};