#pragma once
#include "Layout.h"
#include "HScrollBar.h"
namespace EzUI {
	class UI_EXPORT  HList :
		public Layout, public IScroll
	{
	private:
		HScrollBar* hScrollBar = NULL;
	public:
		int _maxRight = 0;
	public:
		HList();
		virtual ~HList();
		virtual	void SetMargin(int margin);
		virtual void ResumeLayout()override;
		virtual	void AddControl(Control* ctl) override;
		virtual ControlIterator RemoveControl(Control* ctl)override;
		virtual void Clear(bool freeControls) override;
		virtual	void OnSize(const Size& size) override;
		virtual void OnChildPaint(Controls& controls, PaintEventArgs& args)override;
	};
};