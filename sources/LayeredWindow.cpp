#include "LayeredWindow.h"

namespace EzUI {

	std::vector<LayeredWindow*> windows;
	Task* task = NULL;
	std::mutex mtx;
	std::condition_variable condv;

	void __QueuePaint(LayeredWindow* window) {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			windows.push_back(window);
		}
		if (task == NULL) {
			task = new Task([]() {
				while (true)
				{
					{
						std::unique_lock<std::mutex> autoLock(mtx);
						condv.wait(autoLock, []()->bool {
							bool ok = false;
							for (auto& it : windows) {
								if (it->GetUpdateRect().IsEmptyArea() == false) {
									ok = true;
								}
							}
							return ok || windows.empty();
							});
						if (windows.empty()) {
							break;
						}
					}
					EzUI::Invoke([]() {
						for (auto& it : windows) {
							it->PublicData->UpdateWindow();
						}
						});
					Sleep(5);
				}
				});
		}
	}

	//WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT
	LayeredWindow::LayeredWindow(int width, int height, HWND owner) :BorderlessWindow(width, height, owner, WS_EX_LAYERED)
	{
		UpdateShadowBox();
		__QueuePaint(this);//加入绘制列队
		PublicData->InvalidateRect = [this](const Rect& rect) ->void {
			{
				std::unique_lock<std::mutex> autoLock(mtx);
				//标记无效区域
				this->InvalidateRect(rect);
			}
			if (IsVisible()) {
				//通知线程绘制
				condv.notify_one();
			}
			};
		PublicData->UpdateWindow = [this]()->void {
			//实时绘制
			if (IsVisible() && !_invalidateRect.IsEmptyArea()) {
				this->Paint();
			}
			};
	}

	LayeredWindow::~LayeredWindow() {
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			//清理绘制相关
			auto itor = std::find(windows.begin(), windows.end(), this);
			if (itor != windows.end()) {
				windows.erase(itor);
			}
		}
		//通知已经移除了一个窗口了
		condv.notify_all();
		bool isEmpty = false;
		{
			std::unique_lock<std::mutex> autoLock(mtx);
			isEmpty = windows.empty();
		}
		if (isEmpty && task) {
			delete task;
			task = NULL;
		}
		if (_winBitmap) {
			delete _winBitmap;
		}
	}
	Rect LayeredWindow::GetUpdateRect()
	{
		return _invalidateRect;
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
		} //这段代码是保证重绘区域一定是在窗口内
		Rect::Union(_invalidateRect, _invalidateRect, rect);
	}
	void LayeredWindow::OnSize(const Size& sz) {
		if (_winBitmap) {
			delete _winBitmap;
		}
		_winBitmap = new Bitmap(sz.Width, sz.Height, Bitmap::PixelFormat::PixelFormatARGB);
		__super::OnSize(sz);
	}

	void LayeredWindow::Paint()
	{
		if (_winBitmap && !_invalidateRect.IsEmptyArea()) {
			_winBitmap->Earse(_invalidateRect);//清除背景
			HDC winHDC = _winBitmap->GetHDC();

#if 0
			//不使用双缓冲
			DoPaint(winHDC, _invalidateRect);
#else
			//使用双缓冲
			Bitmap doubleBuff(_winBitmap->Width, _winBitmap->Height, Bitmap::PixelFormat::PixelFormatARGB);
			DoPaint(doubleBuff.GetHDC(), _invalidateRect);
			//使用BitBlt函数进行复制到winHDC  //如果窗体不规则 不适用于BitBlt进行复制
			::BitBlt(winHDC, _invalidateRect.X, _invalidateRect.Y,
				_invalidateRect.Width, _invalidateRect.Height,
				doubleBuff.GetHDC(), _invalidateRect.X, _invalidateRect.Y,
				SRCCOPY);
#endif
			PushDC(winHDC);//updatelaredwindow 更新窗口
			_invalidateRect = Rect();//重置区域
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
		::UpdateLayeredWindow(Hwnd(), NULL, NULL, &size, hdc, &point, 0, &blendFunc, ULW_ALPHA);//透明
	}
}