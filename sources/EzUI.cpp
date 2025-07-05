#include "EzUI.h"
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"Uuid.lib")
#pragma comment(lib, "comctl32.lib")
//#pragma comment(lib,"odbc32.lib")
//#pragma comment(lib,"odbccp32.lib")
#pragma comment(lib,"imm32.lib")
#pragma comment(lib, "Msimg32.lib")
//#pragma comment(lib,"Shcore.lib")
namespace ezui {

	WCHAR __EzUI__WindowClassName[]{ L"EzUI_Window" };
	HMODULE __EzUI__HINSTANCE = NULL;
	Resource* __EzUI__Resource = NULL;
	DWORD __EzUI__ThreadId = NULL;
	std::list<HWND> __EzUI__Wnds;
	HWND __EzUI_MessageWnd = NULL;
	const std::list<ezui::MonitorInfo> __EzUI__MonitorInfos;

	Color Color::Make(const UIString& colorStr) {
		if (colorStr == "black") {
			return Color::Black;
		}
		else if (colorStr == "white") {
			return Color::White;
		}
		else if (colorStr == "gray") {
			return Color::Gray;
		}
		else if (colorStr == "lightgray") {
			return Color::LightGray;
		}
		else if (colorStr == "darkgray") {
			return Color::DarkGray;
		}
		else if (colorStr == "red") {
			return Color::Red;
		}
		else if (colorStr == "darkred") {
			return Color::DarkRed;
		}
		else if (colorStr == "lightcoral") {
			return Color::LightCoral;
		}
		else if (colorStr == "tomato") {
			return Color::Tomato;
		}
		else if (colorStr == "crimson") {
			return Color::Crimson;
		}
		else if (colorStr == "green" || colorStr == "lime") {
			return Color::Green;
		}
		else if (colorStr == "darkgreen") {
			return Color::DarkGreen;
		}
		else if (colorStr == "lawngreen") {
			return Color::LawnGreen;
		}
		else if (colorStr == "palegreen") {
			return Color::PaleGreen;
		}
		else if (colorStr == "blue") {
			return Color::Blue;
		}
		else if (colorStr == "royalblue") {
			return Color::RoyalBlue;
		}
		else if (colorStr == "dodgerblue") {
			return Color::DodgerBlue;
		}
		else if (colorStr == "deepskyblue") {
			return Color::DeepSkyBlue;
		}
		else if (colorStr == "lightblue") {
			return Color::LightBlue;
		}
		else if (colorStr == "yellow") {
			return Color::Yellow;
		}
		else if (colorStr == "gold") {
			return Color::Gold;
		}
		else if (colorStr == "lightyellow") {
			return Color::LightYellow;
		}
		else if (colorStr == "khaki") {
			return Color::Khaki;
		}
		else if (colorStr == "orange") {
			return Color::Orange;
		}
		else if (colorStr == "darkorange") {
			return Color::DarkOrange;
		}
		else if (colorStr == "coral") {
			return Color::Coral;
		}
		else if (colorStr == "salmon") {
			return Color::Salmon;
		}
		else if (colorStr == "purple") {
			return Color::Purple;
		}
		else if (colorStr == "mediumpurple") {
			return Color::MediumPurple;
		}
		else if (colorStr == "indigo") {
			return Color::Indigo;
		}
		else if (colorStr == "violet") {
			return Color::Violet;
		}
		else if (colorStr == "plum") {
			return Color::Plum;
		}
		else if (colorStr == "cyan" || colorStr == "aqua") {
			return Color::Cyan;
		}
		else if (colorStr == "teal") {
			return Color::Teal;
		}
		else if (colorStr == "turquoise") {
			return Color::Turquoise;
		}
		else if (colorStr == "brown") {
			return Color::Brown;
		}
		else if (colorStr == "maroon") {
			return Color::Maroon;
		}
		else if (colorStr == "tan") {
			return Color::Tan;
		}
		else if (colorStr == "beige") {
			return Color::Beige;
		}
		else if (colorStr == "navy") {
			return Color::Navy;
		}
		else if (colorStr == "olive") {
			return Color::Olive;
		}
		else if (colorStr == "silver") {
			return Color::Silver;
		}
		else if (colorStr == "transparent") {
			return Color::Transparent;
		}
		if (colorStr.find("#") == 0) { //"#4e6ef2"
			auto rStr = colorStr.substr(1, 2);
			auto gStr = colorStr.substr(3, 2);
			auto bStr = colorStr.substr(5, 2);
			DWORD r, g, b;
			sscanf_s(rStr.c_str(), "%x", &r);
			sscanf_s(gStr.c_str(), "%x", &g);
			sscanf_s(bStr.c_str(), "%x", &b);
			//Argb = MakeARGB(255, r, g, b);
			return Color((BYTE)r, (BYTE)g, (BYTE)b);
		}
		if (colorStr.find("rgb") == 0) { //"rgb(255,100,2,3)"
			size_t pos1 = colorStr.find("(");
			size_t pos2 = colorStr.rfind(")");
			UIString rgbStr = colorStr.substr(pos1 + 1, pos2 - pos1 - 1);
			auto rgbList = rgbStr.split(",");
			BYTE r, g, b;
			r = std::stoi(rgbList.at(0));
			g = std::stoi(rgbList.at(1));
			b = std::stoi(rgbList.at(2));
			BYTE a = 255;
			//考虑到rgba
			if (rgbList.size() > 3) {
				std::string aStr = rgbList.at(3);
				if (aStr.find(".") != std::string::npos) {
					//浮点型0~1
					a = (BYTE)(255 * std::atof(aStr.c_str()) + 0.5);
				}
				else {
					//整数型0~255
					a = std::stoi(aStr.c_str());
				}
			}
			return Color(r, g, b, a);
		}
		return Color();
	}

	Image* Image::Make(const UIString& fileOrRes) {
		//本地文件中获取
		std::wstring wstr = fileOrRes.unicode();
		DWORD dwAttr = GetFileAttributesW(wstr.c_str());
		if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
			return new Image(wstr);
		}
		//从资源中获取
		if (ezui::__EzUI__Resource) {
			std::string data;
			ezui::__EzUI__Resource->GetFile(fileOrRes, &data);
			if (data.empty()) {
				return NULL;
			}
			return new Image(data.c_str(), data.size());
		}
		return NULL;
	}

	void InstallFont(const UIString& fontFileName) {
		auto ret = ::AddFontResourceW(fontFileName.unicode().c_str());
		::SystemParametersInfoW(SPI_SETNONCLIENTMETRICS, 0, nullptr, SPIF_SENDCHANGE);//刷新
	}

	void UnstallFont(const UIString& fontFileName) {
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

	bool GetResource(const UIString& filename, std::string* out) {
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
				ok = ezui::__EzUI__Resource->GetFile(filename, out);
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
	HCURSOR LoadCursor(const UIString& fileName)
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

	void IControl::SetAttribute(const UIString& attrName, const UIString& attrValue) {
		auto itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			(*itor).second = attrValue;
		}
		else {
			_attrs.insert(std::pair<UIString, UIString>(attrName, attrValue));
		}
	}

	UIString IControl::GetAttribute(const UIString& attrName) {
		auto itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			return (*itor).second;
		}
		return "";
	}

	const std::map<UIString, UIString>& IControl::GetAttributes() {
		return _attrs;
	}

	void IControl::RemoveAttribute(const UIString& attrName)
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
