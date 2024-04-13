#include "EzUI/Application.h"
#include "EzUI/VLayout.h"
#include "EzUI/TextBox.h"
#include "EzUI/Button.h"
#include "EzUI/Window.h"
#include "EzUI/Resource.h"
#include "EzUI/Task.h"
#include "EzUI/HLayout.h"
#include "EzUI/UIManager.h"
using namespace EzUI;

class MainFrm :public Window {
	Task* task = NULL;
	UIManager ui;
	//ѡ�
	TabLayout* tab;

	//��һҳ�Ŀؼ�
	//Ҫ�����Ŀ¼
	TextBox* editPackDir;
	//���֮��Ҫ������ļ���
	TextBox* editPackName;
	//��ʼ����İ�ť
	Button* btnSatrtPackage;
	//��ʾ�ı�
	Label* labelTips;
	Label* labelTipsErr;

	//�ڶ�ҳ�Ŀؼ�
	TextBox* editResFile;
	Button* btnBrowserFile;
	VList* listFiles;
	Button* btnUnPackage;

	//��Դָ��
	Resource* res = NULL;
public:
	void Init();
	MainFrm(const EString& cmdLine);
	void OnPackDirChange();
	void OnClose(bool& close)  override;
	bool FileExists(const EString& fileName);
	bool OnNotify(Control* sd, EventArgs& args)override;
	void OnResFileChange(EzUI::EString& resFile);
	virtual LRESULT WndProc(UINT msg, WPARAM wp, LPARAM lp);
	virtual ~MainFrm();
};

inline bool FileExists(const EString& filename) {
	DWORD dwAttr = GetFileAttributesW(filename.unicode().c_str());
	if (dwAttr == DWORD(-1)) {
		return false;
	}
	if (dwAttr & FILE_ATTRIBUTE_ARCHIVE) {
		return true;
	}
	return false;

}
inline bool PathExist(const EString& dir) {
	DWORD dwAttr = GetFileAttributesW(dir.unicode().c_str());
	if (dwAttr == DWORD(-1)) {
		return false;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
	return false;
}
inline bool CreatePath(const EString& path) {
	::CreateDirectoryW(path.unicode().c_str(), NULL);
	if (PathExist(path)) {
		return true;
	}
	//�����༶Ŀ¼
	if (path.find(":") != size_t(-1)) {
		EString dir = path + "/";
		dir = dir.Replace("\\", "/");
		dir = dir.Replace("//", "/");
		std::vector<std::string> arr = dir.Split("/");
		EString root;
		if (arr.size() > 0) {
			root += arr[0] + "/";
			for (size_t i = 1; i < arr.size(); i++)
			{
				if (arr[i].empty()) {
					continue;
				}
				root += arr[i] + "/";
				if (!PathExist(root)) {
					::CreateDirectoryW(root.unicode().c_str(), NULL);
				}
			}
		}
	}
	return PathExist(path);
}

inline EString ShowFileDialog(HWND ownerWnd, const EString& defaultPath = "", const EString& title = "") {
	OPENFILENAMEW ofn;       // ���ļ��Ի���ṹ��
	WCHAR szFile[512]{ 0 };       // ѡ����ļ���
	// ��ʼ��OPENFILENAME�ṹ��
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.hwndOwner = ownerWnd;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	// ��ʾ�ļ��Ի���
	if (GetOpenFileNameW(&ofn) == TRUE) {
		return szFile;
	}
	return szFile;
}
#include <ShlObj.h>
inline EString ShowFolderDialog(HWND ownerWnd, const EString& defaultPath = "", const EString& title = "") {
	WCHAR selectedPath[MAX_PATH]{ 0 };
	BROWSEINFOW browseInfo{ 0 };
	browseInfo.hwndOwner = ownerWnd;
	browseInfo.pszDisplayName = selectedPath;
	auto wTitle = title.unicode();
	browseInfo.lpszTitle = wTitle.c_str();
	//���ø�Ŀ¼
	LPITEMIDLIST pidlRoot;
	::SHParseDisplayName(defaultPath.unicode().c_str(), NULL, &pidlRoot, 0, NULL);
	browseInfo.pidlRoot = pidlRoot;
	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST itemIdList = SHBrowseForFolderW(&browseInfo);
	if (itemIdList != nullptr) {
		SHGetPathFromIDListW(itemIdList, selectedPath);//����·��
		CoTaskMemFree(itemIdList);//����
		return selectedPath;
	}
	return selectedPath;
}
inline std::string GetFileSize(__int64 _KEY_FILE_SIZE) {
	std::string ext;
	std::string disp_size;
	long double KEY_FILE_SIZE = _KEY_FILE_SIZE;
	if (KEY_FILE_SIZE > 1024) {
		KEY_FILE_SIZE = KEY_FILE_SIZE / 1024.0f;  // kb
		ext = "KB";
		if (KEY_FILE_SIZE > 1024) {
			KEY_FILE_SIZE = KEY_FILE_SIZE / 1024.0f;  // mb
			ext = "MB";
			if (KEY_FILE_SIZE > 1024) {
				KEY_FILE_SIZE = KEY_FILE_SIZE / 1024.0f;  // gb
				ext = "GB";
			}
		}
	}
	else {
		ext = "BT";
	}
	disp_size = EString::ToString(KEY_FILE_SIZE, 2) + " " + ext;
	return disp_size;
}

class FileItem :public HBox {
	Label name;
	Label size;
public:
	FileItem(const EString& fileName, size_t fileSize) {
		this->SetFixedHeight(25);
		this->SetDockStyle(DockStyle::Horizontal);

		name.TextAlign = TextAlign::MiddleLeft;
		name.SetText(" " + fileName);
		name.Ellipsis = "...";
		this->Add(&name);

		size.SetFixedWidth(100);
		size.SetText(GetFileSize(fileSize));
		this->Add(&size);

		name.EventPassThrough = size.EventPassThrough = Event::OnHover | Event::OnActive | Event::OnMouseDoubleClick;

		this->HoverStyle.BackColor = Color(100, 100, 100, 50);
		this->Style.FontSize = 13;
		this->ActiveStyle.FontSize = 14;
	}
};
