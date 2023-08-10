#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public Control
	{
	private:
		VScrollBar vScrollBar;
		void Init();
		//�Կؼ�����ƫ��
		int Offset(int offset);
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		VList();
		virtual ~VList();
	
		virtual ScrollBar* GetScrollBar() override;
	};
};