#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public Control, public IScroll
	{
	private:
		VScrollBar* vScrollBar = NULL;
	public:
		int _maxBottom = 0;
		bool AutoHeight = false;//根据内容的高度自动变化
	public:
		VList();
		virtual ~VList();
		virtual void OnLayout()override;
		virtual	void AddControl(Control* ctl) override;
		virtual ControlIterator RemoveControl(Control* ctl)override;
		virtual void Clear(bool freeChilds = false);
		virtual	void OnSize(const Size& size) override;
		virtual void ChildPainting(Controls& controls, PaintEventArgs& args)override;
		virtual void RefreshScroll(const int& _maxBottom);
		virtual ScrollBar* GetScrollBar() override;
	};
};