#include "BorderlessWindow.h"
namespace ezui {
	BorderlessWindow::BorderlessWindow(int_t width, int_t height, HWND owner, DWORD exStyle) : Window(width, height, owner, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, exStyle)
	{
		//无边框
		auto style = ::GetWindowLong(Hwnd(), GWL_STYLE);
		style |= WS_THICKFRAME;
		::SetWindowLong(Hwnd(), GWL_STYLE, style);

		m_shadowBox = new ShadowBox(width, height, Hwnd());
		UpdateShadowBox();
	}
	BorderlessWindow::~BorderlessWindow() {
		CloseShadowBox();
	}
	ShadowBox* BorderlessWindow::GetShadowBox()
	{
		return m_shadowBox;
	}
	void BorderlessWindow::SetShadow(int_t padding)
	{
		m_shadowWeight = padding;
		UpdateShadowBox();
	}

	void BorderlessWindow::OnDpiChange(float systemScale, const Rect& newRect)
	{
		if (m_shadowBox) {//对窗口阴影进行新DPI适配
			if (this->m_shadowScale != systemScale) {
				this->m_shadowWeight *= systemScale / m_shadowScale;
				this->m_shadowScale = systemScale;
				UpdateShadowBox();
			}
		}
		//对边框进行新DPI适配
		float newScale = systemScale / GetPublicData()->Scale;
		__super::OnDpiChange(systemScale, newRect);
	}

	void BorderlessWindow::SetResizable(bool resize) {
		this->m_bResize = resize;
	}

	bool BorderlessWindow::IsResizable() {
		return this->m_bResize;
	}

	void BorderlessWindow::UpdateShadowBox() {
		if (m_shadowBox) {
			auto* mainLayout = this->GetLayout();
			m_shadowBox->Update(m_shadowWeight * this->GetScale(), (mainLayout ? mainLayout->GetBorderTopLeftRadius() : 0));
		}
	}
	void BorderlessWindow::CloseShadowBox()
	{
		if (m_shadowBox) {
			delete m_shadowBox;
			m_shadowBox = NULL;
		}
	}
	LRESULT  BorderlessWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg)
		{
		case WM_NCCALCSIZE:
		{
			if (wParam == TRUE) {
				return 0;
			}
			break;
		}
		case WM_NCPAINT:
		{
			return 0;
		}
		case WM_NCACTIVATE:
		{
			return (wParam == FALSE ? TRUE : FALSE);
		}
		case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS* pPos = (WINDOWPOS*)lParam;
			//在窗口显示/隐藏的时候更新阴影窗口
			if ((pPos->flags & SWP_SHOWWINDOW) || (pPos->flags & SWP_HIDEWINDOW)) {
				this->UpdateShadowBox();
			}
			break;
		}
		case WM_SHOWWINDOW: {
			//窗口的可见状态标志未被设置
			if (wParam == TRUE) {
				//SHOW WINDOWS
			}
			else {
				//HIDE WINDOWS
			}
			break;
		}
		case WM_NCHITTEST:
		{
			//非全屏状态下/非最大化状态下/当调整大小的标志为true 的情况下才允许调整大小;
			if (!IsFullScreen() && !IsMaximized() && this->IsResizable()) {
				RECT rc;
				::GetWindowRect(Hwnd(), &rc);
				POINT pt{ GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) };
				int_t x = 4;//
				if (pt.x < rc.left + x)
				{
					if (pt.y < rc.top + x)return HTTOPLEFT;//
					if (pt.y >= rc.bottom - x)return HTBOTTOMLEFT;//
					return HTLEFT;//
				}
				if (pt.x >= rc.right - x)//
				{
					if (pt.y < rc.top + x)return HTTOPRIGHT;//
					if (pt.y >= rc.bottom - x)return HTBOTTOMRIGHT;//
					return HTRIGHT;//
				}
				if (pt.y < rc.top + x)return HTTOP;//
				if (pt.y >= rc.bottom - x)return HTBOTTOM;//
				return HTCLIENT;//
			}
			break;
		}
		case WM_KILLFOCUS: {
			HWND wnd = (HWND)wParam;
			if (this->GetShadowBox() && wnd != this->GetShadowBox()->Hwnd()) {
				this->OnKillFocus(wnd);
			}
			else if (this->GetShadowBox() == NULL) {
				this->OnKillFocus(wnd);
			}
			return 0;
		}
		default:
			break;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void BorderlessWindow::OnMove(const Point& location)
	{
		__super::OnMove(location);
		UpdateShadowBox();
	}
	void BorderlessWindow::OnSize(const Size& sz)
	{
		__super::OnSize(sz);//避免先显示阴影再显示窗口的问题
		UpdateShadowBox();
	}
}