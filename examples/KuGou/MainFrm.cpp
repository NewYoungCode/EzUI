#include "MainFrm.h"

void MainFrm::InitForm() {

	//this->Zoom = true;
	//CloseShadow();

	this->SetLayout(ui::UIManager::LoadLayout("xml/main.htm"));
	localList = (VList*)this->FindControl("playList");
	searchList = (VList*)this->FindControl("searchList");
	searchEdit = (Edit*)FindControl("searchEdit");
	//美化左侧本地列表的滚动条
	localList->ScrollBar->SetFixedWidth(9);
	localList->ScrollBar->Style.Radius = 9;
	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(100, 255, 255, 255);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(150, 255, 255, 255);
	//美化搜索列表的滚动条
	searchList->ScrollBar->SetFixedWidth(9);
	searchList->ScrollBar->Style.Radius = 9;
	searchList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	searchList->ScrollBar->Style.ForeColor = Color(250, 200, 200, 200);
	searchList->ScrollBar->ActiveStyle.ForeColor = Color(250, 200, 200, 200);
	//集体设置右上角的最大化 最小化 关闭按钮
	$(this->FindControl("btns")->GetControls()).Css("font-family:\"Marlett\";font-size:13;color:#999999;");
	$(this->FindControl("btns")->GetControls()).Css("hover{font-size:13;color:#ffffff;}");
	for (size_t i = 0; i < 99; i++)
	{
		SongItem* it = new SongItem(utf8("TG小辉-女孩(DJ版)"));
		localList->AddControl(it);
	}
	//searchList->ScrollBar->Rolling
}

MainFrm::MainFrm() :Form(1000, 670)
{
	InitForm();
}

MainFrm::~MainFrm()
{
}
void MainFrm::OnClose(bool& cal) {
	//cal = true;
	Application::exit(0);
}

void MainFrm::OnKeyDown(WPARAM wparam)
{
	if (wparam == 13) {

		EString keyword = searchEdit->GetText();
		WebClient wc;

		EString url = "http://songsearch.kugou.com/song_search_v2?keyword={songname}&page=1&pagesize=50&platform=WebFilter";
		EString::Replace(url, "{songname}", keyword);

		EString resp;
		wc.HttpGet(url.c_str(), resp);

		searchList->Clear(true);
		JObject jObj(resp);
		for (auto&& it : jObj["data"]["lists"]) {
			SongItem* sit = new SongItem(it["FileName"].asString());
			sit->SetAttribute("FileHash", it["FileHash"].asString());
			searchList->AddControl(sit);
		}
		searchList->RefreshLayout();
		searchList->Refresh();
		int pause = 0;

	}
	__super::OnKeyDown(wparam);
}

bool MainFrm::OnNotify(Control* sender, const EventArgs& args) {
	if (args.EventType == Event::OnMouseDoubleClick) {
		if (!sender->GetAttribute("FileHash").empty()) {
			EString hash = sender->GetAttribute("FileHash");
			EString url = "http://m.kugou.com/app/i/getSongInfo.php?hash={hash}&cmd=playInfo";
			EString::Replace(url, "{hash}", hash);
			EString resp;
			WebClient wc;
			wc.HttpGet(url, resp);
			JObject json(resp);
			EString playUrl = json["url"].asCString();
			if (!playUrl.empty()) {
				::ShellExecuteA(0, "open", playUrl.c_str(), NULL, NULL, SW_SHOW);
			}
			else {
				::MessageBoxW(Hwnd(), L"歌曲收费", L"ERROR", 0);
			}
		}

	}
	return __super::OnNotify(sender, args);
}