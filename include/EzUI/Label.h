#pragma once
#include "Control.h"

namespace ezui {
	class UI_EXPORT  Label :
		public Control
	{
	private:
		std::wstring m_wstr;
		int m_underlinePos = 0;//显示下划线起始下标
		int m_underlineCount = 0;//显示下划线文字个数
		std::wstring m_ellipsisText;//文字溢出将显示的文字
	protected:
		virtual void OnForePaint(PaintEventArgs& args) override;
		virtual void OnLayout()override;
	public:
		//文字对齐方式
		TextAlign TextAlign = TextAlign::MiddleCenter;
	public:
		Label(Object* ownerObject = NULL);
		virtual ~Label();
		virtual void SetAttribute(const UIString& key, const UIString& value)override;
		virtual void RefreshLayout() override;
		//设置文字
		void SetText(const UIString& text);
		//获取文字
		UIString GetText()const;
		//设置文字溢出控件之后的显示文字
		void SetElidedText(const UIString& text);
		//设置下划线位置
		void SetUnderline(int pos, int count);
	};
};