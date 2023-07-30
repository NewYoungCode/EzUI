#include "EzUI.h"
namespace EzUI {
	WCHAR WindowClassName[]{ L"EzUI_Window" };
	HZIP HZipResource = NULL;
	HGLOBAL HVSResource = NULL;
	size_t GetThreadId() {
		std::thread::id threadId = std::this_thread::get_id();
		return *(size_t*)&threadId;
	}

	std::mutex _resourceMtx;
	bool FindZipResource(const EString& fileName, int* index, size_t* fileSize) {
		if (HZipResource) {
			ZIPENTRY z;
			ZRESULT ret = FindZipItem(HZipResource, fileName.c_str(), false, index, &z);
			if (ret == 0 && z.unc_size != 0) {
				*fileSize = z.unc_size;
				return true;
			}
		}
		return false;
	}
	bool UnZipResource(const EString& fileName, std::string* outData) {
		std::unique_lock<std::mutex> autoLock(_resourceMtx);
		int index;
		size_t fileSize;
		if (FindZipResource(fileName, &index, &fileSize)) {
			outData->resize(fileSize);
			UnzipItem(HZipResource, index, (void*)(outData->c_str()), fileSize);
			return true;
		}
		return false;
	}

	void UnZip(const EString& zipFileName, const EString& outPath, const EString& password, std::function<void(int index, int fileCount)> callback) {
		auto hz = OpenZip(zipFileName.utf16().c_str(), password.empty() ? NULL : password.c_str());
		size_t count = 0;
		ZIPENTRY ze;
		GetZipItem(hz, -1, &ze);
		int numitems = ze.index;
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			EString outFile = outPath + "\\" + EString(ze.name);
			do
			{
				::DeleteFileW(outFile.utf16().c_str());
				if ((ze.attr & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
					::CreateDirectoryW(outFile.utf16().c_str(), NULL);
					break;
				}
				size_t fileSize = ze.unc_size;
				if (fileSize == 0)break;
				char* buf = new char[fileSize] {0};
				UnzipItem(hz, ze.index, (void*)buf, fileSize);
				std::ofstream ofs(outFile.utf16(), std::ios::binary);
				ofs.write(buf, fileSize);
				ofs.flush();
				ofs.close();
				delete[] buf;
			} while (false);
			count++;
			if (callback) {
				callback(i, numitems);
			}
		}
		CloseZipU(hz);
	}

	bool GetResource(const EString& filename, std::string* outFileData) {
		FILE* file(0);
		_wfopen_s(&file, filename.utf16().c_str(), L"rb");
		if (file) {
			std::ifstream ifs(file);
			std::stringstream ss;
			ss << ifs.rdbuf();
			*outFileData = ss.str();
			ifs.close();
			::fclose(file);
			return true;
		}
		else {
			return UnZipResource(filename, outFileData);
		}
	}
	size_t GetMonitors(std::list<MonitorInfo>* outMonitorInfo)
	{
		outMonitorInfo->clear();
		//// 枚举显示器回调函数
		::EnumDisplayMonitors(NULL, NULL, [](HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) ->BOOL {
			// 获取当前所有显示器的信息
			std::list<MonitorInfo>* monitors = (std::list<MonitorInfo>*)dwData;
			MonitorInfo mt;
			//逻辑宽高
			mt.Rect.X = lprcMonitor->left;
			mt.Rect.Y = lprcMonitor->top;
			mt.Rect.Width = lprcMonitor->right - lprcMonitor->left;
			mt.Rect.Height = lprcMonitor->bottom - lprcMonitor->top;
			//获取显示器信息
			MONITORINFOEX miex;
			miex.cbSize = sizeof(miex);
			::GetMonitorInfo(hMonitor, &miex);
			if ((miex.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY) {//是否为主显示器
				mt.Primary = true;
			}
			//获取工作区域 自动排除任务栏
			mt.WorkRect.X = miex.rcWork.left;
			mt.WorkRect.Y = miex.rcWork.top;
			mt.WorkRect.Width = miex.rcWork.right - miex.rcWork.left;
			mt.WorkRect.Height = miex.rcWork.bottom - miex.rcWork.top;
			//获取物理宽高
			DEVMODE dm;
			dm.dmSize = sizeof(dm);
			dm.dmDriverExtra = 0;
			::EnumDisplaySettings(miex.szDevice, ENUM_REGISTRY_SETTINGS, &dm);
			mt.Physical.Width = dm.dmPelsWidth;//物理宽
			mt.Physical.Height = dm.dmPelsHeight;//物理高
			//计算缩放
			mt.Scale = ((float)mt.Physical.Height / (float)mt.Rect.Height);
			//显示器fps
			mt.FPS = (float)dm.dmDisplayFrequency;
			monitors->push_back(mt);
			return TRUE;
			}, LPARAM(outMonitorInfo));
		return outMonitorInfo->size();
	}

	HCURSOR LoadCursor(Cursor cursorType)
	{
		return ::LoadCursorW(NULL, (LPTSTR)cursorType);
	}
	HCURSOR LoadCursor(const EString& fileName)
	{
		return ::LoadCursorFromFileW(fileName.utf16().c_str());
	}
	void FreeCursor(HCURSOR hCursor)
	{
		::DestroyCursor(hCursor);
	}

	EBitmap::EBitmap(WORD width, WORD height, PixelFormat piexlFormat) {//默认24位不透明位图
		biteCount = (byte)piexlFormat;
		this->Width = width;
		this->Height = height;
		memset(&bmi, 0, sizeof(bmi));
		BITMAPINFOHEADER& bmih = bmi.bmiHeader;
		bmih.biSize = sizeof(bmih);
		bmih.biBitCount = biteCount;
		bmih.biCompression = BI_RGB;
		bmih.biPlanes = 1;
		bmih.biWidth = width;
		bmih.biHeight = -height;
		bmih.biSizeImage = width * height * biteCount;
		_bitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &point, NULL, 0);
		this->GetDC();
	}

