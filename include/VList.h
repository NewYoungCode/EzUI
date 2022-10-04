#pragma once
#include "Layout.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public Layout, public IScroll
	{
	private:
		VScrollBar* vScrollBar = NULL;
	public:
		int _maxBottom = 0;
	public:
		VList();
		virtual ~VList();
		virtual void ResumeLayout()override;
		virtual	void AddControl(Control* ctl) override;
		virtual ControlIterator RemoveControl(Control* ctl)override;
		virtual void Clear(bool freeChilds = false);
		virtual	bool OnSize(const Size& size) override;
		virtual void ChildPainting(Controls& controls, PaintEventArgs& args)override;
	};
};