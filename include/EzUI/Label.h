#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT  Label :
		public Control
	{
	private:
		std::wstring m_wstr;
		int_t m_underlinePos = int_t(-1);
		int_t m_underlineCount = 0;
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnDpiChange(const DpiChangeEventArgs& args)override;
		virtual void OnLayout()override;
	public:
		//文字溢出将显示的文字
		UIString Ellipsis;
		//基于控件的文字的边距
		ezui::Distance TextMargin;
		//文字对齐方式
		TextAlign TextAlign = TextAlign::MiddleCenter;
	public:
		Label(Object* parentObject=NULL);
		virtual ~Label();
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual void RefreshLayout() override;
		void SetText(const UIString& text);
		void SetUnderline(size_t pos, size_t count);
		UIString GetText()const;
	};
};