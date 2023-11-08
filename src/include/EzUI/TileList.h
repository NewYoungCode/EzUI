#pragma once
#include "List.h"
#include "VScrollBar.h"
namespace EzUI {
	class UI_EXPORT  TileList :
		public List
	{
	private:
		VScrollBar vScrollBar;
		bool AutoHeight = false;//根据内容的高度自动变化
		void Init();
		void Offset(int offset);
	protected:
		virtual void OnChildPaint(PaintEventArgs& args)override;
		virtual void OnLayout()override;
	public:
		TileList();
		virtual ~TileList();
		virtual ScrollBar* GetScrollBar()override;
	};
};