#include "mainFrom.h"
#include "FileSystem.h"

const WCHAR* xml = L"<vbox id=\"main\"> <hbox height=\"40\">  <radiobutton class=\"btnTab\" tablayout=\"tab\" checked=\"true\" text=\"打包\" width=\"100\"></radiobutton>  <radiobutton class=\"btnTab\" tablayout=\"tab\" text=\"解包\" width=\"100\"></radiobutton> </hbox> <tablayout id=\"tab\">  <hbox id=\"page1\">   <spacer width=\"10\"></spacer>   <vbox>    <spacer height=\"10\"></spacer>    <label height=\"30\" halign=\"left\" text=\"请选择你要打包的目录 :\"></label>    <hbox height=\"30\">  <textbox class=\"edit\" id=\"editPackDir\"></textbox> <spacer width=\"10\"></spacer><button class=\"btn\" id=\"btnBrowserDir\" width=\"100\" text=\"浏览\"></button>   </hbox>    <label halign=\"left\" style=\"color:#ff0000\" id=\"labelTipsErr\" height=\"20\"></label>    <spacer></spacer>    <label height=\"30\" halign=\"left\" text=\"请选择输出目录 :\"></label>    <hbox height=\"30\">  <textbox class=\"edit\" id=\"editPackName\"></textbox> <spacer width=\"10\"></spacer><button class=\"btn\" id=\"btnSatrtPackage\" width=\"100\" text=\"开始打包\"></button>   </hbox>    <spacer></spacer>    <!-- <hbox height=\"30\">  <spacer></spacer> <button class=\"btn\" id=\"btnSatrtPackage\" width=\"100\" text=\"开始\"></button> <spacer></spacer></hbox>-->    <spacer height=\"10\"></spacer>   </vbox>   <spacer width=\"10\"></spacer>  </hbox>  <hbox id=\"page2\">   <spacer width=\"10\"></spacer>   <vbox>    <spacer height=\"10\"></spacer>    <label height=\"30\" halign=\"left\" text=\"请选择你要预览的文件 :\"></label>    <hbox height=\"30\">  <textbox class=\"edit\" readonly=\"true\" id=\"editResFile\"></textbox> <spacer width=\"10\"></spacer><button class=\"btn\" id=\"btnBrowserFile\" width=\"100\" text=\"浏览\"></button><spacer width=\"10\"></spacer><button class=\"btn\" height=\"30\" id=\"btnUnPackage\" width=\"100\" text=\"解压至...\"></button>   </hbox>    <spacer height=\"10\"></spacer>    <vlist id=\"listFiles\" scrollbar=\"fileScrollbar\" style=\"background-color:rgba(175, 106, 106, 0.5)\"></vlist>   </vbox>   <spacer width=\"10\"></spacer>  </hbox> </tablayout> <hbox margin=\"0,10\" height=\"30\">  <label id=\"labelTips\" text=\"技术支持 718987717@qq.com/19980103ly@gmail.com\"></label> </hbox></vbox><style> .btn {  border-radius: 5;  border: 1;  border-color: #D0D0D0;  background-color: #FDFDFD;  font-size: 13; }  .btn:hover {   border-color: #0078D4;   background-color: #E0EEF9;  }  .btn:active {   font-size: 14;  } .edit {  border: 1;  border-radius: 2;  border-color: #808080; } #tab {  background-color: #F0F0F0; } .btnTab:checked {  background-color: #F0F0F0; } .btnTab:hover {  cursor: pointer; } #fileScrollbar {  border-radius: 5;  background-color: rgba(50,50,50,0.5);  fore-color: rgba(200,200,200,0.5); } #fileScrollbar:active {  fore-color: rgba(200,200,200,0.8); }</style>";

void MainFrm::Init() {
	this->SetText(L"EzUI资源打包器");
	//ui.LoadXmlFile("main.html");
	ui.LoadXmlData(xml);
	ui.SetupUI(this);
	//第一页的控件
	this->tab = (TabLayout*)this->FindControl("tab");
	this->editPackDir = (TextBox*)this->FindControl("editPackDir");
	this->editPackName = (TextBox*)this->FindControl("editPackName");
	this->btnSatrtPackage = (Button*)this->FindControl("btnSatrtPackage");
	this->labelTipsErr = (Label*)this->FindControl("labelTipsErr");
	this->labelTips = (Label*)this->FindControl("labelTips");
	this->editPackDir->TextChanged = [=](const UIString text)->void {
		this->OnPackDirChange();
		};

	//第二页的控件
	this->editResFile = (TextBox*)this->FindControl("editResFile");
	this->btnBrowserFile = (Button*)this->FindControl("btnBrowserFile");
	this->listFiles = (VListView*)this->FindControl("listFiles");
	this->btnUnPackage = (Button*)this->FindControl("btnUnPackage");
}

MainFrm::MainFrm(const UIString& cmdLine) :Window(600, 400) {
	Init();
	editPackDir->SetText(cmdLine);
	OnPackDirChange();
}

