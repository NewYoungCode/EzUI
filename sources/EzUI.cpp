#include "EzUI.h"
#include "Bitmap.h"
#include "Control.h"
#include "IFrame.h"
#include "Window.h"
#include "TabLayout.h"
#include <gdiplus.h>
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib,"imm32.lib")
namespace ezui {

	HMODULE __EzUI__HINSTANCE = NULL;
	Resource* __EzUI__Resource = NULL;
	DWORD __EzUI__ThreadId = NULL;
	HWND __EzUI_MessageWnd = NULL;
	const std::list<ezui::MonitorInfo> __EzUI__MonitorInfos;

	Color Color::Make(const UIString& colorStr, bool* _isGood) {
		Color color;
		bool _bCopy;
		bool* isGood = _isGood ? _isGood : &_bCopy;
		*isGood = false;
		do {
			if (colorStr == "black") {
				color = Color::Black; *isGood = true; break;
			}
			else if (colorStr == "white") {
				color = Color::White; *isGood = true; break;
			}
			else if (colorStr == "gray") {
				color = Color::Gray; *isGood = true; break;
			}
			else if (colorStr == "lightgray") {
				color = Color::LightGray; *isGood = true; break;
			}
			else if (colorStr == "darkgray") {
				color = Color::DarkGray; *isGood = true; break;
			}
			else if (colorStr == "red") {
				color = Color::Red; *isGood = true; break;
			}
			else if (colorStr == "darkred") {
				color = Color::DarkRed; *isGood = true; break;
			}
			else if (colorStr == "lightcoral") {
				color = Color::LightCoral; *isGood = true; break;
			}
			else if (colorStr == "tomato") {
				color = Color::Tomato; *isGood = true; break;
			}
			else if (colorStr == "crimson") {
				color = Color::Crimson; *isGood = true; break;
			}
			else if (colorStr == "green" || colorStr == "lime") {
				color = Color::Green; *isGood = true; break;
			}
			else if (colorStr == "darkgreen") {
				color = Color::DarkGreen; *isGood = true; break;
			}
			else if (colorStr == "lawngreen") {
				color = Color::LawnGreen; *isGood = true; break;
			}
			else if (colorStr == "palegreen") {
				color = Color::PaleGreen; *isGood = true; break;
			}
			else if (colorStr == "blue") {
				color = Color::Blue; *isGood = true; break;
			}
			else if (colorStr == "royalblue") {
				color = Color::RoyalBlue; *isGood = true; break;
			}
			else if (colorStr == "dodgerblue") {
				color = Color::DodgerBlue; *isGood = true; break;
			}
			else if (colorStr == "deepskyblue") {
				color = Color::DeepSkyBlue; *isGood = true; break;
			}
			else if (colorStr == "lightblue") {
				color = Color::LightBlue; *isGood = true; break;
			}
			else if (colorStr == "yellow") {
				color = Color::Yellow; *isGood = true; break;
			}
			else if (colorStr == "gold") {
				color = Color::Gold; *isGood = true; break;
			}
			else if (colorStr == "lightyellow") {
				color = Color::LightYellow; *isGood = true; break;
			}
			else if (colorStr == "khaki") {
				color = Color::Khaki; *isGood = true; break;
			}
			else if (colorStr == "orange") {
				color = Color::Orange; *isGood = true; break;
			}
			else if (colorStr == "darkorange") {
				color = Color::DarkOrange; *isGood = true; break;
			}
			else if (colorStr == "coral") {
				color = Color::Coral; *isGood = true; break;
			}
			else if (colorStr == "salmon") {
				color = Color::Salmon; *isGood = true; break;
			}
			else if (colorStr == "purple") {
				color = Color::Purple; *isGood = true; break;
			}
			else if (colorStr == "mediumpurple") {
				color = Color::MediumPurple; *isGood = true; break;
			}
			else if (colorStr == "indigo") {
				color = Color::Indigo; *isGood = true; break;
			}
			else if (colorStr == "violet") {
				color = Color::Violet; *isGood = true; break;
			}
			else if (colorStr == "plum") {
				color = Color::Plum; *isGood = true; break;
			}
			else if (colorStr == "cyan" || colorStr == "aqua") {
				color = Color::Cyan; *isGood = true; break;
			}
			else if (colorStr == "teal") {
				color = Color::Teal; *isGood = true; break;
			}
			else if (colorStr == "turquoise") {
				color = Color::Turquoise; *isGood = true; break;
			}
			else if (colorStr == "brown") {
				color = Color::Brown; *isGood = true; break;
			}
			else if (colorStr == "maroon") {
				color = Color::Maroon; *isGood = true; break;
			}
			else if (colorStr == "tan") {
				color = Color::Tan; *isGood = true; break;
			}
			else if (colorStr == "beige") {
				color = Color::Beige; *isGood = true; break;
			}
			else if (colorStr == "navy") {
				color = Color::Navy; *isGood = true; break;
			}
			else if (colorStr == "olive") {
				color = Color::Olive; *isGood = true; break;
			}
			else if (colorStr == "silver") {
				color = Color::Silver; *isGood = true; break;
			}
			else if (colorStr == "transparent") {
				color = Color::Transparent; *isGood = true; break;
			}
			else if (colorStr.find("#") == 0 && colorStr.size() == 7) { //"#4e6ef2"
				auto rStr = colorStr.substr(1, 2);
				auto gStr = colorStr.substr(3, 2);
				auto bStr = colorStr.substr(5, 2);
				DWORD r, g, b;
				sscanf_s(rStr.c_str(), "%x", &r);
				sscanf_s(gStr.c_str(), "%x", &g);
				sscanf_s(bStr.c_str(), "%x", &b);
				color = Color((BYTE)r, (BYTE)g, (BYTE)b);
				*isGood = true; break;
			}
			else if (colorStr.find("rgb") == 0) { //"rgb(255,255,255,50%)"
				size_t pos1 = colorStr.find("(");
				size_t pos2 = colorStr.rfind(")");
				if (pos1 == std::string::npos || pos2 == std::string::npos) {
					break;//非标准rgb格式
				}
				UIString rgbStr = colorStr.substr(pos1 + 1, pos2 - pos1 - 1);
				auto rgbList = rgbStr.replace(' ', ',').split(",");
				if (rgbList.size() < 3) {
					break;//非标准rgb格式
				}
				BYTE r = std::stoi(rgbList.at(0));
				BYTE g = std::stoi(rgbList.at(1));
				BYTE b = std::stoi(rgbList.at(2));
				BYTE a = 255;
				if (rgbList.size() > 3) { //如果有设置透明度
					std::string aStr = rgbList.at(3);
					if (aStr.rfind("%") != std::string::npos) {
						a = (BYTE)(std::atof(aStr.c_str()) / 100.0f * 255 + 0.5);//计算出透明度转为byte
					}
					else {
						a = std::stoi(aStr.c_str());//0~255描述透明度
					}
				}
				color = Color(r, g, b, a);
				*isGood = true; break;
			}
		} while (false);
		return color;
	}

