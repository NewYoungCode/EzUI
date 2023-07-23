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
		bool AutoWidth = false;//�Ƿ���������Զ����
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
		virtual bool IsAutoWidth();//�Ƿ��Զ��߶�
		virtual void SetAutoWidth(bool flag);//�����Զ��߶�
	};
};