#pragma once
#include "Control.h"

namespace ezui {
	//文字标签
	class EZUI_API Label :
		public Control
	{
	private:
		TextAlign m_textAlign;//文字对齐方式
		int m_underlinePos;//显示下划线起始下标
		int m_underlineCount;//显示下划线文字个数
		int m_strikeThroughPos;//删除线起始下标
		int m_strikeThroughCount;//删除线文字个数
		std::wstring* m_wstr;
		std::wstring* m_ellipsisText;//文字溢出将显示的文字
		int m_lineHeight;//行高
	protected:
		virtual void OnForePaint(PaintEventArgs* args) EZUI_OVERRIDE;
		virtual bool ApplyAttribute(const UIString& key, const UIString& value)EZUI_OVERRIDE;
		virtual void OnLayout()EZUI_OVERRIDE;
		virtual void OnDpiChanged(DpiChangedEventArgs* args) EZUI_OVERRIDE;
	public:
		Label(Object* ownerObject = NULL);

		// Supported attributes:
		// valign | halign | align: top | bottom | mid | left | right | center
		// text: string
		// underline: offset,length
		// ellipsis: text
		virtual void SetAttribute(const UIString& key, const UIString& value) EZUI_OVERRIDE;

		//设置文字
		void SetText(const UIString& text);

		//获取文字
		UIString GetText()const;

		//设置文字溢出控件之后的显示文字
		void SetElidedText(const UIString& text);

		//设置下划线位置
		void SetUnderline(int pos, int count);

		//设置删除线位置
		void SetStrikeThrough(int pos, int count);

		//设置文字对齐方式
		void SetTextAlign(TextAlign textAlign);

		//获取文字对齐方式
		TextAlign GetTextAlign();

		//设置字体行高
		void SetLineHeight(int lineHeight);

		virtual ~Label();
	};

	//------------------------------------LinkLabel------------------------------------
	//超链接标签
	class LinkLabel :public Label {
	public:
		LinkLabel(Object* ownerObject = NULL);
		virtual ~LinkLabel();
	};
};
