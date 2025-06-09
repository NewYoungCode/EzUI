#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  Label :
		public Control
	{
	private:
		std::wstring _wstr;
		int_t _underlinePos = int_t(-1);
		int_t _underlineCount = 0;
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnDpiChange(const DpiChangeEventArgs& args)override;
		virtual void OnLayout()override;
	public:
		//文字溢出将显示的文字
		EString Ellipsis;
		//基于控件的文字的边距
		EzUI::Distance TextMargin;
	public:
		TextAlign TextAlign = TextAlign::MiddleCenter;
		Label();
		virtual ~Label();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual void RefreshLayout() override;
		void SetText(const EString& text);
		void SetUnderline(size_t pos, size_t count);
		EString GetText()const;
	};
};