	Image::Image(Bitmap* bitmap) :DXImage(bitmap->GetHBITMAP())
	{
	}

	Image* Image::Make(const UIString& fileOrRes) {
		//本地文件中获取
		std::wstring wstr = fileOrRes.unicode();
		DWORD dwAttr = GetFileAttributesW(wstr.c_str());
		if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
			Image* img = new Image(wstr);
#ifdef _DEBUG
			if (img) {
				img->m_path = fileOrRes;
			}
#endif
			return img;
		}
		//从资源中获取
		if (ezui::__EzUI__Resource) {
			std::string data;
			if (!ezui::__EzUI__Resource->GetFile(fileOrRes, &data) || data.empty()) {
				return NULL;
			}
			Image* img = new Image(data.c_str(), data.size());
#ifdef _DEBUG
			if (img) {
				img->m_path = fileOrRes;
			}
#endif
			return img;
		}
		return NULL;
	}

	bool IsFloatZero(float num)
	{
		return std::abs(num) <= EZUI_FLOAT_EPSILON;
	}

	bool IsFloatEqual(float num1, float num2)
	{
		return std::fabsf(num1 - num2) <= EZUI_FLOAT_EPSILON;
	}

	HICON LoadIcon(const UIString& fileName)
	{
		std::string fileData;
		if (!GetResource(fileName, &fileData)) {
			return NULL;
		}

		const char* pData = fileData.c_str();
		size_t size = fileData.size();

		HICON hIcon = NULL;
		HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, size);
		void* pMem = ::GlobalLock(hMem);
		::memcpy(pMem, pData, size);
		::GlobalUnlock(hMem);
		IStream* pStream = NULL;
		if (::CreateStreamOnHGlobal(hMem, TRUE, &pStream) != S_OK) {
			return NULL;
		}
		Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromStream(pStream);
		if (bmp && bmp->GetHICON(&hIcon) != Gdiplus::Ok) {
			hIcon = NULL;
		}
		delete bmp;
		pStream->Release();
		return hIcon;
	}

	void InstallFont(const UIString& fontFileName) {
		auto ret = ::AddFontResourceW(fontFileName.unicode().c_str());
		::SystemParametersInfoW(SPI_SETNONCLIENTMETRICS, 0, NULL, SPIF_SENDCHANGE);//刷新
	}

	void UnstallFont(const UIString& fontFileName) {
		auto ret = ::RemoveFontResourceW(fontFileName.unicode().c_str());
		::SystemParametersInfoW(SPI_SETNONCLIENTMETRICS, 0, NULL, SPIF_SENDCHANGE);//刷新
	}

	bool CopyToClipboard(int uFormat, void* pData, size_t size, HWND hWnd) {
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

	bool GetResource(const UIString& filename, std::string* outFileData)
	{
		outFileData->clear();
		//本地文件中获取
		std::wstring wstr = filename.unicode();
		DWORD dwAttr = GetFileAttributesW(wstr.c_str());
		if (dwAttr && (dwAttr != -1) && (dwAttr & FILE_ATTRIBUTE_ARCHIVE)) {
			std::ifstream ifs(wstr, std::ios::binary);
			ifs.seekg(0, std::ios::end);
			auto size = ifs.tellg();
			outFileData->resize(size);
			ifs.seekg(0);
			ifs.read((char*)outFileData->c_str(), size);
			ifs.close();
			return true;
		}
		//从资源中获取
		if (ezui::__EzUI__Resource) {
			if (ezui::__EzUI__Resource->GetFile(filename, outFileData)) {
				return true;
			}
		}
		return false;
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
	bool DefaultNotify(Control* sender, EventArgs& args) {
		WindowData* winData = NULL;
		if (!sender || !(winData = sender->GetPublicData())) {
			return false;
		}
		auto* win = winData->Window;

		switch (args.EventType)
		{
		case Event::OnMouseDoubleClick: {
			if (sender->Action == ControlAction::Title) {
				if (::IsZoomed(win->Hwnd())) {
					win->ShowNormal();
				}
				else {
					win->ShowMaximized();
				}
			}
			break;
		}
		case Event::OnMouseDown: {
			if (sender->Action == ControlAction::MoveWindow) {
				winData->MoveWindow();
				break;
			}
			if (sender->Action == ControlAction::Title) {
				winData->TitleMoveWindow();
				break;
			}
			if (sender->Action == ControlAction::Mini) {
				win->ShowMinimized();
				break;
			}
			if (sender->Action == ControlAction::Max) {
				if (!win->IsMaximized()) {
					win->ShowMaximized();
				}
				else {
					win->ShowNormal();
				}
				break;
			}
			if (sender->Action == ControlAction::Close) {
				win->Close();
				break;
			}
			UIString tabName = sender->GetAttribute("tablayout");
			if (!tabName.empty()) {
				auto ctls = sender->Parent->FindControl("tablayout", tabName);
				IFrame* frame = sender->GetFrame();
				TabLayout* tabLayout = dynamic_cast<TabLayout*>(frame ? frame->FindControl(tabName) : win->FindControl(tabName));
				if (tabLayout && sender->Parent) {
					int pos = 0;
					for (auto& it : ctls)
					{
						if (it == sender) {
							tabLayout->SlideToPage(pos);
							tabLayout->Invalidate();
							break;
						}
						++pos;
					}
				}
			}
			break;
		}
		default:
			break;
		}
		return false;
	}

	void ControlStyle::Scale(float scale)
	{
		this->FontSize = this->FontSize * scale + 0.5;
		this->Border.Scale(scale);

		if (this->BackImage) {
			this->BackImage->DrawPosition.Scale(scale);
			this->BackImage->DrawSize.Scale(scale);
		}
		if (this->ForeImage) {
			this->ForeImage->DrawPosition.Scale(scale);
			this->ForeImage->DrawSize.Scale(scale);
		}
	}

	Object::Object(Object* parentObject)
	{
		if (parentObject) {
			parentObject->Attach(this);
		}
	}
	Object::~Object() {
	}

	void Object::SetAttribute(const UIString& attrName, const UIString& attrValue) {
		auto itor = m_attrs.find(attrName);
		if (itor != m_attrs.end()) {
			(*itor).second = attrValue;
		}
		else {
			m_attrs.insert(std::pair<UIString, UIString>(attrName, attrValue));
		}
	}

	UIString Object::GetAttribute(const UIString& attrName) {
		auto itor = m_attrs.find(attrName);
		if (itor != m_attrs.end()) {
			return (*itor).second;
		}
		return "";
	}

	const std::map<UIString, UIString>& Object::GetAttributes() {
		return m_attrs;
	}

	void Object::RemoveAttribute(const UIString& attrName)
	{
		auto itor = m_attrs.find(attrName);
		if (itor != m_attrs.end()) {
			m_attrs.erase(itor);
		}
	}

	Object* Object::Attach(Object* obj)
	{
		m_childObjects.Add(obj);
		return obj;
	}

	void Object::Detach(Object* obj)
	{
		m_childObjects.Remove(obj);
	}

	void PaintEventArgs::PushLayer(const Geometry& dxGeometry) {
		this->Graphics.PushLayer(dxGeometry);
		m_layers.push_back(false);
	}
	void PaintEventArgs::PushLayer(const Rect& rectBounds) {
		this->Graphics.PushAxisAlignedClip(rectBounds);
		m_layers.push_back(true);
	}
	void PaintEventArgs::PopLayer() {
		if (m_layers.size() > 0) {
			if (*m_layers.rbegin() == true) {
				this->Graphics.PopAxisAlignedClip();
			}
			else {
				this->Graphics.PopLayer();
			}
			m_layers.pop_back();
		}
	}
	void PaintEventArgs::PushOffset(const Point& offset)
	{
		this->Graphics.SetTransform(offset.X, offset.Y);
		this->m_offsets.push_back(offset);
	}
	void PaintEventArgs::PopOffset()
	{
		this->m_offsets.pop_back();
		if (this->m_offsets.size() > 0) {
			auto& offset = *this->m_offsets.rbegin();
			this->Graphics.SetTransform(offset.X, offset.Y);
		}
	}
};
