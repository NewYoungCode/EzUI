#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  Label :
		public Control
	{
	protected:
		std::wstring _wstr;
		bool _underline = false;
		RectF fontBox;
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
	public:
		TextAlign TextAlign = TextAlign::MiddleCenter;
		bool AutoWidth = false;
		bool AutoHeight = false;
		Size GetFontWidth();
		Label();
		virtual ~Label();
		void SetText(const EString& text);
		void SetUnderline(bool enable = true);
		EString GetText()const;
	};
};