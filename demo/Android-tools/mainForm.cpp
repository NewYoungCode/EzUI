#include "mainForm.h"
MainFrm::MainFrm(const EString& cmdLine) :Window(600, 400) {
	ui.LoadXmlFile(L"xml/mainForm.htm");
	ui.SetupUI(this);
	this->SetText(L"Android-tools");
	Directory::Create(Path::StartPath() + "/Temp");
	rate_bar = (Label*)this->FindControl("rate_bar");
}
bool MainFrm::OnNotify(Control* sd, EventArgs& args) {
	if (args.EventType == Event::OnMouseClick) {
		if (sd->Name == "btn_down") {
			if (task) {
				return false;
			}
			EString url = ((TextBox*)FindControl("textBox_downloadUrl"))->GetText();
			if (url.empty()) {
				return false;
			}
			task = new Task([this, url]() {
				WebClient wc;
				EString fileName = Path::GetFileName(url);
				EString tempPath = Path::StartPath() + "/Temp";
				Directory::Create(tempPath);
				EString downFileName = tempPath + "/" + fileName;

				wc.DownloadFile(url, downFileName.unicode(), [&](__int64 total, __int64 now, float rate)->void {
					this->Invoke([=]() ->void {
						double bfb = now * 1.0f / total;
						rate_bar->SetFixedWidth(rate_bar->Parent->Width() * bfb);
						rate_bar->SetText(Text::ToString(bfb * 100, 2) + "%");
						rate_bar->Parent->Invalidate();
						});
					}, 9999999);

				::MessageBoxW(Hwnd(), L"下载已完成!", L"", 0);
				});

		}
		//ADB命令
		if (sd->Name == "btn_adb_reboot") {
			WinTool::ExecuteCMD("platform-tools/adb.exe reboot");
		}
		if (sd->Name == "btn_adb_rec") {
			WinTool::ExecuteCMD("platform-tools/adb.exe reboot recovery");
		}
		if (sd->Name == "btn_adb_fb") {
			WinTool::ExecuteCMD("platform-tools/adb.exe reboot fastboot");
		}
		if (sd->Name == "btn_adb_9008") {
			WinTool::ExecuteCMD("platform-tools/adb.exe reboot edl");
		}
		//FASTBOOT命令
		if (sd->Name == "btn_fb_reboot") {
			WinTool::ExecuteCMD("platform-tools/fastboot.exe reboot");
		}
		if (sd->Name == "btn_fb_unlock") {
			WinTool::ExecuteCMD("platform-tools/fastboot.exe flashing unlock");
		}
		if (sd->Name == "btn_fb_lock") {
			WinTool::ExecuteCMD("platform-tools/fastboot.exe flashing lock");
		}
		if (sd->Name == "btn_fb_flash_boot") {
			::MessageBoxW(Hwnd(), L"刷入boot!", L"", 0);
			WinTool::ExecuteCMD("platform-tools/fastboot.exe flash boot ??");
		}
		if (sd->Name == "btn_done") {
			Directory::Delete(Path::StartPath() + "/Temp");
			::MessageBoxW(Hwnd(), L"done", L"信息", 0);
		}
	}
	return __super::OnNotify(sd, args);
}
void MainFrm::OnClose(bool& close) {
	Application::Exit(0);
}