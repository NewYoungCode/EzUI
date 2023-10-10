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

	namespace Base {
		WCHAR WindowClassName[]{ L"EzUI_Window" };
		HZIP HZipResource = NULL;
		HGLOBAL HVSResource = NULL;
		const std::list<EzUI::MonitorInfo> MonitorInfos;
		std::mutex ResourceMtx;
	};

	bool CopyToClipboard(int uFormat, void* pData, size_t size, HWND hWnd) {
		//打开剪贴板
		bool ret = OpenClipboard(hWnd);
		if (!ret)return ret;
		//清空剪贴板
		::EmptyClipboard();
		//为剪切板申请内存
		HGLOBAL clip = ::GlobalAlloc(GMEM_DDESHARE, size);
		memcpy((void*)clip, pData, size);
		//解锁
		ret = ::GlobalUnlock(clip);
		ret = ::SetClipboardData(uFormat, clip);
		ret = ::CloseClipboard();
		return ret;
	}

	bool GetClipboardData(int uFormat, std::function<void(void*, size_t)> Callback, HWND hWnd) {
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

	bool FindZipResource(const EString& fileName, int* index, size_t* fileSize) {
		if (Base::HZipResource) {
			ZIPENTRY z;
			ZRESULT ret = FindZipItem(Base::HZipResource, fileName.c_str(), false, index, &z);
			if (ret == 0 && z.unc_size != 0) {
				*fileSize = z.unc_size;
				return true;
			}
		}
		return false;
	}
	bool UnZipResource(const EString& fileName, std::string* outData) {
		std::unique_lock<std::mutex> autoLock(Base::ResourceMtx);
		int index;
		size_t fileSize;
		if (FindZipResource(fileName, &index, &fileSize)) {
			outData->resize(fileSize);
			UnzipItem(Base::HZipResource, index, (void*)(outData->c_str()), fileSize);
			return true;
		}
		return false;
	}
	///--------------------------
	Ziper::Ziper(const EString& fileName, const EString& password) {
		_hZip = OpenZip(fileName.unicode().c_str(), password.empty() ? NULL : password.c_str());
		ASSERT(_hZip);
		GetZipItem(_hZip, -1, &_ze);
		_numitems = _ze.index;
	}
	Ziper::Ziper(void* pData, size_t len, const EString& password) {
		_hZip = OpenZip(pData, len, password.empty() ? NULL : password.c_str());
		ASSERT(_hZip);
		GetZipItem(_hZip, -1, &_ze);
		_numitems = _ze.index;
	}
	size_t Ziper::GetCount() {
		return _numitems;
	}
	bool Ziper::Find(const EString& fileName, ZIPENTRY* outZe) {
		int index = 0;
		ZRESULT ret = FindZipItem(this->_hZip, fileName.c_str(), false, &index, outZe);
		if (ret == 0 && outZe->unc_size != 0) {
			return true;
		}
		return false;
	}
	void Ziper::UnZip(const ZIPENTRY& ze, void** pData) {
		*pData = new char[ze.unc_size] { 0 };
		UnzipItem(Base::HZipResource, ze.index, *pData, ze.unc_size);
	}
	void Ziper::UnZip(std::function<bool(int index, const EString& fileName, void* pData, size_t len, DWORD fileAttribute)> callback) {
		ZIPENTRY ze;
		for (int i = 0; i < _numitems; i++)
		{
			GetZipItem(_hZip, i, &ze);
			size_t fileSize = ze.unc_size;
			char* buf = NULL;
			if (fileSize != 0) {
				buf = new char[fileSize] {0};
				UnzipItem(_hZip, ze.index, (void*)buf, fileSize);
			}
			bool continue_ = callback(i, ze.name, buf, fileSize, ze.attr);
			if (buf) {
				delete[] buf;
			}
			if (!continue_) {
				break;
			}
		}
	}
	Ziper::~Ziper() {
		if (_hZip) {
			CloseZipU(_hZip);
		}
	}
	///--------------------------

	bool GetResource(const EString& filename, std::string* out) {
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
			return true;
		}
		else {
			return UnZipResource(filename, out);
		}
	}

