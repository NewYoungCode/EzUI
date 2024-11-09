#include "DesktopLrcFrm.h"

HWND DesktopLrcFrm::GetDeskTopWnd() {
	int result;
	HWND windowHandle = ::FindWindowW(L"Progman", NULL);
	::SendMessageTimeoutW(windowHandle, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, (PDWORD_PTR)&result);
	::EnumWindows([](HWND tophandle, LPARAM lParam)->BOOL {
		HWND defview = ::FindWindowExW(tophandle, 0, L"SHELLDLL_DefView", NULL);
		if (defview != NULL)
		{
			HWND workerw = ::FindWindowExW(0, tophandle, L"WorkerW", 0);
			::ShowWindow(workerw, SW_HIDE);
		}
		return true;
		}, NULL);
	return windowHandle;
}

DesktopLrcFrm::DesktopLrcFrm(VlcPlayer* player) :_player(player), LayeredWindow(0, 0) {
	//关闭默认的窗口阴影
	this->CloseShadowBox();
	//获取当前屏幕的大小
	std::list<MonitorInfo> monitorInfo;
	GetMonitor(&monitorInfo);
	const MonitorInfo& def = *monitorInfo.begin();
	this->SetRect(def.Rect);
	//获取桌面的窗口句柄
	HWND workWnd = GetDeskTopWnd();
	::SetParent(Hwnd(), workWnd);
	::SetWindowPos(Hwnd(), NULL, def.Rect.X, def.Rect.Y, def.Rect.Width, def.Rect.Height, SWP_NOZORDER | SWP_NOACTIVATE);
	//设置窗口布局
	_lrc.Style.FontSize = 20;
	_lrc.Style.ForeColor = Color::White;
	this->SetLayout(&_lrc);
}
void DesktopLrcFrm::LoadLrc(const EString& lrcData) {
	_lrc.LoadLrc(lrcData);
}

void DesktopLrcFrm::ChangePostion(int postion) {
	_lrc.ChangePostion(postion);
}

void DesktopLrcFrm::OnPaint(PaintEventArgs& arg) {
	//绘制视频的帧图像到窗口上
	if (_player->BuffBitmap) {
		Image img(_player->BuffBitmap->GetHBITMAP());
		img.SizeMode = ImageSizeMode::CenterImage;
		arg.Graphics.DrawImage(&img, this->GetClientRect());
		arg.Graphics.SetColor(Color(0, 0, 0, 100));
		arg.Graphics.FillRectangle(this->GetClientRect());
	}
	__super::OnPaint(arg);
}
