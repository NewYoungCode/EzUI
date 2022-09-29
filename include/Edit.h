#pragma once
#include "Control.h"
#include "Timer.h"

namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring text;//文字缓冲
		bool _focus = false;//是否具有焦点中
		void Analysis();
	protected:
		Point point_Start;
		Point point_End;
		Rect selectRect;//选中的字符矩形
		Rect careRect;//光标位置
		int FontHeight = 0;
		TextLayout* textLayout = NULL;
		TextFormat* textFormat = NULL;
	public:
		int TextPos = 0;

		Point A;
		BOOL A_isTrailingHit;//如果是1表示是字符的后半边
		int A_TextPos = 0;//点击了第几个字符

		Point B;
		BOOL B_isTrailingHit;//如果是1表示是字符的后半边
		int B_TextPos = 0;//点击了第几个字符
	protected:
		virtual bool OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
	protected:
		void BuildCare(int TextPos);
	public:
		virtual ~Edit();
		void SetText(const EString& text);
		EString GetText();
	};
};
