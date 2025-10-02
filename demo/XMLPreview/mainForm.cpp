#include "mainForm.h"
bool ReadFile(const UIString& filename, std::string* outFileStream) {
	outFileStream->clear();
	std::ifstream ifs(filename.unicode().c_str(), std::ios::binary);
	if (!ifs.is_open()) {
		return false;
	}
	ifs.seekg(0, std::ios::end);
	std::streamsize size = ifs.tellg();
	if (size == std::streamsize(-1)) {
		return false;
	}
	outFileStream->resize(size);
	ifs.seekg(0);
	ifs.read((char*)outFileStream->c_str(), size);
	return ifs.good();
}
void SetClientSize(HWND hWnd, int clientWidth, int clientHeight)
{
	RECT rc = { 0, 0, clientWidth, clientHeight };

	// 先换算客户区 -> 窗口整体大小
	DWORD style = GetWindowLong(hWnd, GWL_STYLE);
	DWORD exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	AdjustWindowRectEx(&rc, style, FALSE, exStyle);

	int winWidth = rc.right - rc.left;
	int winHeight = rc.bottom - rc.top;

	// 设置窗口大小（保持位置不变）
	SetWindowPos(hWnd, NULL, 0, 0, winWidth, winHeight,
		SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}
// 写字符串到注册表
bool WriteRegistryString(const std::wstring& valueName, const std::wstring& data) {
	HKEY hKey;
	LONG lRes = RegCreateKeyExW(
		HKEY_CURRENT_USER,
		L"Software\\EzUI\\EzUI_Preview",   // 你的程序路径
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE,
		NULL,
		&hKey,
		NULL
	);

	if (lRes != ERROR_SUCCESS) {
		return false;
	}

	lRes = RegSetValueExW(
		hKey,
		valueName.c_str(),
		0,
		REG_SZ,
		reinterpret_cast<const BYTE*>(data.c_str()),
		static_cast<DWORD>((data.size() + 1) * sizeof(wchar_t))
	);

	RegCloseKey(hKey);
	return (lRes == ERROR_SUCCESS);
}
// 读字符串
std::wstring ReadRegistryString(const std::wstring& valueName, const std::wstring& defaultValue = L"") {
	HKEY hKey;
	LONG lRes = RegOpenKeyExW(
		HKEY_CURRENT_USER,
		L"Software\\EzUI\\EzUI_Preview",
		0,
		KEY_READ,
		&hKey
	);

	if (lRes != ERROR_SUCCESS) {
		return defaultValue;
	}

	wchar_t buffer[512]; // 可改成动态分配
	DWORD bufferSize = sizeof(buffer);
	DWORD type = 0;

	lRes = RegGetValueW(
		hKey,
		NULL,
		valueName.c_str(),
		RRF_RT_REG_SZ,
		&type,
		buffer,
		&bufferSize
	);

	RegCloseKey(hKey);

	if (lRes == ERROR_SUCCESS) {
		return std::wstring(buffer);
	}
	else {
		return defaultValue;
	}
}

class PreviewForm;

UIString g_workDir;
std::vector<PreviewForm*> g_wnds;

//预览窗口
class PreviewForm :public Window {
	UIString m_path;
	Task* task = NULL;
	std::string fileData;
	bool exit = false;
	std::string m_workDir;
public:
	PreviewForm(const UIString& path) :Window(0, 0) {
		LONG style = GetWindowLong(Hwnd(), GWL_STYLE);
		style &= ~WS_POPUP;      // 去掉 popup 样式
		style |= WS_CHILD;       // 加上 child 样式
		SetWindowLong(Hwnd(), GWL_STYLE, style);
		this->m_path = path;
		task = new Task([this]() {
			while (!exit)
			{
				bool reLoad = false;//是否需要重新加载
				if (m_workDir != g_workDir) {
					m_workDir = g_workDir;//工作目录修改了
					reLoad = true;
				}
				std::string data;
				if (ReadFile(m_path, &data) && data != fileData) {
					fileData = data;//文件被修改了
					reLoad = true;
				}
				if (reLoad) {
					OutputDebugStringW(UIString(L"重新载入: %s\n").format(m_path.c_str()).unicode().c_str());
					Invoke([this]() {
						//this->SetText(m_path);
						this->LoadXml(m_path);
						auto* mainLayout = this->GetLayout();
						if (mainLayout) {
							int width = 0, height = 0;
							auto wStr = mainLayout->GetAttribute("width");
							auto hStr = mainLayout->GetAttribute("height");
							if (!wStr.empty()) {
								width = std::atoi(wStr.c_str());
							}
							if (!hStr.empty()) {
								height = std::atoi(hStr.c_str());
							}
							auto sizeStr = mainLayout->GetAttribute("size");
							auto strs = sizeStr.split(",");
							if (strs.size() == 2) {
								width = std::atoi(strs[0].c_str());
								height = std::atoi(strs[1].c_str());
							}
							if (width) {
								SetClientSize(this->Hwnd(), width, Height());
							}
							if (height) {
								SetClientSize(this->Hwnd(), Width(), height);
							}
						}
						this->Invalidate();
						});
				}
				Sleep(100);
			}
			});


	}
	virtual ~PreviewForm() {
		exit = true;
		if (task) {
			delete task;
		}
	}
	void OnClose(bool& b) {
		auto itor = std::find(g_wnds.begin(), g_wnds.end(), this);
		if (itor != g_wnds.end()) {
			g_wnds.erase(itor);
		}
		this->DeleteLater();
	}
	void OnSize(const Size& sz) {
		__super::OnSize(sz);
		UIString title = UIString("%s w:%d h:%d").format(this->m_path.c_str(), this->GetClientRect().Width, this->GetClientRect().Height);
		this->SetText(title);
	}
	void OnPaint(PaintEventArgs& args) {
		__super::OnPaint(args);
	}
};

class LabelX :public Label {
public:
	LabelX() {};
	virtual ~LabelX() {}
	void OnForePaint(PaintEventArgs& args)override {
		std::wstring drawText(this->GetText().unicode() + L": " + std::to_wstring(Height()));
		std::wstring fontFamily = GetFontFamily();
		int fontSize = GetFontSize();
		int fontWidght = GetFontWeight();
		if (fontSize == 0)return;
		Font font(fontFamily, fontSize, fontWidght);
		args.Graphics.SetFont(font);
		args.Graphics.SetColor(GetForeColor());
		args.Graphics.DrawString(drawText, RectF(0, 0, Width(), Height()), TextAlign::MiddleCenter);
	}
};

MainFrm::MainFrm(int cx, int cy) :Window(cx, cy) {

	ezui::RegisterControl<LabelX>("LabelX");

	this->SetText(L"EzUI_XMLPreview");
	hbox.SetFixedHeight(50);
	hbox.Style.BackColor = Color(249, 249, 249);
	hbox.Style.Border.Bottom = 1;
	hbox.Style.Border.Color = Color(255, 215, 215);
	hbox.Style.Border.Style = StrokeStyle::Solid;
	main.AddChild(&hbox);
	SetLayout(&main);

	Label* tips2 = new Label(&hbox);
	tips2->SetText(L"请将xml界面文件拖拽到此处。");
	tips2->Style.FontSize = 15;
	main.AddChild(tips2);

	auto* lb = new Label;
	lb->SetMarginLeft(10);
	lb->SetAutoSize(true);
	lb->SetText(L"工作目录: ");
	hbox.AddChild(lb);

	auto* edit = new TextBox(this);
	edit->Name = "edit";
	edit->SetFixedSize({ 350,35 });
	edit->SetMarginLeft(10);
	edit->Style.Border = 1;
	edit->Style.Border.Color = Color(215, 215, 215);
	edit->Style.Border.Style = StrokeStyle::Solid;
	edit->Style.BackColor = Color(253, 253, 253);
	hbox.AddChild(edit);

	auto* btn = new Button(this);
	btn->SetFixedSize({ 100,35 });
	btn->SetMarginLeft(10);
	btn->SetText(L"应用");
	btn->Style.Border = 1;
	btn->Style.Border.Color = Color(220, 220, 220);
	btn->Style.Border.Style = StrokeStyle::Solid;
	btn->Style.Border.Radius = 5;
	btn->HoverStyle.BackColor = Color(224, 238, 249);
	btn->HoverStyle.Border.Color = Color(151, 199, 237);
	btn->Name = "btnApplay";
	hbox.AddChild(btn);

	auto* tips = new Label(this);
	tips->SetAutoSize(true);
	tips->Name = "tips";
	tips->SetMarginLeft(10);
	tips->SetVisible(false);
	hbox.AddChild(tips);

	UIString dir = ReadRegistryString(L"workDir");
	edit->SetText(dir);
	SetWorkDir(dir);

	//this->CreatePreview("d:/aa.htm");
}
void MainFrm::SetWorkDir(const UIString& dir) {
	BOOL ret = ::SetCurrentDirectoryW(dir.unicode().c_str());
	Label* tips = (Label*)this->FindControl("tips");
	if (ret) {
		g_workDir = dir;
		WriteRegistryString(L"workDir", dir.unicode());
		tips->SetText(L"设置工作目录成功!");
		tips->Style.ForeColor = Color::Green;
	}
	else {
		tips->SetText(L"设置工作目录失败!");
		tips->Style.ForeColor = Color::Red;
	}
	tips->SetVisible(true);
	tips->Invalidate();
	Timer::Timeout(2000, [=]() {
		Invoke([=]() {
			tips->SetVisible(false);
			tips->Invalidate();
			});
		});

}
void MainFrm::CreatePreview(const UIString& xmlFile)
{
	auto* wnd = new PreviewForm(xmlFile);
	g_wnds.push_back(wnd);
	::SetParent(wnd->Hwnd(), Hwnd());
	int width = Width() * 0.5;
	int height = Height() * 0.5;
	int x = (Width() - width) / 2;
	int y = (Height() - height) / 2;
	wnd->SetRect(Rect(x, y, width, height));
	wnd->Show();

}
void MainFrm::OnNotify(Control* sd, EventArgs& args) {
	if (args.EventType == Event::OnMouseDown) {
		if (sd->Name == "btnApplay") {
			auto* edit = (TextBox*)this->FindControl("edit");
			SetWorkDir(edit->GetText());
		}
	}
	__super::OnNotify(sd, args);
}
LRESULT MainFrm::WndProc(UINT msg, WPARAM wp, LPARAM lp) {
	if (msg == WM_DROPFILES) {
		HDROP hDrop = (HDROP)wp;
		UINT numFiles = ::DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);  //  获取拖入的文件数量
		TCHAR szFilePath[MAX_PATH]{ 0 };
		::DragQueryFileW(hDrop, 0, szFilePath, sizeof(szFilePath));  //  获取第一个文件路径
		UIString file = szFilePath;
		this->CreatePreview(file);
	}
	return __super::WndProc(msg, wp, lp);
}
void MainFrm::OnSize(const Size& sz) {
	__super::OnSize(sz);
}
void MainFrm::OnClose(bool& b) {
	for (auto& it : g_wnds) {
		delete it;
	}
	Application::Exit(0);
}
MainFrm::~MainFrm() {

}
