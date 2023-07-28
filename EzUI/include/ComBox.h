#pragma once
#include "Control.h"
#include "Label.h"
#include "VList.h"
#include "HLayout.h"
#include "TextBox.h"
#include "ToolWindow.h"
namespace EzUI {
	class UI_EXPORT ComBox :
		public HLayout
	{
	private:
		TextBox edit;
		Label label;
		VList vlist;
		PoupWindow* poupWnd = NULL;
		void Init();
	public:
		ComBox();
		ComBox(Control*parent);
		virtual ~ComBox();
		size_t AddItem(const EString& text);
		void RemoveItem(size_t pos);
	protected:
		virtual void OnMouseClick( MouseButton btn, const Point& pt) override;
		virtual void OnLayout()override;
	};
};