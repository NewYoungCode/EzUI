#pragma once
#include "Control.h"
#include "HScrollBar.h"
namespace EzUI {
	class UI_EXPORT  HList :
		public Control
	{
	private:
		int _maxRight = 0;
		HScrollBar hScrollBar;
		void Init();
	public:
		bool AutoWidth = false;//是否根据内容自动宽度
	public:
		HList();
		HList(Control* parent);
		virtual ~HList();
		int ContentLenght();
		virtual void OnLayout()override;
		virtual void ChildPainting(Controls& controls, PaintEventArgs& args)override;
		virtual void RefreshScroll(const int& _maxRight);
		virtual ScrollBar* GetScrollBar()override;
	};
};