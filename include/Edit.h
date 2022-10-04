#pragma once
#include "Control.h"
#include "Timer.h"

namespace EzUI {
	class UI_EXPORT Edit :
		public Control
	{
	private:
		std::wstring text;//文字
		EString Placeholder;//placeholder懂得都懂
		bool _down = false;//是否具有焦点中
		bool _focus = false;//是否具有焦点中
		void Analysis();//分析字符串
	protected:
		Point point_Start;//开始选中的位置
		Point point_End;//结束位置
		Rect selectRect;//选中的字符矩形
		Rect careRect;//光标位置
		int FontHeight = 0;//字符高度
		Size FontBox;
		TextFormat* textFormat = NULL;//字体格式
		TextLayout* textLayout = NULL;//字体布局
		byte __i = 0;//用于光标闪烁
		Windows::Timer timer;//用于光标闪烁
	protected:
		Point A;
		BOOL A_isTrailingHit;//如果是1表示是字符的后半边
		int A_TextPos = 0;//点击了第几个字符

		Point B;
		BOOL B_isTrailingHit;//如果是1表示是字符的后半边
		int B_TextPos = 0;//点击了第几个字符
	protected:
		int TextPos = 0;
		BOOL isTrailingHit = TRUE;//如果是1表示后边
		int x = 0;
	protected:
		virtual bool OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
		virtual void OnMouseWheel(short zDelta, const Point& point)override;
	private:
		void Insert(const std::wstring& str);
		bool DeleteRange();
		bool GetSelectedRange(int* outPos, int* outCount);
		bool Copy();
		bool SelectedAll();
		void OnBackspace();
		Point ConvertPoint(const Point& pt);
	public:
		Edit();
		virtual ~Edit();
		virtual void SetAttribute(const EString& key, const EString& value);
		void SetText(const EString& text);
		EString GetText();
	};
};
