#include "ShadowBox.h"

namespace EzUI {

	ShadowBox::ShadowBox(int width, int height, HWND hwnd)
	{
		DWORD dwFlags = WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
		_hWnd = CreateWindowExW(dwFlags, Base::WindowClassName, L"ShadowBox", WS_POPUP, 0, 0, width, height, hwnd, NULL, GetModuleHandle(NULL), NULL);
		ASSERT(_hWnd);
	}

	bool ShadowBox::SetShadow(int m_Width, int m_Height, size_t iSize) {
		int width = m_Width < m_Height ? m_Width : m_Height;
		float radius = iSize / 2.0f;//�뾶
		int max_size = width / 2 - radius;
		if (max_size <= 0) {
			radius = 0;
			max_size = width / 2;
		}
		iSize = (int)iSize < max_size ? iSize : max_size;
		double piAngle = 3.1415926;
		int iSizeB = 4 * iSize;
		double fN = piAngle / iSize / 4.5;//������������ģ����
		double lN = 1.0 / iSize;
		int iAplpha = 0;
		int Left = iSize + radius,
			Top = iSize + radius,
			Right = m_Width - iSize - radius,
			Bottom = m_Height - iSize - radius;
		int x = 0, y = 0;
		for (size_t i = 0; i < iSize; i++)
		{
			iAplpha = int(255 - cos(fN * (i)) * 255);
			for (x = Left; x <= Right; x++)
			{
				setA(x, i, iAplpha);
				setA(x, m_Height - i - 1, iAplpha);
			}
			for (y = Top; y <= Bottom; y++)
			{
				setA(i, y, iAplpha);
				setA(m_Width - i - 1, y, iAplpha);
			}
		}
		double fL = 0;
		int iSizeR = iSize + radius;
		for (int y = 0; y < iSizeR; y++)
		{
			for (int x = y; x < iSizeR; x++)
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
				setA(x, y, iAplpha);
				setA(y, x, iAplpha);

				setA(m_Width - x - 1, y, iAplpha);
				setA(m_Width - y - 1, x, iAplpha);

				setA(x, m_Height - y - 1, iAplpha);
				setA(y, m_Height - x - 1, iAplpha);

				setA(m_Width - x - 1, m_Height - y - 1, iAplpha);
				setA(m_Width - y - 1, m_Height - x - 1, iAplpha);
			}
		}
		return true;
	}

	void ShadowBox::setA(const int& x, const int& y, const BYTE& a) {
		if (clipRect.Contains(x, y)) { //�����������OWner��������
			return;
		}
		DWORD* point = (DWORD*)_bufBitmap->point + (x + y * _bufBitmap->Width);//��ʼ��ַ+����ƫ��
		((BYTE*)point)[3] = a;//�޸�Aͨ����ֵ
		//((BYTE*)point)[2] = 0;//�޸�Rͨ����ֵ
		//((BYTE*)point)[1] = 10;//�޸�Gͨ����ֵ
		//((BYTE*)point)[0] = 10;//�޸�Bͨ����ֵ
	}
	void ShadowBox::Update(int _shadowWidth) {
		HWND OwnerWnd = ::GetWindowOwner(_hWnd);
		if (!::IsWindowVisible(OwnerWnd)) {
			::ShowWindow(_hWnd, SW_HIDE);
			return;
		}
		::ShowWindow(_hWnd, SW_SHOW);

		RECT Orect;
		BOOL empty = ::GetWindowRect(OwnerWnd, &Orect);
		Size paintSize{ Orect.right - Orect.left,Orect.bottom - Orect.top };//���ؼ���ͼ��С

		clipRect = Rect({ _shadowWidth ,_shadowWidth }, paintSize);//�ü�����
		int x = 0;
		int y = 0;
		int width = paintSize.Width + _shadowWidth * 2;
		int height = paintSize.Height + _shadowWidth * 2;
		//�ƶ���Ӱ����
		::MoveWindow(_hWnd, Orect.left - _shadowWidth, Orect.top - _shadowWidth, width, height, FALSE);
		//ֻ���ڴ�С�����ı��ʱ��Ż�ȥ��������layered����
		if (paintSize.Equals(_bufSize)) {
			return;
		}
		_bufSize = paintSize;
		if (_bufBitmap != NULL) {
			delete _bufBitmap;
			_bufBitmap = NULL;
		}
		_bufBitmap = new EBitmap(width, height, EBitmap::PixelFormat::PixelFormatARGB);//32λ͸��ͼ
		//Debug::Log(TEXT("Update BoxShadow"));
		Rect rect{ 0,0,width, height };
		//������
		SetShadow(rect.Width, rect.Height, _shadowWidth);

		POINT point{ 0,0 };
		SIZE size{ rect.Width,  rect.Height };
		BLENDFUNCTION blend{ 0 };
		blend.BlendOp = AC_SRC_OVER;
		blend.BlendFlags = 0;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.SourceConstantAlpha = 255;
		::UpdateLayeredWindow(_hWnd, NULL, NULL, &size, _bufBitmap->GetDC(), &point, 0, &blend, ULW_ALPHA);//���������������·ֲ㴰��
	}
	const HWND& ShadowBox::Hwnd()
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