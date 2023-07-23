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
		bool AutoHeight = false;//根据内容的高度自动变化
		void Init();
	public:
		VList();
		VList(Control* parent);
		virtual ~VList();
		int ContentLenght();//获取内容实际长度
		virtual void OnLayout()override;
		virtual void SetAttribute(const EString& attrName, const EString& attrValue)override;
		virtual void OnChildPaint( PaintEventArgs& args)override;
		virtual void RefreshScroll(const int& _maxBottom);
		virtual ScrollBar* GetScrollBar() override;
		virtual int MoveScroll(int offset) override;

		virtual bool IsAutoHeight();//是否自动高度
		virtual void SetAutoHeight(bool flag);//设置自动高度
	};
};