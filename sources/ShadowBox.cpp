#include "ShadowBox.h"

namespace ezui {

	ShadowBox::ShadowBox(int width, int height, HWND mainHwnd)
	{
		m_mainHWnd = mainHwnd;
		DWORD dwFlags = WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
		m_hWnd = CreateWindowExW(dwFlags, EZUI_WINDOW_CLASS, L"EzUI_ShadowWindow", WS_POPUP, 0, 0, width, height, NULL, NULL, ezui::__EzUI__HINSTANCE, NULL);
		ASSERT(m_hWnd);
		this->m_publicData = new WindowData;
		//绑定消息过程
		m_publicData->WndProc = [this](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ->LRESULT {
			return this->WndProc(uMsg, wParam, lParam);
			};
		UI_SET_USERDATA(Hwnd(), this->m_publicData);
	}

	LRESULT ShadowBox::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			break;
		default:
			break;
		}
		return ::DefWindowProcW(Hwnd(), uMsg, wParam, lParam);
	}
	bool ShadowBox::SetShadow(int m_Width, int m_Height, int iSize, float _radius) {
		float radius = _radius;
		if (radius == 0) {//判断窗口是不是有圆角
			//如果没有圆角就让窗口阴影加点弧度显得更和谐一点
			radius = iSize / 2.0f;//半径
		}
		int width = m_Width < m_Height ? m_Width : m_Height;
		int max_size = width / 2.0f - radius;
		if (max_size <= 0) {
			radius = 0;
			max_size = width / 2.0f;
		}
		iSize = (int)iSize < max_size ? iSize : max_size;
		double piAngle = 3.1415926;
		int iSizeB = 4 * iSize;
		double fN = piAngle / iSize / 5.0f;//设置四条边外模糊度 值越小越黑
		double lN = 1.0 / iSize;
		int iAplpha = 0;
		int Left = iSize + radius,
			Top = iSize + radius,
			Right = m_Width - iSize - radius,
			Bottom = m_Height - iSize - radius;
		//处理四边的阴影
		int x = 0, y = 0;
		for (size_t i = 0; i < iSize; ++i)
		{
			iAplpha = int(255 - cos(fN * (i)) * 255);
			for (x = Left; x <= Right; ++x)
			{
				SetAplpha(x, i, iAplpha, _radius);
				SetAplpha(x, m_Height - i - 1, iAplpha, _radius);
			}
			for (y = Top; y <= Bottom; ++y)
			{
				SetAplpha(i, y, iAplpha, _radius);
				SetAplpha(m_Width - i - 1, y, iAplpha, _radius);
			}
		}
		//处理R角的阴影
		double fL = 0;
		int iSizeR = iSize + radius;
		for (int y = 0; y < iSizeR; ++y)
		{
			for (int x = y; x < iSizeR; ++x)
			{
				fL = sqrt((iSizeR - x) * (iSizeR - x) + (iSizeR - y) * (iSizeR - y));
				if (fL <= radius) {
					break;
				}
				if (fL > radius && fL < iSizeR) {
					iAplpha = int(255 - cos(fN * (iSizeR - fL)) * 255);
				}
				else {
					iAplpha = 0;
				}
				SetAplpha(x, y, iAplpha, _radius);
				SetAplpha(y, x, iAplpha, _radius);

				SetAplpha(m_Width - x - 1, y, iAplpha, _radius);
				SetAplpha(m_Width - y - 1, x, iAplpha, _radius);

				SetAplpha(x, m_Height - y - 1, iAplpha, _radius);
				SetAplpha(y, m_Height - x - 1, iAplpha, _radius);

				SetAplpha(m_Width - x - 1, m_Height - y - 1, iAplpha, _radius);
				SetAplpha(m_Width - y - 1, m_Height - x - 1, iAplpha, _radius);
			}
		}
		return true;
	}

	void ShadowBox::SetAplpha(int x, int y, BYTE a, float radius) {
		DWORD* point = (DWORD*)m_bufBitmap->GetPixel() + (x + y * m_bufBitmap->Width());//起始地址+坐标偏移
		((BYTE*)point)[3] = a;//修改A通道数值
		////使用A通道数值进行预乘
		//float opacity = (1.0f / 255.0f) * a;
		//((BYTE*)point)[2] = 50 * opacity;//修改R通道数值
		//((BYTE*)point)[1] = 50 * opacity;//修改G通道数值
		//((BYTE*)point)[0] = 50 * opacity;//修改B通道数值
	}
	void ShadowBox::Update(int shadowMargin, int radius) {
		if (!::IsWindowVisible(m_mainHWnd) || ::IsIconic(m_mainHWnd)) {
			::ShowWindow(m_hWnd, SW_HIDE);
			return;
		}
		::ShowWindow(m_hWnd, SW_SHOW);
		::SetWindowPos(m_hWnd, m_mainHWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		Rect mainWinRect;//主窗口矩形
		{
			RECT winRect;
			::GetWindowRect(m_mainHWnd, &winRect);
			mainWinRect.X = winRect.left;
			mainWinRect.Y = winRect.top;
			mainWinRect.Width = winRect.right - winRect.left;
			mainWinRect.Height = winRect.bottom - winRect.top;
		}
		Size mainWinSize = mainWinRect.GetSize();//主窗口大小

		//阴影窗口所在的位置
		Rect shadowRect(mainWinRect.X - shadowMargin, mainWinRect.Y - shadowMargin, mainWinRect.Width + shadowMargin * 2, mainWinRect.Height + shadowMargin * 2);
		::MoveWindow(m_hWnd, shadowRect.X, shadowRect.Y, shadowRect.Width, shadowRect.Height, FALSE);
		//只有在大小发生改变或者圆角改变的时候才回去重新生成新的窗口阴影贴上去
		if (mainWinSize.Equals(m_lastSize) && m_radius == radius && m_lastShadowMargin == shadowMargin) {
			return;
		}
		m_lastShadowMargin = shadowMargin;//记录最后一次窗口阴影大小
		m_radius = radius;//记录最后一次窗口圆角值
		m_lastSize = mainWinSize;//记录最后一次阴影大小
		if (m_bufBitmap) {
			delete m_bufBitmap;
			m_bufBitmap = NULL;
		}
		//创建用于渲染阴影的位图
		m_bufBitmap = new Bitmap(shadowRect.Width, shadowRect.Height);
		SetShadow(shadowRect.Width, shadowRect.Height, shadowMargin, radius);//渲染阴影

		POINT point{ 0,0 };
		SIZE size{ shadowRect.Width, shadowRect.Height };
		BLENDFUNCTION blend{ 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.SourceConstantAlpha = 255;
		::UpdateLayeredWindow(m_hWnd, NULL, NULL, &size, m_bufBitmap->GetHDC(), &point, 0, &blend, ULW_ALPHA);//。。。。。。更新分层窗口
	}
	const HWND ShadowBox::Hwnd()
	{
		return m_hWnd;
	}
	ShadowBox::~ShadowBox()
	{
		if (::IsWindow(Hwnd())) {
			::DestroyWindow(Hwnd());
		}
		if (m_bufBitmap) {
			delete m_bufBitmap;
		}
		if (m_publicData) {
			delete m_publicData;
		}
	}
};