#include "EzUI.h"
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Uuid.lib")
#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib,"odbc32.lib")
//#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"imm32.lib")
#pragma comment(lib, "Msimg32.lib")
//#pragma comment(lib,"Shcore.lib")
namespace EzUI {

	WCHAR __EzUI__WindowClassName[]{ L"EzUI_Window" };
	HMODULE __EzUI__HINSTANCE = NULL;
	Resource* __EzUI__Resource = NULL;
	DWORD __EzUI__ThreadId = NULL;
	std::list<HWND> __EzUI__WNDS;
	const std::list<EzUI::MonitorInfo> __EzUI__MonitorInfos;

	void InstallFont(const EString& fontFileName) {
		auto ret = ::AddFontResourceW(fontFileName.unicode().c_str());
		::SystemParametersInfoW(SPI_SETNONCLIENTMETRICS, 0, nullptr, SPIF_SENDCHANGE);//刷新
	}

	void UnstallFont(const EString& fontFileName) {
		auto ret = ::RemoveFontResourceW(fontFileName.unicode().c_str());
		::SystemParametersInfoW(SPI_SETNONCLIENTMETRICS, 0, nullptr, SPIF_SENDCHANGE);//刷新
	}

	bool CopyToClipboard(int_t uFormat, void* pData, size_t size, HWND hWnd) {
		//打开剪贴板
		bool ret = ::OpenClipboard(hWnd);
		if (!ret)return ret;
		//清空剪贴板
		::EmptyClipboard();
		//为剪切板申请内存
		HGLOBAL clip = ::GlobalAlloc(GMEM_DDESHARE, size);
		::memcpy((void*)clip, pData, size);
		//解锁
		ret = ::GlobalUnlock(clip);
		ret = ::SetClipboardData(uFormat, clip);
		ret = ::CloseClipboard();
		return ret;
	}

	bool GetClipboardData(int_t uFormat, std::function<void(void*, size_t)> Callback, HWND hWnd) {
		//只接收当前类型
		bool ret = ::IsClipboardFormatAvailable(uFormat);
		if (!ret)return ret;
		//打开剪贴版
		ret = OpenClipboard(hWnd);
		if (!ret)return ret;
		//获取剪贴板数据
		HANDLE hClipboard = ::GetClipboardData(uFormat);
		size_t dataSize = ::GlobalSize(hClipboard);
		void* pData = ::GlobalLock(hClipboard);
		if (Callback) {
			Callback(pData, dataSize);
		}
		//解锁
		ret = ::GlobalUnlock(hClipboard);
		ret = ::CloseClipboard();
		return ret;
	}

	bool CopyToClipboard(const std::wstring& str, HWND hWnd) {
		return CopyToClipboard(CF_UNICODETEXT, (void*)str.c_str(), (str.size() + 1) * 2, hWnd);
	}
	bool GetClipboardData(std::wstring* outStr, HWND hWnd) {
		bool ret = GetClipboardData(CF_UNICODETEXT, [=](void* data, size_t _sz) {
			wchar_t* wstr = (wchar_t*)data;
			size_t sz = _sz / 2;
			outStr->clear();
			outStr->append(wstr, wstr[sz - 1] == 0 ? sz - 1 : sz);
			}, hWnd);
		return ret;
	}

