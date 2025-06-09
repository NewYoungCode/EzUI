#include "ShadowBox.h"

namespace EzUI {

	ShadowBox::ShadowBox(int_t width, int_t height, HWND hwnd)
	{
		DWORD dwFlags = WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
		_hWnd = CreateWindowExW(dwFlags, EzUI::__EzUI__WindowClassName, L"ShadowBox", WS_POPUP, 0, 0, width, height, hwnd, NULL, EzUI::__EzUI__HINSTANCE, NULL);
		ASSERT(_hWnd);
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
		double fN = piAngle / iSize / 6.0f;//设置四条边外模糊度
		double lN = 1.0 / iSize;
		int iAplpha = 0;
		int Left = iSize + radius,
			Top = iSize + radius,
			Right = m_Width - iSize - radius,
			Bottom = m_Height - iSize - radius;
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
		if (radius == 0 && _clipRect.Contains(x, y)) { //不允许绘制在OWner窗口区域
			return;
		}
		DWORD* point = (DWORD*)_bufBitmap->GetPixel() + (x + y * _bufBitmap->Width());//起始地址+坐标偏移
		((BYTE*)point)[3] = a;//修改A通道数值
		//((BYTE*)point)[2] = 0;//修改R通道数值
		//((BYTE*)point)[1] = 0;//修改G通道数值
		//((BYTE*)point)[0] = 0;//修改B通道数值
	}
	void ShadowBox::Update(int_t _shadowWidth, float radius) {
		HWND OwnerWnd = ::GetWindowOwner(_hWnd);
		if (!::IsWindowVisible(OwnerWnd)) {
			::ShowWindow(_hWnd, SW_HIDE);
			return;
		}
		::ShowWindow(_hWnd, SW_SHOW);

		RECT Orect;
		BOOL empty = ::GetWindowRect(OwnerWnd, &Orect);
		Size paintSize{ Orect.right - Orect.left,Orect.bottom - Orect.top };//父控件作图大小

		_clipRect = Rect({ _shadowWidth ,_shadowWidth }, paintSize);//裁剪区域
		int_t x = 0;
		int_t y = 0;
		int_t width = paintSize.Width + _shadowWidth * 2;
		int_t height = paintSize.Height + _shadowWidth * 2;
		//移动阴影窗口
		::MoveWindow(_hWnd, Orect.left - _shadowWidth, Orect.top - _shadowWidth, width, height, FALSE);
		//只有在大小发生改变的时候才回去重新生成layered窗口
		if (paintSize.Equals(_bufSize)) {
			return;
		}
		_bufSize = paintSize;
		if (_bufBitmap != NULL) {
			delete _bufBitmap;
			_bufBitmap = NULL;
		}
		_bufBitmap = new Bitmap(width, height, Bitmap::PixelFormat::PixelFormatARGB);//32位透明图
		Rect rect{ 0,0,width, height };
		SetShadow(rect.Width, rect.Height, _shadowWidth, radius);
		/*	{
				Rect clientRect(rect.X + _shadowWidth, rect.Y + _shadowWidth, rect.Width - _shadowWidth * 2, rect.Height - _shadowWidth * 2);
				DXRender render(_bufBitmap->GetDC(), clientRect.X, clientRect.Y, clientRect.Width, clientRect.Height);
				render.SetColor(Color(128, 128, 128, 128));
				render.DrawRectangle(Rect(0, 0, clientRect.Width, clientRect.Height), 0, 1);
			}*/
		POINT point{ 0,0 };
		SIZE size{ rect.Width,  rect.Height };
		BLENDFUNCTION blend{ 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.SourceConstantAlpha = 255;
		::UpdateLayeredWindow(_hWnd, NULL, NULL, &size, _bufBitmap->GetHDC(), &point, 0, &blend, ULW_ALPHA);//。。。。。。更新分层窗口
	}
	const HWND ShadowBox::Hwnd()
	{
		return _hWnd;
	}
	ShadowBox::~ShadowBox()
	{
		::SendMessage(_hWnd, WM_DESTROY, 0, 0);
		if (_bufBitmap) {
			delete _bufBitmap;
		}
	}
};