#pragma once
#include "List.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public List
	{
	private:
		VScrollBar _vScrollBar;
		void Init();
		//对控件进行偏移
		void Offset(int_t offset);
	protected:
		virtual void OnLayout()override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
	public:
		VList();
		virtual ~VList();
		virtual ScrollBar* GetScrollBar() override;
	};
};