#pragma once
#include "List.h"
#include "HScrollBar.h"
namespace EzUI {
	class UI_EXPORT  HList :
		public List
	{
	private:
		HScrollBar hScrollBar;
		void Init();
		int Offset(int offset);
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		HList();
		virtual ~HList();
		virtual ScrollBar* GetScrollBar()override;
	};
};