#undef GetMonitorInfo
	void GetMonitorInfo(MonitorInfo& mt, HMONITOR hMonitor) {
		//获取显示器信息
		MONITORINFOEX infoEx;
		infoEx.cbSize = sizeof(infoEx);
		::GetMonitorInfoW(hMonitor, &infoEx);
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

	size_t GetMonitors(std::list<MonitorInfo>* outMonitorInfo)
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

	void ControlStyle::Scale(const float& scale)
	{
		this->FontSize = this->FontSize * scale + 0.5;
		this->Border.Scale(scale);
		if (BackImage) {
			BackImage->Margin.Scale(scale);
		}
		if (ForeImage) {
			ForeImage->Margin.Scale(scale);
		}
	}
	void ControlStyle::SetStyleSheet(const EString& styleStr, const std::function<void(Image*)>& callback)
	{
		auto attrs = styleStr.Split(";");
		for (auto& it : attrs) {
			size_t pos = it.find(":");
			if (pos == -1)continue;
			EString key = it.substr(0, pos);
			EString value = it.substr(pos + 1);
			this->SetStyle(key, value, callback);
		}
	}
	void ControlStyle::SetStyle(const EString& key, const EString& _value, const std::function<void(Image*)>& callback)
	{
		EString& value = (EString&)_value;
		ControlStyle* style = this;
		do
		{
			if (key == "cursor") {
				if (value == "pointer") {
					style->Cursor = LoadCursor(EzUI::Cursor::HAND);
				}
				else if (value == "help") {
					style->Cursor = LoadCursor(EzUI::Cursor::HELP);
				}
				else if (value == "n-resize") {
					//南北箭头 纵向
					style->Cursor = LoadCursor(EzUI::Cursor::SIZENS);
				}
				else if (value == "e-resize") {
					//东西箭头 水平
					style->Cursor = LoadCursor(EzUI::Cursor::SIZEWE);
				}
				else if (value == "move") {
					//四个方向的箭头都有
					style->Cursor = LoadCursor(EzUI::Cursor::SIZEALL);
				}
				break;
			}
			if (key == "background-color") {
				style->BackColor = Color::Make(value);
				break;
			}
			if (key == "background-image") {
				value = value.Erase('"');//删除双引号;
				style->BackImage = Image::Make(value);
				if (callback) {
					callback(style->BackImage);
				}
				break;
			}
			if (key == "fore-image") {
				value = value.Erase('"');//删除双引号;
				style->ForeImage = Image::Make(value);
				if (callback) {
					callback(style->ForeImage);
				}
				break;
			}
			if (key == "border-color") {
				style->Border.Color = Color::Make(value);
				break;
			}
			if (key == "color" || key == "fore-color") {
				style->ForeColor = Color::Make(value);
				break;
			}
			if (key == "border-radius") {
				style->Border.Radius = std::stoi(value);
				break;
			}
			if (key == "border-top-left-radius") {
				style->Border.TopLeftRadius = std::stoi(value);
				break;
			}
			if (key == "border-top-right-radius") {
				style->Border.TopRightRadius = std::stoi(value);
				break;
			}
			if (key == "border-bottom-right-radius") {
				style->Border.BottomRightRadius = std::stoi(value);
				break;
			}
			if (key == "border-bottom-left-radius") {
				style->Border.BottomLeftRadius = std::stoi(value);
				break;
			}
			if (key == "font-size") {
				style->FontSize = std::stoi(value);
				break;
			}
			if (key == "font-family") {
				value = value.Erase('"');//删除双引号;
				style->FontFamily = value.unicode();
				break;
			}
			if (key == "border") {
				auto width = std::stoi(value);
				style->Border.Left = width;
				style->Border.Top = width;
				style->Border.Right = width;
				style->Border.Bottom = width;
				break;
			}
			if (key == "border-left") {
				style->Border.Left = std::stoi(value);
				break;
			}
			if (key == "border-top") {
				style->Border.Top = std::stoi(value);
				break;
			}
			if (key == "border-right") {
				style->Border.Right = std::stoi(value);
				break;
			}
			if (key == "border-bottom") {
				style->Border.Bottom = std::stoi(value);
				break;
			}
		} while (false);
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
