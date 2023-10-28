#include "LayeredWindow.h"

namespace EzUI {

	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int width, int height, HWND owner) :BorderlessWindow(width, height, owner, WS_EX_LAYERED)
	{
		UpdateShadowBox();
		PublicData->InvalidateRect = [this](const Rect& rect) ->void {
			{
				std::unique_lock<std::mutex> autoLock(mtx);
				//�����Ч����
				this->InvalidateRect(rect);
			}
			if (IsVisible()) {
				//֪ͨ�̻߳���
				condv.notify_one();
			}
			};

		PublicData->UpdateWindow = [this]()->void {
			//ʵʱ����
			if (IsVisible() && !_InvalidateRect.IsEmptyArea()) {
				this->Invoke([this]() {
					this->Paint();
					});
			}
			};

		_paintTask = new Task([this]() {
			while (true)
			{
				{
					std::unique_lock<std::mutex> autoLock(mtx);
					condv.wait(autoLock, [this]()->bool {
						if (this->PublicData->HANDLE == NULL) {
							this->_bStop = true;
						}
						return (this->_bStop || !_InvalidateRect.IsEmptyArea());
						});
					if (this->_bStop) {
						break;
					}
				}
				//1000/5=200֡�ⶥ
				Sleep(5);
				this->Invoke([this]() {
					this->Paint();
					});
			}
			});
	}

	LayeredWindow::~LayeredWindow() {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			_bStop = true;
		}
		condv.notify_all();
		delete _paintTask;
		if (_winBitmap) {
			delete _winBitmap;
		}
	}

	void LayeredWindow::InvalidateRect(const Rect& _rect) {
		const Rect& clientRect = GetClientRect();
		int Width = clientRect.Width;
		int Height = clientRect.Height;
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
		//_InvalidateRect = clientRect;
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new EBitmap(sz.Width, sz.Height, EBitmap::PixelFormat::PixelFormatARGB);
		__super::OnSize(sz);
	}

	void LayeredWindow::Paint()
	{
		if (_winBitmap && !_InvalidateRect.IsEmptyArea()) {
			_winBitmap->Earse(_InvalidateRect);//�������
			HDC winHDC = _winBitmap->GetHDC();
			DoPaint(winHDC, _InvalidateRect);
			PushDC(winHDC);//updatelaredwindow ���´���
			_InvalidateRect = Rect();//��������
		}
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