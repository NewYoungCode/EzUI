#pragma once
#include "Control.h"
#include "VScrollBar.h"
#include "Timer.h"
namespace EzUI {
	class UI_EXPORT TextBox :
		public Control
	{
	private:
		VScrollBar _vScrollbar;
		int _lastWidth = 0;
		int _lastHeight = 0;
		bool _multiLine = false;
		std::wstring _text;//文字
		Size _fontBox;
		bool _down = false;//是否具有焦点中
		bool _focus = false;//是否具有焦点中
		Point _point_Start;//开始选中的位置
		Point _point_End;//结束位置
		std::list<Rect> _selectRects;//选中的字符矩形
		Rect _careRect;//光标位置
		Font* _font = NULL;//字体
		TextLayout* _textLayout = NULL;//字体布局
		Point _A;//A点
		BOOL _A_isTrailingHit;//如果是1表示是字符的后半边
		int _A_TextPos = 0;//点击了第几个字符
		Point _B;//B点
		BOOL _B_isTrailingHit;//如果是1表示是字符的后半边
		int _B_TextPos = 0;//点击了第几个字符
		int _textPos = 0;//当前文字的下标 0~text.size()
		int _scrollX = 0;//用于左右滚动
		int _scrollY = 0;//用于y轴滚动
		int _lastX = 0;//上一个x位置
		int _lastY = 0;//上一个y位置 
		Timer _timer;//用于光标闪烁
		bool _careShow = false;//用于光标闪烁
	private:
		void Init();
		void _Insert(const std::wstring& str);//插入文字
		bool DeleteRange();//删除选中内容
		bool GetSelectedRange(int* outPos, int* outCount);//获取当前被选中的区域 返回下标和个数
		bool Copy();//复制到剪切板
		bool Paste();//粘贴
		bool SelectedAll();//全选
		void OnBackspace();//退格键要做的事
		void BuildCare();//构建光标
		void BuildSelectedRect();
		Point ConvertPoint(const Point& pt);//坐标转换
	protected:
		virtual void OnRemove()override;
		virtual void SetAutoWidth(bool flag)override;
		virtual void SetAutoHeight(bool flag)override;
		virtual void OnForePaint(PaintEventArgs& e) override;
		virtual void OnKeyChar(const KeyboardEventArgs& arg) override;
		virtual void OnKeyDown(const KeyboardEventArgs& arg)override;
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnKillFocus(const KillFocusEventArgs& arg) override;
		virtual void OnLayout();
		void Offset(int moveY);
	public:
		size_t MaxCount = -1;//最大文字数量
		EString Placeholder;//placeholder懂得都懂 (在没有文字的情况下显示的文字)
		std::wstring PasswordChar;
		bool ReadOnly = false;//是否只读
		std::function<void(const EString&)> TextChanged = NULL;
	public:
		TextBox();
		virtual ~TextBox();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual Rect GetCareRect()override;
		void Analysis();//分析字符串
		void Insert(const EString& str);//在当前光标中插入文字
		const EString GetText();
		virtual ScrollBar* GetScrollBar()override;
		void SetText(const EString& text);
		bool IsMultiLine();
		void SetMultiLine(bool multiLine);
	};
};
