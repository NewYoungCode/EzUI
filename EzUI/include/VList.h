#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public Control
	{
	private:
		VScrollBar vScrollBar;
		int _maxBottom = 0;
		void Init();
	public:
		bool AutoHeight = false;//根据内容的高度自动变化
	public:
		VList();
		VList(Control* parent);
		virtual ~VList();
		int ContentLenght();//获取内容实际长度
		virtual void OnLayout()override;
		virtual void ChildPainting(Controls& controls, PaintEventArgs& args)override;
		virtual void RefreshScroll(const int& _maxBottom);
		virtual ScrollBar* GetScrollBar() override;
	};
};