	bool GetResource(const EString& filename, std::string* out) {
		bool ok = false;
		do
		{
			FILE* file(0);
			_wfopen_s(&file, filename.unicode().c_str(), L"rb");
			if (file) {
				std::ifstream fs(file);
				fs.seekg(0, std::ios_base::end);
				auto size = fs.tellg();
				fs.seekg(0);
				out->resize(static_cast<size_t>(size));
				fs.read((char*)out->c_str(), static_cast<std::streamsize>(size));
				fs.close();
				::fclose(file);
				ok = true;
				break;
			}
			if (__EzUI__Resource) {
				ok = EzUI::__EzUI__Resource->GetFile(filename, out);
			}
		} while (false);
		return ok;
	}

#undef GetMonitorInfo
	void GetMonitorInfo(MonitorInfo& mt, HMONITOR hMonitor) {
		//获取显示器信息
		MONITORINFOEX infoEx;
		infoEx.cbSize = sizeof(infoEx);
		::GetMonitorInfoW(hMonitor, &infoEx);
		mt.Monitor = hMonitor;
		//逻辑宽高
		mt.Rect.X = infoEx.rcMonitor.left;
		mt.Rect.Y = infoEx.rcMonitor.top;
		mt.Rect.Width = infoEx.rcMonitor.right - infoEx.rcMonitor.left;
		mt.Rect.Height = infoEx.rcMonitor.bottom - infoEx.rcMonitor.top;
		if ((infoEx.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY) {//是否为主显示器
			mt.Primary = true;
		}
		//获取工作区域 自动排除任务栏
		mt.WorkRect.X = infoEx.rcWork.left;
		mt.WorkRect.Y = infoEx.rcWork.top;
		mt.WorkRect.Width = infoEx.rcWork.right - infoEx.rcWork.left;
		mt.WorkRect.Height = infoEx.rcWork.bottom - infoEx.rcWork.top;
		//获取物理宽高
		DEVMODE dm;
		dm.dmSize = sizeof(dm);
		dm.dmDriverExtra = 0;
		::EnumDisplaySettings(infoEx.szDevice, ENUM_REGISTRY_SETTINGS, &dm);
		mt.Physical.Width = dm.dmPelsWidth;//物理宽
		mt.Physical.Height = dm.dmPelsHeight;//物理高
		//计算缩放
		mt.Scale = ((float)mt.Physical.Height / (float)mt.Rect.Height);
		//显示器fps
		mt.FPS = (float)dm.dmDisplayFrequency;
	}

	size_t GetMonitor(std::list<MonitorInfo>* outMonitorInfo)
	{
		outMonitorInfo->clear();
		//// 枚举显示器回调函数
		::EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) ->BOOL {
			// 获取当前所有显示器的信息
			std::list<MonitorInfo>* monitors = (std::list<MonitorInfo>*)dwData;
			MonitorInfo mt;
			GetMonitorInfo(mt, hMonitor);
			monitors->push_back(mt);
			return TRUE;
			}, LPARAM(outMonitorInfo));
		return outMonitorInfo->size();
	}

	void GetMontior(MonitorInfo* monitorInfo, HWND hWnd)
	{
		POINT cursorPos;
		HMONITOR hMonitor;
		if (hWnd) {
			hMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		}
		else {
			// 确定包含鼠标位置的屏幕
			GetCursorPos(&cursorPos);
			hMonitor = ::MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
		}
		// 获取屏幕信息
		GetMonitorInfo(*monitorInfo, hMonitor);
	}

	void GetMontior(MonitorInfo* monitorInfo, const Rect& rect)
	{
		RECT r = rect.ToRECT();
		HMONITOR hMonitor = ::MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST);
		// 获取屏幕信息
		GetMonitorInfo(*monitorInfo, hMonitor);
	}

	HCURSOR LoadCursor(Cursor cursorType)
	{
		return ::LoadCursorW(NULL, (LPTSTR)cursorType);
	}
	HCURSOR LoadCursor(const EString& fileName)
	{
		return ::LoadCursorFromFileW(fileName.unicode().c_str());
	}
	void FreeCursor(HCURSOR hCursor)
	{
		::DestroyCursor(hCursor);
	}

	void ControlStyle::Scale(float scale)
	{
		this->FontSize = this->FontSize * scale + 0.5;
		this->Border.Scale(scale);
	}

	IControl::IControl() {}
	IControl::~IControl() {}

	void IControl::SetAttribute(const EString& attrName, const EString& attrValue) {
		auto itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			(*itor).second = attrValue;
		}
		else {
			_attrs.insert(std::pair<EString, EString>(attrName, attrValue));
		}
	}

	EString IControl::GetAttribute(const EString& attrName) {
		auto itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			return (*itor).second;
		}
		return "";
	}

	const std::map<EString, EString>& IControl::GetAttributes() {
		return _attrs;
	}

	void IControl::RemoveAttribute(const EString& attrName)
	{
		auto itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			_attrs.erase(itor);
		}
	}

	void PaintEventArgs::PushLayer(const Geometry& dxGeometry) {
		this->Graphics.PushLayer(dxGeometry);
		layers.push_back(false);
	}
	void PaintEventArgs::PushLayer(const Rect& rectBounds) {
		this->Graphics.PushAxisAlignedClip(rectBounds);
		layers.push_back(true);
	}
	void PaintEventArgs::PopLayer() {
		if (layers.size() > 0) {
			if (*layers.rbegin() == true) {
				this->Graphics.PopAxisAlignedClip();
			}
			else {
				this->Graphics.PopLayer();
			}
			layers.pop_back();
		}
	}
	void PaintEventArgs::PushOffset(const Point& offset)
	{
		this->Graphics.SetTransform(offset.X, offset.Y);
		this->offsets.push_back(offset);
	}
	void PaintEventArgs::PopOffset()
	{
		this->offsets.pop_back();
		if (this->offsets.size() > 0) {
			auto& offset = *this->offsets.rbegin();
			this->Graphics.SetTransform(offset.X, offset.Y);
		}
	}
};
