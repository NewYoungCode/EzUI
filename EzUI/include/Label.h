#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  Label :
		public Control
	{
	protected:
		std::wstring _wstr;
		bool _underline = false;
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
	public:
		EString EllipsisText;//文字溢出将显示的文字
	public:
		TextAlign TextAlign = TextAlign::MiddleCenter;
		bool AutoWidth = false;
		bool AutoHeight = false;
		Label();
		virtual ~Label();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual void DoPaint(PaintEventArgs& args)override;
		void SetText(const EString& text);
		void SetUnderline(bool enable = true);
		EString GetText()const;
	};
};