void MainFrm::OnPackDirChange()
{
	UIString dir = editPackDir->GetText();
	if (dir.empty() || !PathExist(dir)) {
		editPackName->SetText("");
		editPackName->Invalidate();
		labelTipsErr->SetText(L"打包目录无效!");
		labelTipsErr->Invalidate();
		return;
	}
	else {
		labelTipsErr->SetText("");
		labelTipsErr->Invalidate();
	}

	ui_text::Replace(&dir, "\"", "");
	ui_text::Replace(&dir, "\\", "/");
	ui_text::Replace(&dir, "//", "/");
	if (dir[dir.size() - 1] == '/') {
		dir.erase(dir.size() - 1, 1);
	}
	UIString resDir = dir;
	UIString rootDir;
	size_t pos = dir.rfind('/');
	UIString dirName;
	if (pos != size_t(-1)) {
		rootDir = dir.substr(0, pos);
		dirName = dir.substr(pos + 1);
	}
	UIString resFile = rootDir + "/" + dirName + ".bin";
	editPackName->SetText(resFile);
	editPackName->Invalidate();
}
void MainFrm::OnClose(bool& close) {
	Application::Exit(0);
}
bool MainFrm::FileExists(const UIString& fileName) {
	DWORD dwAttr = GetFileAttributesW(fileName.unicode().c_str());
	if (dwAttr == DWORD(-1)) {
		return false;
	}
	if (dwAttr & FILE_ATTRIBUTE_ARCHIVE) {
		return true;
	}
	return false;
}
bool MainFrm::OnNotify(Control* sd, EventArgs& args) {
	if (args.EventType == Event::OnMouseClick) {
		if (sd->Name == "btnBrowserDir") {
			UIString dir = ShowFolderDialog(Hwnd(), "", "");
			if (!dir.empty()) {
				this->editPackDir->SetText(dir);
				this->editPackDir->Invalidate();
				this->OnPackDirChange();
			}
		}
		if (sd->Name == "btnSatrtPackage") {
			do
			{
				UIString resDir = editPackDir->GetText();
				UIString resFile = editPackName->GetText();

				if (task && !task->IsStopped()) {
					::MessageBoxW(Hwnd(), L"请等待上次任务完成!", L"失败", 0);
					break;
				}

				if (FileExists(resFile) && ::DeleteFileW(resFile.unicode().c_str()) == FALSE) {
					::MessageBoxW(Hwnd(), L"文件已存在且无法覆盖!", L"失败", 0);
					break;
				}

				if (task) {
					delete task;
					task = NULL;
				}

				if (resFile.empty()) {
					::MessageBoxW(Hwnd(), L"打包文件路径不正确!", L"失败", 0);
					break;
				}

				labelTips->SetText(L"正在计算...");
				labelTips->Invalidate();

				task = new Task([resDir, resFile, this]() {
					Resource::Package(resDir, resFile, [=](const UIString& file, int index, int count) {
						Invoke([&]() {
							int rate = (index + 1) * 1.0f / count * 100 + 0.5;
							labelTips->SetText(UIString("(" + std::to_string(rate) + "%)") + UIString(L"正在打包\"") + file + "\"");
							labelTips->Invalidate();
							});
						Sleep(2);
						});

					Invoke([&]() {
						labelTips->SetText(L"打包成功!");
						labelTips->Invalidate();
						::MessageBoxW(Hwnd(), L"打包成功!", L"成功", 0);
						});
					});

			} while (false);
		}
		if (sd->Name == "btnBrowserFile") {
			UIString resFile = ShowFileDialog(Hwnd());
			OnResFileChange(resFile);
		}
		if (sd->Name == "btnUnPackage") {
			UIString resDir = ShowFolderDialog(Hwnd());
			if (!resDir.empty() && PathExist(resDir)) {
				for (auto& it : this->res->Items) {
					UIString fileName = resDir + "/" + it.Name;
					Directory::Create(Path::GetDirectoryName(fileName));
					File::Delete(fileName);
					UIString data;
					this->res->GetFile(it, &data);
					File::Write(data.c_str(), data.size(), fileName);
				}
				::MessageBoxW(Hwnd(), L"解压完成!", L"", 0);
			}
		}
	}
	return __super::OnNotify(sd, args);
}
void MainFrm::OnResFileChange(UIString& resFile)
{
	do
	{
		if (FileExists(resFile)) {
			Resource* newRes = new Resource(resFile);
			if (!newRes->IsGood()) {
				::MessageBoxW(Hwnd(), L"不是标准的资源文件", L"错误", 0);
				delete newRes;
				break;
			}
			if (res) {
				delete res;
				res = NULL;
			}
			res = newRes;

			listFiles->Clear(true);
			for (auto& item : res->Items) {
				FileItem* fileItem = new FileItem(item.Name, item.Size);
				listFiles->Add(fileItem);
			}
			listFiles->Invalidate();
			this->editResFile->SetText(resFile);
			this->editResFile->Invalidate();
		}
	} while (false);
}
LRESULT MainFrm::WndProc(UINT msg, WPARAM wp, LPARAM lp) {
	//准备做一个解压的功能
	if (msg == WM_DROPFILES) {
		HDROP hDrop = (HDROP)wp;
		UINT numFiles = ::DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);  //  获取拖入的文件数量
		TCHAR szFilePath[MAX_PATH]{ 0 };
		::DragQueryFileW(hDrop, 0, szFilePath, sizeof(szFilePath));  //  获取第一个文件路径
		UIString file = szFilePath;

		if (tab->GetPageIndex() == 0) {
			//打包
			if (PathExist(file)) {
				this->editPackDir->SetText(file);
				this->editPackDir->Invalidate();
				this->OnPackDirChange();
			}
		}
		else if (tab->GetPageIndex() == 1) {
			//解包
			if (FileExists(file)) {
				this->OnResFileChange(file);
			}
		}
	}
	return __super::WndProc(msg, wp, lp);
}
MainFrm::~MainFrm() {
	if (task) {
		delete task;
	}
	if (res) {
		delete res;
	}
}