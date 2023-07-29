#pragma once
#include "Control.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT VList :
		public ScrollableControl
	{
	private:
		VScrollBar vScrollBar;
		bool AutoHeight = false;//根据内容的高度自动变化
		void Init();
		//对控件进行偏移
		int Offset(int offset);
	public:
		VList();
		VList(Control* parent);
		virtual ~VList();
		virtual void OnLayout()override;
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual ScrollBar* GetScrollBar() override;
		virtual bool IsAutoHeight();//是否自动高度
		virtual void SetAutoHeight(bool flag);//设置自动高度
	};
};