	void EBitmap::SetPixel(int x, int y, const Color& color) {
		DWORD* point = (DWORD*)this->point + (x + y * this->Width);//起始地址+坐标偏移	
		if (biteCount == 32) { //argb
			((BYTE*)point)[3] = color.GetA();//修改A通道数值
		}
		((BYTE*)point)[2] = color.GetR();//修改R通道数值
		((BYTE*)point)[1] = color.GetG();//修改G通道数值
		((BYTE*)point)[0] = color.GetB();//修改B通道数值
	}
	Color EBitmap::GetPixel(int x, int y) {
		DWORD* point = (DWORD*)this->point + (x + y * this->Width);//起始地址+坐标偏移
		BYTE a = 255, r, g, b;
		if (biteCount == 32) { //argb
			a = ((BYTE*)point)[3];
		}
		r = ((BYTE*)point)[2];//修改R通道数值
		g = ((BYTE*)point)[1];//修改G通道数值
		b = ((BYTE*)point)[0];//修改B通道数值
		return Color(a, r, g, b);
	}

	void EBitmap::Earse(const Rect& _rect) {
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
			rect.Height = this->Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = this->Width - rect.X;
		}
		for (int y = rect.Y; y < rect.GetBottom(); y++)
		{
			DWORD* point = (DWORD*)this->point + (rect.X + y * this->Width);//起始地址+坐标偏移
			::memset(point, 0, rect.Width * 4);//抹除
		}
	}
	void EBitmap::FillRect(const Rect& _rect, const Color& color) {
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
			rect.Height = this->Height - rect.Y;
		}
		if (rect.GetRight() > Width) {
			rect.Width = this->Width - rect.X;
		}

		BYTE a = color.GetA();
		BYTE r = color.GetR();
		BYTE g = color.GetG();
		BYTE b = color.GetB();
		DWORD c1 = 0;
		BYTE* c = (BYTE*)&c1;
		c[3] = a;
		c[2] = r;
		c[1] = g;
		c[0] = b;

		for (int y = rect.Y; y < rect.GetBottom(); y++)
		{
			DWORD* point = (DWORD*)this->point + (rect.X + y * this->Width);//起始地址+坐标偏移
			::memset(point, 255, rect.Width * 4);//抹除
		}
	}

	HDC& EBitmap::GetDC() {
		if (!_hdc) {
			_hdc = ::CreateCompatibleDC(NULL);
			::SelectObject(_hdc, _bitmap);
		}
		return _hdc;
	}
	EBitmap::~EBitmap() {
		if (_hdc) {
			::DeleteDC(_hdc);
			::DeleteBitmap(_bitmap);
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
				style->BackColor = Convert::StringToColor(value);
				break;
			}
			if (key == "background-image") {
				value = value.Erase('"');//删除双引号;
				style->BackImage = Image::FromFile(value);
				if (callback) {
					callback(style->BackImage);
				}
				break;
			}
			if (key == "fore-image") {
				value = value.Erase('"');//删除双引号;
				style->ForeImage = Image::FromFile(value);
				if (callback) {
					callback(style->ForeImage);
				}
				break;
			}
			if (key == "border-color") {
				style->Border.Color = Convert::StringToColor(value);
				break;
			}
			if (key == "color" || key == "fore-color") {
				style->ForeColor = Convert::StringToColor(value);
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
				style->FontFamily = value;
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
	IControl::~IControl() {

	}

	void IControl::SetAttribute(const EString& attrName, const EString& attrValue) {
		AttributeIterator itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			(*itor).second = attrValue;
		}
		else {
			_attrs.insert(std::pair<EString, EString>(attrName, attrValue));
		}
	}
	EString IControl::GetAttribute(const EString& attrName) {
		AttributeIterator itor = _attrs.find(attrName);
		if (itor != _attrs.end()) {
			return (*itor).second;
		}
		return "";
	}

};