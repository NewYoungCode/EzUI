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
		bool AutoWidth = false;//是否根据内容自动宽度
		void Init();
	public:
		HList();
		HList(Control* parent);
		virtual ~HList();
		int ContentLenght();
		virtual void OnLayout()override;
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual void RefreshScroll(const int& _maxRight);
		virtual ScrollBar* GetScrollBar()override;
		virtual int MoveScroll(int offset) override;
		virtual bool IsAutoWidth();//是否自动高度
		virtual void SetAutoWidth(bool flag);//设置自动高度
	};
};