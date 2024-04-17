#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VList.h"
#include "PopupWindow.h"

namespace EzUI {
	class UI_EXPORT ComBox :public Control {
	private:
		class MenuContent :public PopupWindow {
		public:
			MenuContent(Control* ownerCtl);
			virtual void OnKillFocus(HWND wnd) override;
			virtual ~MenuContent();
		};
	private:
		VList _list;
		int _index = -1;
		TextBox _textBox;
		MenuContent* _wnd = NULL;
		Rect _hittestRect;
		void Init();
	protected:
		virtual void OnPaint(PaintEventArgs& args) override;
		virtual void OnLayout()override;
		virtual void OnMouseClick(const MouseEventArgs& args) override;
	public:
		ComBox();
		EString GetText();
		int SelectedIndex();
		virtual ~ComBox();
		int AddItem(const EString& text);
		void RemoveItem(int index);
	};
};
