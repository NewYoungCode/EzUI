#pragma once
#include "Control.h"
#include "HScrollBar.h"
namespace EzUI {
	class UI_EXPORT  HList :
		public Control
	{
	private:
		HScrollBar hScrollBar;
		bool AutoWidth = false;//�Ƿ���������Զ����
		void Init();
		int Offset(int offset);
	public:
		HList();
		HList(Control* parent);
		virtual ~HList();
		virtual void OnLayout()override;
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual ScrollBar* GetScrollBar()override;
		virtual bool IsAutoWidth();//�Ƿ��Զ��߶�
		virtual void SetAutoWidth(bool flag);//�����Զ��߶�
	};
};