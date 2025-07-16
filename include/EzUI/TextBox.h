#pragma once
#include "Control.h"
#include "VScrollBar.h"
#include "Timer.h"

namespace ezui {
	class UI_EXPORT TextBox :
		public Control
	{
	private:
		VScrollBar m_vScrollbar;
		int_t m_lastWidth = 0;
		int_t m_lastHeight = 0;
		bool m_multiLine = false;
		std::wstring m_text;//文字
		Size m_fontBox;
		bool m_down = false;//是否具有焦点中
		bool m_focus = false;//是否具有焦点中
		Point m_point_Start;//开始选中的位置
		Point m_point_End;//结束位置
		std::list<Rect> m_selectRects;//选中的字符矩形
		Rect m_careRect;//光标位置
		Font* m_font = NULL;//字体
		TextLayout* m_textLayout = NULL;//字体布局
		Point m_pointA;//A点
		BOOL m_A_isTrailingHit;//如果是1表示是字符的后半边
		int_t m_A_TextPos = 0;//点击了第几个字符
		Point m_pointB;//B点
		BOOL m_B_isTrailingHit;//如果是1表示是字符的后半边
		int_t m_B_TextPos = 0;//点击了第几个字符
		int_t m_textPos = 0;//当前文字的下标 0~text.size()
		int_t m_scrollX = 0;//用于左右滚动
		int_t m_scrollY = 0;//用于y轴滚动
		int_t m_lastX = 0;//上一个x位置
		int_t m_lastY = 0;//上一个y位置 
		Timer m_timer;//用于光标闪烁
		bool m_bCareShow = false;//用于光标闪烁
	private:
		void Init();
		void InsertUnicode(const std::wstring& str);//插入unicode文字内部使用
		bool DeleteRange();//删除选中内容
		bool GetSelectedRange(int_t* outPos, int_t* outCount);//获取当前被选中的区域 返回下标和个数
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
		virtual void OnFocus(const FocusEventArgs& arg) override;
		virtual void OnKillFocus(const KillFocusEventArgs& arg) override;
		virtual void OnLayout();
		void Offset(int_t moveY);
	public:
		int_t MaxCount = -1;//最大文字数量
		UIString Placeholder;//placeholder懂得都懂 (在没有文字的情况下显示的文字)
		std::wstring PasswordChar;
		bool ReadOnly = false;//是否只读
		std::function<void(const UIString&)> TextChanged = NULL;
	public:
		TextBox();
		TextBox(Control* parentObject);
		virtual ~TextBox();
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual Rect GetCareRect()override;
		//分析字符串
		void Analysis();
		//在当前光标中插入文字
		void Insert(const UIString& str);
		const UIString GetText();
		//获取滚动条
		virtual IScrollBar* GetScrollBar()override;
		void SetText(const UIString& text);
		//是否多行显示
		bool IsMultiLine();
		//设置是否多行显示
		void SetMultiLine(bool multiLine);
	};
};
