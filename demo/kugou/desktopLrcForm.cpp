#include "desktopLrcForm.h"
// 检测是否为 Windows 11 24H2 或更高版本
bool IsWindows11_24H2OrLater() {
	typedef LONG(WINAPI* RtlGetVersionPtr)(OSVERSIONINFOEXW*);
	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
	if (hMod) {
		RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");
		if (RtlGetVersion) {
			OSVERSIONINFOEXW osInfo = { 0 };
			osInfo.dwOSVersionInfoSize = sizeof(osInfo);
			if (RtlGetVersion(&osInfo) == 0) {
				// Windows 11 是 Build 22000+
				// 24H2 是 Build 26100+
				return osInfo.dwBuildNumber >= 26100;
			}
		}
	}
	return false;
}
HWND GetWorkerHWNDWin11()
{
	HWND progmanWnd = ::FindWindowW(L"Progman", L"Program Manager");
	::SendMessageW(progmanWnd, 0x052c, 0, 0);
	HWND hDefView = FindWindowExW(progmanWnd, 0, L"SHELLDLL_DefView", 0);
	if (hDefView != NULL) {
		HWND hWorkerw = FindWindowExW(progmanWnd, NULL, L"WorkerW", 0);
		return hWorkerw;
	}
	return NULL;
}
HWND GetWorkerHWNDWin10() {
	// 找到名为 "Progman" 的顶级窗口句柄，它的窗口标题是 "Program Manager"
	HWND progmanWnd = ::FindWindow(L"Progman", L"Program Manager");
	// 发送一个特殊的消息 (0x052C) 给找到的 "Progman" 窗口
	// 这条消息会使 Windows 创建一个新的 WorkerW 窗口
	DWORD_PTR dResult = 0;
	::SendMessageW(progmanWnd, 0x052c, 0, 0);
	// 声明一个变量来存储 WorkerW 窗口的句柄
	HWND hwndWorkW = NULL;
	// 使用一个 do-while 循环来查找 WorkerW 窗口
	int64_t startTime = ezui::GetTickCountMs();
	while (true)
	{
		if (ezui::GetTickCountMs() - startTime > 1500) {
			progmanWnd = NULL;// 超过 1.5 秒后仍未找到 WorkerW 窗口，设置 progmanWnd 为 NULL
			break; // 超过 1.5 秒后退出循环，避免死循环
		}
		ezui::SleepMs(10);
		// 查找下一个 WorkerW 窗口
		hwndWorkW = ::FindWindowEx(NULL, hwndWorkW, L"WorkerW", NULL);
		if (NULL == hwndWorkW) {
			continue;
		}
		// 在 WorkerW 窗口中查找 SHELLDLL_DefView 窗口
		HWND hView = ::FindWindowEx(hwndWorkW, NULL, L"SHELLDLL_DefView", NULL);
		if (NULL == hView) {
			continue;
		}
		// 如果找到了 SHELLDLL_DefView 窗口，则尝试关闭多余的 WorkerW 窗口
		HWND h = ::FindWindowEx(NULL, hwndWorkW, L"WorkerW", NULL);
		while (NULL != h) {
			SendMessage(h, WM_CLOSE, 0, 0);
			h = ::FindWindowEx(NULL, hwndWorkW, L"WorkerW", NULL);
		}
		break;
	}
	// 返回 "Progman" 窗口的句柄
	return progmanWnd;
}
HWND GetWorkerHWND() {
	HWND workWnd = NULL;
	if (!IsWindows11_24H2OrLater()) {
		workWnd = GetWorkerHWNDWin10();
	}
	if (workWnd == NULL) {
		workWnd = GetWorkerHWNDWin11();
	}
	return workWnd;
}

//修改窗口exstyle
void SetWindowExStyle(HWND hWnd) {
	LONG_PTR nExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
	nExStyle |= WS_EX_LAYERED;
	nExStyle |= WS_EX_COMPOSITED;
	SetWindowLongPtr(hWnd, GWL_EXSTYLE, nExStyle);
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

DesktopLrcFrm::DesktopLrcFrm(VlcPlayer* player) :_player(player), _lrc(NULL) {
	//关闭默认的窗口阴影
	this->CloseShadow();
	////获取桌面的窗口句柄
	HWND workWnd = GetWorkerHWND();
	//设置窗口为桌面窗口的子窗口
	SetWindowExStyle(GetWindowHandle());
	::SetParent(GetWindowHandle(), workWnd);

	//调整窗口大小和位置以覆盖整个桌面
	RECT workRect;
	::GetClientRect(workWnd, &workRect);
	::SetWindowPos(GetWindowHandle(), NULL, workRect.left, workRect.top, workRect.left + workRect.right, workRect.top + workRect.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
	//设置窗口布局显示歌词
	this->LoadXml("res/xml/desktopLrc.htm");
	_lrc = dynamic_cast<LrcPanel*>(this->GetLayout());

	//绑定主frame层的绘制回调 frame层是最底层,所以这样做没问题
	this->GetLayout()->AddEventHandler(Event::Paint, [this](void*, EventArgs* _args) {
		Image* img = NULL;
		_player->play_mtx.lock();
		PaintEventArgs* arg = (PaintEventArgs*)_args;
		if (_player->play_img) {
			img = _player->play_img->Clone();
		}
		_player->play_mtx.unlock();
		if (img) {
			//绘制视频的帧图像到窗口上
			img->SizeMode = ImageSizeMode::Cover;
			arg->Graphics()->DrawImage(img, this->GetClientRect());
			delete img;
		}
		});

}
void DesktopLrcFrm::LoadLrc(const UIString& lrcData) {
	_lrc->LoadLrc(lrcData);
}

void DesktopLrcFrm::ChangePostion(int postion) {
	_lrc->ChangePostion(postion);
}
