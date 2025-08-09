#include "PopupWindow.h"
namespace ezui {
	PopupWindow::PopupWindow(int_t width, int_t height, HWND owner) :LayeredWindow(width, height, owner)
	{
		this->SetResizable(false);
	}
	PopupWindow::PopupWindow(int_t width, int_t height, Control* ownerCtl) :LayeredWindow(width, height, ownerCtl->Hwnd())
	{
		this->m_ownerCtl = ownerCtl;
		this->SetResizable(false);
	}
	void PopupWindow::Show()
	{
		int_t x, y, width, height;
		const Rect& rect = this->GetClientRect();
		Rect ctlRect;

		POINT location;
		if (m_ownerCtl) {
			ctlRect = m_ownerCtl->GetClientRect();
			location.x = ctlRect.GetLeft();
			location.y = ctlRect.GetBottom();
			::ClientToScreen(m_ownerCtl->Hwnd(), &location);
		}
		else {
			::GetCursorPos(&location);
		}

		MonitorInfo monitorInfo;
		GetMontior(&monitorInfo, ::GetWindow(Hwnd(), GW_OWNER));
		x = location.x;
		y = location.y;
		width = rect.Width;
		height = rect.Height;
		if (m_ownerCtl) {
			width = ctlRect.Width;
		}
		if ((location.y + height) > monitorInfo.Rect.GetBottom()) {
			y -= height;
			if (m_ownerCtl) {
				y -= ctlRect.Height;
			}
		}
		this->SetRect({ x, y, width, height });
		__super::Show();
		::SetForegroundWindow(Hwnd());
		this->SetTopMost(true);
	}

	int_t PopupWindow::ShowModal(bool disableOnwer)
	{
		return __super::ShowModal(disableOnwer);
	}

	PopupWindow::~PopupWindow()
	{
	}

	void PopupWindow::OnKillFocus(HWND hWnd)
	{
		::DestroyWindow(Hwnd());
	}
};