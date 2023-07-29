#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public ScrollableControl
	{
	private:
		VScrollBar vScrollBar;
		bool AutoHeight = false;//�������ݵĸ߶��Զ��仯
		void Init();
		//�Կؼ�����ƫ��
		int Offset(int offset);
	public:
		VList();
		VList(Control* parent);
		virtual ~VList();
		virtual void OnLayout()override;
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual ScrollBar* GetScrollBar() override;
		virtual bool IsAutoHeight();//�Ƿ��Զ��߶�
		virtual void SetAutoHeight(bool flag);//�����Զ��߶�
	};
};