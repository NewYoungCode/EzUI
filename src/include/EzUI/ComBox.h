#pragma once
#include "TextBox.h"
#include "Label.h"
#include "VList.h"
#include "PopupWindow.h"
#include "HLayout.h"

namespace EzUI {
	//���׵������б��
	class UI_EXPORT ComBox :public HLayout {
	private:
		//�����˵�ѡ��
		class MenuContent :public PopupWindow {
		public:
			Control* _hittestCtl;
			MenuContent(Control* ownerCtl, Control* hittestCtl);
			virtual void OnKillFocus(HWND wnd) override;
			virtual ~MenuContent();
		};
	private:
		//�����˵�����
		MenuContent* _menuWnd = NULL;
		//ѡ��֮����ʾ���ı���
		TextBox _textBox;
		//չ���˵��İ�ť
		Label _UpDown;

		VList _list;
		int _index = -1;
		void Init();
	protected:
		virtual void OnLayout()override;
	public:
		ComBox();
		EString GetText();
		int GetCheck();
		bool SetCheck(int pos);
		virtual ~ComBox();
		int AddItem(const EString& text);
		void RemoveItem(int index);
	};
};
