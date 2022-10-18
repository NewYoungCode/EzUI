#pragma once
#include "Control.h"
#include "Label.h"
#include "VList.h"
#include "HLayout.h"
#include "Edit.h"
#include "PoupWindow.h"
namespace EzUI {
	class UI_EXPORT ComBox :
		public HLayout
	{
	private:
		Edit edit;
		Label label;
		VList vlist;
		PoupWindow* poupWnd = NULL;
	public:
		ComBox();
		virtual ~ComBox();
		size_t AddItem(const EString& text);
		void RemoveItem(size_t pos);
	protected:
		virtual void OnMouseClick( MouseButton btn, const Point& pt) override;
		virtual void OnLoad() override;
		virtual void OnLayout()override;
	};
};