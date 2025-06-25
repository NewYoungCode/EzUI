#include "BorderlessWindow.h"
namespace EzUI {
	BorderlessWindow::BorderlessWindow(int_t width, int_t height, HWND owner, DWORD exStyle) : Window(width, height, owner, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_POPUP, exStyle)
	{
		//无边框
		auto style = ::GetWindowLong(Hwnd(), GWL_STYLE);
		style |= WS_THICKFRAME;
		::SetWindowLong(Hwnd(), GWL_STYLE, style);

		_shadowBox = new ShadowBox(width, height, Hwnd());
		UpdateShadowBox();
	}
	BorderlessWindow::~BorderlessWindow() {
		CloseShadowBox();
	}
	ShadowBox* BorderlessWindow::GetShadowBox()
	{
		return _shadowBox;
	}
	void BorderlessWindow::SetShadow(int_t padding)
	{
		_shadowWeight = padding;
		UpdateShadowBox();
	}
	void BorderlessWindow::OnPaint(PaintEventArgs& args) {
		const auto& border = this->Border;
		const auto& rect = this->GetClientRect();
		bool hasRadius = border.TopLeftRadius || border.TopRightRadius || border.BottomRightRadius || border.BottomLeftRadius;
		if (hasRadius) {
			//处理圆角控件 使用纹理的方式 (这样做是为了控件内部无论怎么绘制都不会超出圆角部分) 带抗锯齿
			Geometry roundRect(rect, border.TopLeftRadius, border.TopRightRadius, border.BottomRightRadius, border.BottomLeftRadius);
			args.PushLayer(roundRect);
		}
		else {
			args.PushLayer(rect);
		}
		__super::OnPaint(args);
		args.PopLayer();
	}

	void BorderlessWindow::OnDpiChange(float systemScale, const Rect& newRect)
	{
		if (_shadowBox) {//对窗口阴影进行新DPI适配
			if (this->_shadowScale != systemScale) {
				this->_shadowWeight *= systemScale / _shadowScale;
				this->_shadowScale = systemScale;
				UpdateShadowBox();
			}
		}
		//对边框进行新DPI适配
		float newScale = systemScale / PublicData->Scale;
		this->Border.Scale(newScale);
		__super::OnDpiChange(systemScale, newRect);
	}

	void BorderlessWindow::SetResizable(bool resize) {
		this->_resize = resize;
	}

	bool BorderlessWindow::IsResizable() {
		return this->_resize;
	}

	void BorderlessWindow::UpdateShadowBox() {
		if (_shadowBox) {
			_shadowBox->Update(_shadowWeight * this->GetScale(), this->Border);
		}
	}
	void BorderlessWindow::CloseShadowBox()
	{
		if (_shadowBox) {
			delete _shadowBox;
			_shadowBox = NULL;
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