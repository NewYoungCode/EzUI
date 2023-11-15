#include "mainFrom.h"

MainFrm::MainFrm(const EString& cmdLine) :Window(500, 300) {
	this->SetText(L"EzUI资源打包器");
	this->SetLayout(&layout);

	tips.SetText(L"请设置输出文件名");
	tips.SetFixedHeight(35);
	tips.Style.ForeColor = Color::Gray;
	tips.SetParent(&layout);

	edit.Style.Border = 1;
	edit.Style.Border.Color = Color::Gray;
	edit.SetFixedHeight(35);
	edit.SetParent(&layout);

	layout.Add(new Spacer);

	btn.Style.Border.Radius = 4;
	btn.Style.Border = 1;
	btn.Style.Border.Color = Color::Gray;
	btn.HoverStyle.BackColor = Color(100, 100, 100, 100);
	btn.ActiveStyle.BackColor = Color::White;

	btn.SetFixedSize({ 100,30 });
	btn.SetText(L"确认");
	btn.SetParent(&layout);

	layout.Add(new Spacer);

	tips2.SetFixedHeight(40);
	tips2.TextAlign = TextAlign::MiddleCenter;
	tips2.SetParent(&layout);

	bottom.SetFixedHeight(10);
	bottom.SetParent(&layout);

	bar.SetFixedWidth(1);
	bar.Style.BackColor = Color::Blue;
	bar.SetParent(&bottom);

	bottom.Add(new Spacer);

	EString dir = cmdLine;
	if (dir.empty()) {
		return;
	}
	EString::Replace(&dir, "\"", "");
	EString::Replace(&dir, "\\", "/");
	EString::Replace(&dir, "//", "/");
	if (dir[dir.size() - 1] == '/') {
		dir.erase(dir.size() - 1, 1);
	}

	resDir = dir;

	EString rootDir;
	size_t pos = dir.rfind('/');
	EString dirName;
	if (pos != size_t(-1)) {
		rootDir = dir.substr(0, pos);
		dirName = dir.substr(pos + 1);
	}

	resFile = rootDir + "/" + dirName + ".bin";
	edit.SetText(resFile);
}
void MainFrm::OnClose(bool& close) {
	Application::Exit(0);
}
bool MainFrm::FileExists(const EString& fileName) {
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
	if (args.EventType == Event::OnMouseClick && sd == &btn) {
		do
		{
			this->resFile = edit.GetText();

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

			tips2.SetText(L"正在计算...");
			tips2.Invalidate();

			task = new Task([this]() {
				Resource::Package(resDir, resFile, [=](const EString& file, int index, int count) {
					this->Invoke([&]() {
						float rate = (index + 1) * 1.0f / count;
						int width = bottom.Width() * rate + 0.5;
						bar.SetFixedWidth(width);
						bottom.Invalidate();

						tips2.SetText(EString(L"正在打包\"") + file + "\"");
						tips2.Invalidate();
						});
					Sleep(2);
					});

				this->Invoke([&]() {
					tips2.SetText(L"打包成功!");
					tips2.Invalidate();
					::MessageBoxW(Hwnd(), L"打包成功!", L"成功", 0);
					});
				});

		} while (false);
	}
	return __super::OnNotify(sd, args);
}
MainFrm::~MainFrm() {
	if (task) {
		delete task;
	}
}