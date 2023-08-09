#pragma once
#include "Control.h"

namespace EzUI {
	class UI_EXPORT  Label :
		public Control
	{
	private:
		std::wstring _wstr;
		size_t _underlinePos = size_t(-1);
		size_t _underlineCount = 0;
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnDpiChange(const DpiChangeEventArgs& args)override;
	public:
		EString EllipsisText;//�����������ʾ������
		EzUI::Distance Padding;//���ڿؼ������ֵı߾�
	public:
		TextAlign TextAlign = TextAlign::MiddleCenter;
		Label();
		virtual ~Label();
		virtual void SetAttribute(const EString& key, const EString& value)override;
		virtual void ResumeLayout() override;
		virtual void OnLayout()override;
		void SetText(const EString& text);
		void SetUnderline(size_t pos,size_t count);
		EString GetText()const;
	};
};