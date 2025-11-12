#pragma once
#include "Control.h"
#include "VScrollBar.h"
#include "Timer.h"

namespace ezui {
	class UI_EXPORT TextBox :
		public Control
	{
	private:
		TextAlign m_textAlign = TextAlign::MiddleLeft;//文字对其方式(针对单行输入框有效)
		bool m_bCareShow = false;//用于光标闪烁
		bool m_readOnly = false;//是否只读
		bool m_multiLine = false;//是否多行输入
		bool m_down = false;//是否具有焦点中
		bool m_focus = false;//是否具有焦点中
		Size m_fontBox;//文字box
		Point m_point_Start;//开始选中的位置
		Point m_point_End;//结束位置
		Point m_pointA;//A点
		Point m_pointB;//B点
		BOOL m_A_isTrailingHit;//如果是1表示是字符的后半边
		BOOL m_B_isTrailingHit;//如果是1表示是字符的后半边
		int m_A_TextPos = 0;//点击了第几个字符
		int m_B_TextPos = 0;//点击了第几个字符
		int m_textPos = 0;//当前文字的下标 0~text.size()
		int m_scrollX = 0;//用于左右滚动
		int m_scrollY = 0;//用于y轴滚动
		int m_lastX = 0;//上一个x位置
		int m_lastY = 0;//上一个y位置 
		int m_lastWidth = 0;//上次宽度
		int m_lastHeight = 0;//上次高度
		int m_maxLen = -1;//最大文字数量
		Timer* m_timer;//用于光标闪烁
		VScrollBar* m_vScrollbar;//垂直滚动条
		Font* m_font = NULL;//字体
		TextLayout* m_textLayout = NULL;//字体布局
		std::wstring& m_text;//文字
		std::wstring& m_placeholder;//placeholder懂得都懂 (在没有文字的情况下显示的文字)
		std::wstring& m_passwordChar;//密码占位符
		std::shared_ptr<std::atomic<bool>> m_alive;//输入框是否已被释放
		Rect m_careRect;//光标位置
		std::vector<RectF> m_selectRects;//选中的字符矩形
	private:
		int Width2();//内部使用(返回偏移后的宽度)
		int Height2();//内部使用(返回偏移后的高度)
		void CalcContentSize();//内部使用 计算和设置contentSize
		void Init();//初始化
		void InsertUnicode(const std::wstring& str);//插入unicode文字内部使用
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
		virtual void OnKeyChar(const KeyEventArgs& arg) override;
		virtual void OnKeyDown(const KeyEventArgs& arg)override;
		virtual void OnMouseDown(const MouseEventArgs& arg)override;
		virtual void OnMouseWheel(const MouseEventArgs& arg)override;
		virtual void OnMouseMove(const MouseEventArgs& arg) override;
		virtual void OnMouseUp(const MouseEventArgs& arg)override;
		virtual void OnFocus(const FocusEventArgs& arg) override;
		virtual void OnKillFocus(const FocusEventArgs& arg) override;
		virtual void OnLayout();
		void Offset(int moveY);
	public:
		std::function<void(TextBox* sender, const UIString& text)> TextChanged = NULL;
	public:
		TextBox(Object* ownerObject = NULL);
		virtual ~TextBox();
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		//获取焦点所在光标位置
		virtual Rect GetCareRect()override;
		//分析字符串
		void Analysis();
		//在当前光标中插入文字(可触发文字更改事件)
		void Insert(const UIString& str);
		//获取输入框文字
		UIString GetText()const;
		//获取滚动条
		virtual VScrollBar* GetVScrollBar()override;
		//设置文字(不会触发文字更改事件)
		void SetText(const UIString& text, bool triggerChangedEvent = false);
		//设置文字对齐方式(针对单行输入框有效)
		void SetTextAlign(TextAlign textAlign);
		//获取文字对齐方式
		TextAlign GetTextAlign();
		//是否多行显示
		bool IsMultiLine();
		//设置是否多行显示
		void SetMultiLine(bool multiLine);
		//设置为是否只读
		void SetReadOnly(bool bReadOnly);
		//是否为只读
		bool IsReadOnly();
		//设置最大输入字符个数
		void SetMaxLength(int maxLen);
		// 设置占位符文本
		void SetPlaceholderText(const UIString& text);
		//设置密码框占位符(建议单字符)
		void SetPasswordChar(const UIString& passwordChar);
	};
};
