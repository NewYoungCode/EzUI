#include "LayeredWindow.h"

namespace EzUI {

	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int width, int height, HWND owner) :BorderlessWindow(width, height, owner, WS_EX_LAYERED)
	{
		UpdateShadowBox();
		PublicData.InvalidateRect = [=](void* _rect) ->void {
			Rect& rect = *(Rect*)_rect;
			this->InvalidateRect(rect);
			//����ʵʱ����
			if (IsVisible() && !_InvalidateRect.IsEmptyArea()) {
				::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
			}
			};
		PublicData.UpdateWindow = [=]()->void {
			//�����ظ����� ���ڻ��Ϊ��ķ�ʽ
			/*if (IsVisible() && !_InvalidateRect.IsEmptyArea()) {
				::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
			}*/
			};
	}

	LayeredWindow::~LayeredWindow() {
	}

	void LayeredWindow::InvalidateRect(const Rect& _rect) {
		int Width = GetClientRect().Width;
		int Height = GetClientRect().Height;
		Rect rect = _rect;
		if (rect.X < 0) {
			rect.X = 0;
			rect.Width += rect.X;
		}
		if (rect.Y < 0) {
			rect.Y = 0;
			rect.Height += rect.Y;
		}
		if (rect.GetBottom() > Height) {
			rect.Height = Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = Width - rect.X;
		} //��δ����Ǳ�֤�ػ�����һ�����ڴ�����
		Rect::Union(_InvalidateRect, _InvalidateRect, rect);
		//��˸�����ҵ��� �����Զ�ػ������ͻ��˽�������˸
		//_InvalidateRect = GetClientRect();
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new EBitmap(sz.Width, sz.Height, EBitmap::PixelFormat::PixelFormatARGB);
		__super::OnSize(sz);
	}

	LRESULT  LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_PAINT && _winBitmap && !_InvalidateRect.IsEmptyArea()) {
			_winBitmap->Earse(_InvalidateRect);//�������
			HDC winHDC = _winBitmap->GetDC();
			DoPaint(winHDC, _InvalidateRect);
			PushDC(winHDC);//updatelaredwindow ���´���
			_InvalidateRect = Rect();//��������
			return 0;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}

	void LayeredWindow::PushDC(HDC hdc) {
		const Rect& _rectClient = GetClientRect();
		POINT point{ _rectClient.X,_rectClient.Y };
		SIZE size{ _rectClient.Width,  _rectClient.Height };
		BLENDFUNCTION blendFunc{ 0 };
		blendFunc.SourceConstantAlpha = 255;
		blendFunc.BlendOp = AC_SRC_OVER;
		blendFunc.AlphaFormat = AC_SRC_ALPHA;
		blendFunc.BlendFlags = 0;
		::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_ALPHA);//͸��
	}
}