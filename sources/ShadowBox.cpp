#include "ShadowBox.h"

namespace ezui {

	ShadowBox::ShadowBox(int_t width, int_t height, HWND hwnd)
	{
		DWORD dwFlags = WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
		m_hWnd = CreateWindowExW(dwFlags, ezui::__EzUI__WindowClassName, L"ShadowBox", WS_POPUP, 0, 0, width, height, hwnd, NULL, ezui::__EzUI__HINSTANCE, NULL);
		ASSERT(m_hWnd);
		this->PublicData = new WindowData;
		//绑定消息过程
		PublicData->WndProc = [this](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) ->LRESULT {
			return this->WndProc(uMsg, wParam, lParam);
			};
		UI_SET_USERDATA(Hwnd(), this->PublicData);
	}

	LRESULT ShadowBox::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
			break;
		default:
			break;
		}
		return ::DefWindowProcW(Hwnd(), uMsg, wParam, lParam);
	}
	bool ShadowBox::SetShadow(int_t m_Width, int_t m_Height, int_t iSize, float _radius) {
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
				setA(x, i, iAplpha, _radius);
				setA(x, m_Height - i - 1, iAplpha, _radius);
			}
			for (y = Top; y <= Bottom; ++y)
			{
				setA(i, y, iAplpha, _radius);
				setA(m_Width - i - 1, y, iAplpha, _radius);
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
				setA(x, y, iAplpha, _radius);
				setA(y, x, iAplpha, _radius);

				setA(m_Width - x - 1, y, iAplpha, _radius);
				setA(m_Width - y - 1, x, iAplpha, _radius);

				setA(x, m_Height - y - 1, iAplpha, _radius);
				setA(y, m_Height - x - 1, iAplpha, _radius);

				setA(m_Width - x - 1, m_Height - y - 1, iAplpha, _radius);
				setA(m_Width - y - 1, m_Height - x - 1, iAplpha, _radius);
			}
		}
		return true;
	}

	void ShadowBox::setA(int_t x, int_t y, BYTE a, float radius) {
		//如果窗口没有圆角 则不允许圆角绘制到窗口内部去造成遮挡影响观感
		if (std::abs(radius) < 1e-6f && m_clipRect.Contains(x, y)) { //不允许绘制在OWner窗口区域
			return;
		}
		DWORD* point = (DWORD*)m_bufBitmap->GetPixel() + (x + y * m_bufBitmap->Width());//起始地址+坐标偏移
		((BYTE*)point)[3] = a;//修改A通道数值
		//使用A通道数值进行预乘
		float opacity = (1.0f / 255.0f) * a;
		((BYTE*)point)[2] = 50 * opacity;//修改R通道数值
		((BYTE*)point)[1] = 50 * opacity;//修改G通道数值
		((BYTE*)point)[0] = 50 * opacity;//修改B通道数值
	}
	void ShadowBox::Update(int_t _shadowWidth, const Border& border) {
		HWND ownerWnd = ::GetWindow(m_hWnd, GW_OWNER);
		if (!::IsWindowVisible(ownerWnd) || ::IsIconic(ownerWnd)) {
			::ShowWindow(m_hWnd, SW_HIDE);
			return;
		}
		::ShowWindow(m_hWnd, SW_SHOW);

		RECT Orect;
		BOOL empty = ::GetWindowRect(ownerWnd, &Orect);
		Size paintSize{ Orect.right - Orect.left,Orect.bottom - Orect.top };//父控件作图大小

		m_clipRect = Rect({ _shadowWidth ,_shadowWidth }, paintSize);//裁剪区域
		int_t x = 0;
		int_t y = 0;
		int_t width = paintSize.Width + _shadowWidth * 2;
		int_t height = paintSize.Height + _shadowWidth * 2;
		//移动阴影窗口
		::MoveWindow(m_hWnd, Orect.left - _shadowWidth, Orect.top - _shadowWidth, width, height, FALSE);
		//只有在大小发生改变的时候才回去重新生成layered窗口
		if (paintSize.Equals(m_bufSize) && m_borderWidth == border.TopLeftRadius) {
			return;
		}
		//新窗口圆角值
		m_borderWidth = border.TopLeftRadius;
		m_bufSize = paintSize;
		if (m_bufBitmap != NULL) {
			delete m_bufBitmap;
			m_bufBitmap = NULL;
		}
		m_bufBitmap = new Bitmap(width, height, Bitmap::PixelFormat::BGRA);//32位透明图
		Rect rect{ 0,0,width, height };
		SetShadow(rect.Width, rect.Height, _shadowWidth, border.TopLeftRadius);

#if 0
		{
			int_t roundWidth = m_bufBitmap->Width() - _shadowWidth * 2;
			int_t roundHeight = m_bufBitmap->Height() - _shadowWidth * 2;

			//绘制边框
			DXRender* render = NULL;

			//规则的矩形
			int_t borderLeft = border.Left;
			int_t borderTop = border.Top;
			int_t borderRight = border.Right;
			int_t borderBottom = border.Bottom;
			int_t topLeftRadius = border.TopLeftRadius;
			int_t topRightRadius = border.TopRightRadius;
			int_t bottomRightRadius = border.BottomRightRadius;
			int_t bottomLeftRadius = border.BottomLeftRadius;
			if (topLeftRadius == 0 && topRightRadius == 0 && bottomLeftRadius == 0 && bottomRightRadius == 0) {
				bool hasBorder = borderLeft || borderTop || borderRight || borderBottom;
				if (hasBorder) {

					render = new DXRender(m_bufBitmap->GetHDC(), 0, 0, width, height);
					render->SetTransform(_shadowWidth, _shadowWidth);
					render->SetStrokeStyle(border.Style);
					render->SetColor(border.Color);

					if (borderLeft > 0) {
						RectF rect(0, 0, (float)borderLeft, (float)roundHeight);
						render->FillRectangle(rect);
					}
					if (borderTop > 0) {
						RectF rect(0, 0, (float)roundWidth, (float)borderTop);
						render->FillRectangle(rect);
					}
					if (borderRight > 0) {
						RectF rect((float)(roundWidth - borderRight), 0, (float)borderRight, (float)roundHeight);
						render->FillRectangle(rect);
					}
					if (borderBottom > 0) {
						RectF rect(0, (float)(roundHeight - borderBottom), (float)roundWidth, (float)borderBottom);
						render->FillRectangle(rect);
					}
					render->SetTransform(0, 0);
				}
			}
			else {
				render = new DXRender(m_bufBitmap->GetHDC(), 0, 0, width, height);
				render->SetTransform(_shadowWidth, _shadowWidth);
				render->SetStrokeStyle(border.Style);
				render->SetColor(border.Color);

				int_t value1 = borderLeft > borderTop ? borderLeft : borderTop;
				int_t value2 = borderRight > borderBottom ? borderRight : borderBottom;
				int_t maxBorder = value1 > value2 ? value1 : value2;
				Geometry rr(Rect(0, 0, roundWidth, roundHeight), topLeftRadius, topRightRadius, bottomRightRadius, bottomLeftRadius);
				render->DrawGeometry(rr.Get(), maxBorder - 0.5f);//-0.5是因为Geometry这种方式抗锯齿导致看起来边框很宽
				render->SetTransform(0, 0);
			}

			if (render) {
				delete render;
			}
		}
#endif

		POINT point{ 0,0 };
		SIZE size{ rect.Width,  rect.Height };
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
		if (Hwnd()) {
			::DestroyWindow(Hwnd());
		}
		if (m_bufBitmap) {
			delete m_bufBitmap;
		}
		if (PublicData) {
			delete PublicData;
		}
	}
};