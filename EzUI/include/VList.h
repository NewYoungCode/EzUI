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
		bool AutoHeight = false;//�������ݵĸ߶��Զ��仯
		void Init();
	public:
		VList();
		VList(Control* parent);
		virtual ~VList();
		int ContentLenght();//��ȡ����ʵ�ʳ���
		virtual void OnLayout()override;
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual void OnChildPaint( PaintEventArgs& args)override;
		virtual void RefreshScroll(const int& _maxBottom);
		virtual ScrollBar* GetScrollBar() override;
		virtual int MoveScroll(int offset) override;

		virtual bool IsAutoHeight();//�Ƿ��Զ��߶�
		virtual void SetAutoHeight(bool flag);//�����Զ��߶�
	};
};