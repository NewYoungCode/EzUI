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
		void Init();
	public:
		bool AutoHeight = false;//�������ݵĸ߶��Զ��仯
	public:
		VList();
		VList(Control*parent);
		virtual ~VList();
		int ContentLenght();//��ȡ����ʵ�ʳ���
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