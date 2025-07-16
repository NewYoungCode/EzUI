#include "LayeredWindow.h"

namespace ezui {
	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int_t width, int_t height, HWND owner) :BorderlessWindow(width, height, owner, WS_EX_LAYERED)
	{
		m_timeOut.Interval = 0;
		m_timeOut.Tick = [this](Timer* t) {
			t->Stop();//停止
			Sleep(5);//延迟5ms之后再去绘制
			::SendMessage(Hwnd(), WM_PAINT,  NULL, NULL);
			};
		this->PublicData->InvalidateRect = [this](const Rect& rect) ->void {
			//标记窗口无效区域
			this->InvalidateRect(rect);
			};
		this->PublicData->UpdateWindow = [this]()->void {
			//立即更新窗口中的无效区域
			::SendMessage(Hwnd(), WM_PAINT, NULL, NULL);
			};
		//获取客户区大小 创建一个位图给窗口绘制
		Size sz = GetClientRect().GetSize();
		m_winBitmap = new Bitmap(sz.Width, sz.Height, Bitmap::PixelFormat::PixelFormatARGB);
	}
	LayeredWindow::~LayeredWindow() {
		if (m_winBitmap) {
			delete m_winBitmap;
		}
	}

	void LayeredWindow::InvalidateRect(const Rect& _rect) {
		//将此区域添加到无效区域
		m_invalidateRect.push_back(_rect);
		//timer延迟绘制
		m_timeOut.Start();
	}
	
	void LayeredWindow::BeginPaint(Rect* out_rect)
	{
		const Rect& clientRect = GetClientRect();
		int_t Width = clientRect.Width;
		int_t Height = clientRect.Height;
		//将所有无效区域并集
		for (auto& it : m_invalidateRect) {
			Rect rect = it;
			//这段代码是保证重绘区域一定是在窗口内
			if (rect.X < 0) {
				rect.Width += rect.X;
				rect.X = 0;
				if (rect.Width < 0) rect.Width = 0;
			}
			if (rect.Y < 0) {
				rect.Height += rect.Y;
				rect.Y = 0;
				if (rect.Height < 0) rect.Height = 0;
			}
			if (rect.GetRight() > Width) {
				rect.Width = Width - rect.X;
				if (rect.Width < 0) rect.Width = 0;
			}
			if (rect.GetBottom() > Height) {
				rect.Height = Height - rect.Y;
				if (rect.Height < 0) rect.Height = 0;
			}
			//并集
			Rect::Union(*out_rect, *out_rect, rect);
		}
	}
	void LayeredWindow::EndPaint()
	{
		m_invalidateRect.clear();
	}
	void LayeredWindow::Paint()
	{
		if (IsVisible()) {
			Rect invalidateRect;
			BeginPaint(&invalidateRect);
			if ((m_winBitmap && !invalidateRect.IsEmptyArea())) {
				m_winBitmap->Earse(invalidateRect);//清除背景
				HDC winHDC = m_winBitmap->GetHDC();
#if 1
				//不使用双缓冲
				DoPaint(winHDC, invalidateRect);
#else
				//使用双缓冲
				Bitmap doubleBuff(_winBitmap->Width(), _winBitmap->Height(), Bitmap::PixelFormat::PixelFormatARGB);
				DoPaint(doubleBuff.GetHDC(), invalidateRect);
				//使用BitBlt函数进行复制到winHDC  //如果窗体不规则 不适用于BitBlt进行复制
				::BitBlt(winHDC, invalidateRect.X, invalidateRect.Y,
					invalidateRect.Width, invalidateRect.Height,
					doubleBuff.GetHDC(), invalidateRect.X, invalidateRect.Y,
					SRCCOPY);
#endif
				UpdateLayeredWindow(winHDC);//updatelaredwindow 更新窗口
				EndPaint();
			}
		}
	}
	LRESULT LayeredWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_PAINT: {
			Paint();
			return TRUE;
		}
		default:
			break;
		}
		return __super::WndProc(uMsg, wParam, lParam);
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (m_winBitmap) {
			delete m_winBitmap;
		}
		m_winBitmap = new Bitmap(sz.Width, sz.Height, Bitmap::PixelFormat::PixelFormatARGB);
		__super::OnSize(sz);
	}

	void LayeredWindow::UpdateLayeredWindow(HDC hdc) {
		const Rect& _rectClient = GetClientRect();
		POINT point{ _rectClient.X,_rectClient.Y };
		SIZE size{ _rectClient.Width,  _rectClient.Height };
		BLENDFUNCTION blendFunc{ 0 };
		blendFunc.SourceConstantAlpha = 255;
		blendFunc.BlendOp = AC_SRC_OVER;
		blendFunc.AlphaFormat = AC_SRC_ALPHA;
		blendFunc.BlendFlags = 0;
		::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_ALPHA);//透明
	}
}