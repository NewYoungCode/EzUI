#include "mainFrom.h"
#include "FileSystem.h"

void MainFrm::Init() {
	this->SetTitle(L"EzUI资源打包器");
	this->LoadXml("res/main.html");
	//第一页的控件
	this->tab = (TabControl*)this->FindControl("tab");
	this->editPackDir = (TextBox*)this->FindControl("editPackDir");
	this->editPackName = (TextBox*)this->FindControl("editPackName");
	this->btnSatrtPackage = (Button*)this->FindControl("btnSatrtPackage");
	this->labelTipsErr = (Label*)this->FindControl("labelTipsErr");
	this->labelTips = (Label*)this->FindControl("labelTips");
	this->editPackDir->TextChanged = [=](TextBox*, const UIString text)->void {
		this->OnPackDirChange();
		};

	//第二页的控件
	this->editResFile = (TextBox*)this->FindControl("editResFile");
	this->btnBrowserFile = (Button*)this->FindControl("btnBrowserFile");
	this->listFiles = (VListView*)this->FindControl("listFiles");
	this->btnUnPackage = (Button*)this->FindControl("btnUnPackage");

	Control* root = this->GetLayout();
	if (root) {
		root->SetDropEnabled(true);
		root->AddEventHandler(Event::DragEnter | Event::DragOver | Event::Drop, [this](Control*, EventArgs* e) {
			FileDragEventArgs* args = e->As<FileDragEventArgs>();
			if (!args || args->Files().empty()) {
				return;
			}

			UIString file = args->Files()[0];
			if (tab->GetPageIndex() == 0) {
				if (!PathExist(file)) {
					return;
				}
				args->Accept();
				if (args->EventType() == Event::Drop) {
					this->editPackDir->SetText(file);
					this->editPackDir->Invalidate();
					this->OnPackDirChange();
				}
			}
			else if (tab->GetPageIndex() == 1) {
				if (!FileExists(file)) {
					return;
				}
				args->Accept();
				if (args->EventType() == Event::Drop) {
					this->OnResFileChange(file);
				}
			}
			});
	}
}

MainFrm::MainFrm(const UIString& cmdLine) :Window() {
	this->SetSize({ 600, 400 });
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

	dir = dir.replace("\"", "");
	dir = dir.replace("\\", "/");
	while (dir.contains("//")) {
		dir = dir.replace("//", "/");
	}
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
void MainFrm::OnClose(bool& allowClose) {
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
void MainFrm::OnNotify(Control* sd, EventArgs* args) {
	if (args->EventType() == Event::MouseClick && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
		if (sd->GetName() == "btnBrowserDir") {
			UIString dir = ShowFolderDialog(GetWindowHandle(), "", "");
			if (!dir.empty()) {
				this->editPackDir->SetText(dir);
				this->editPackDir->Invalidate();
				this->OnPackDirChange();
			}
		}
		if (sd->GetName() == "btnSatrtPackage") {
			do
			{
				UIString resDir = editPackDir->GetText();
				UIString resFile = editPackName->GetText();

				if (task && !task->IsFinished()) {
					::MessageBoxW(GetWindowHandle(), L"请等待上次任务完成!", L"失败", 0);
					break;
				}

				if (FileExists(resFile) && ::DeleteFileW(resFile.unicode().c_str()) == FALSE) {
					::MessageBoxW(GetWindowHandle(), L"文件已存在且无法覆盖!", L"失败", 0);
					break;
				}

				if (task) {
					delete task;
					task = NULL;
				}

				if (resFile.empty()) {
					::MessageBoxW(GetWindowHandle(), L"打包文件路径不正确!", L"失败", 0);
					break;
				}

				labelTips->SetText(L"正在计算...");
				labelTips->Invalidate();

				task = new Thread([resDir, resFile, this]() {
					bool ret = Resource::Package(resDir, resFile, [=](const UIString& file, int index, int count) {
						Invoke([&]() {
							int rate = (index + 1) * 1.0f / count * 100 + 0.5;
							labelTips->SetText(UIString("(" + std::to_string(rate) + "%)") + UIString(L"正在打包\"") + file + "\"");
							labelTips->Invalidate();
							});
						Sleep(2);
						});

					Invoke([&]() {
						if (ret) {
							labelTips->SetText(L"打包成功!");
							labelTips->Invalidate();
							::MessageBoxW(GetWindowHandle(), L"打包成功!", L"成功", 0);
						}
						else {
							::MessageBoxW(GetWindowHandle(), L"打包错误!", L"失败", 0);
						}
						});
					});

			} while (false);
		}
		if (sd->GetName() == "btnBrowserFile") {
			UIString resFile = ShowFileDialog(GetWindowHandle());
			OnResFileChange(resFile);
		}
		if (sd->GetName() == "btnUnPackage") {
			UIString resDir = ShowFolderDialog(GetWindowHandle());
			if (!resDir.empty() && PathExist(resDir)) {
				for (auto& it : this->res->Items) {
					UIString fileName = resDir + "/" + it.Name;
					UIString dir = Path::GetDirectoryName(fileName);
					Directory::Create(dir);
					File::Delete(fileName);
					UIString data;
					this->res->GetFile(it, &data);
					File::Write(data.c_str(), data.size(), fileName);
				}
				::MessageBoxW(GetWindowHandle(), L"解压完成!", L"", 0);
			}
		}
	}
	__super::OnNotify(sd, args);
}
void MainFrm::OnResFileChange(UIString& resFile)
{
	do
	{
		if (FileExists(resFile)) {
			Resource* newRes = new Resource(resFile);
			if (!newRes->IsGood()) {
				::MessageBoxW(GetWindowHandle(), L"不是标准的资源文件", L"错误", 0);
				delete newRes;
				break;
			}
			if (res) {
				delete res;
				res = NULL;
			}
			res = newRes;

			listFiles->RemoveAll(true);
			for (auto& item : res->Items) {
				FileItem* fileItem = new FileItem(item.Name, item.Size);
				listFiles->AddChild(fileItem);
			}
			listFiles->Invalidate();
			this->editResFile->SetText(resFile);
			this->editResFile->Invalidate();
		}
	} while (false);
}
MainFrm::~MainFrm() {
	if (task) {
		delete task;
	}
	if (res) {
		delete res;
	}
}
