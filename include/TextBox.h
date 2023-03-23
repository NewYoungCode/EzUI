#pragma once
#include "Control.h"
#include "Timer.h"
namespace EzUI {
	class UI_EXPORT TextBox :
		public Control
	{
		UI_GetClassName()
	private:
#ifndef UNICODE
		std::string ansiBuf;
#endif
		std::wstring text;//文字
		bool _down = false;//是否具有焦点中
		bool _focus = false;//是否具有焦点中
		Point point_Start;//开始选中的位置
		Point point_End;//结束位置
		Rect selectRect;//选中的字符矩形
		Rect careRect;//光标位置
		Size FontBox;//文字占用空间
		TextFormat* textFormat = NULL;//字体格式
		TextLayout* textLayout = NULL;//字体布局
		Point A;//A点
		BOOL A_isTrailingHit;//如果是1表示是字符的后半边
		int A_TextPos = 0;//点击了第几个字符
		Point B;//B点
		BOOL B_isTrailingHit;//如果是1表示是字符的后半边
		int B_TextPos = 0;//点击了第几个字符
		int TextPos = 0;//当前文字的下标 0~text.size()
		int x = 0;//用于滚动
		Windows::Timer timer;//用于光标闪烁
		bool _careShow = false;//用于光标闪烁
	private:
		void Analysis();//分析字符串
		void Insert(const std::wstring& str);//插入文字
		bool DeleteRange();//删除选中内容
		bool GetSelectedRange(int* outPos, int* outCount);//获取当前被选中的区域 返回下标和个数
		bool Copy();//复制到剪切板
		bool Paste();//粘贴
		bool SelectedAll();//全选
		void OnBackspace();//退格键要做的事
		void BuildCare();//构建光标
		Point ConvertPoint(const Point& pt);//坐标转换
	protected:
		virtual void OnRemove()override;
		virtual void OnSize(const Size& size)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam)override;
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam)override;
		virtual void OnMouseDown(MouseButton mbtn, const Point& point)override;
		virtual void OnMouseMove(const Point& point) override;
		virtual void OnMouseUp(MouseButton mbtn, const Point& point)override;
		virtual void OnKillFocus() override;
		virtual void OnMouseWheel(short zDelta, const Point& point)override;
	public:
		EString Placeholder;//placeholder懂得都懂 (在没有文字的情况下显示的文字)
		Color SelectColor = Color(100, 255, 0, 0);//选中颜色
		Color CareColor = Color::Black;//光标颜色
	public:
		TextBox();
		virtual ~TextBox();
		const EString GetText();
		EventTextChange TextChange = NULL;
		virtual void SetAttribute(const EString& key, const EString& value)override;
		void SetText(const EString& text);
	};
};
