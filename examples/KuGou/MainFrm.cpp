#include "MainFrm.h"

void MainFrm::InitForm() {

	this->Zoom = true;
	//CloseShadow();
	this->SetLayout(ui::UIManager::LoadLayout("xml/main.htm"));
	localList = (VList*)this->FindControl("playList");
	searchList = (VList*)this->FindControl("searchList");
	searchEdit = (Edit*)FindControl("searchEdit");
	//美化左侧本地列表的滚动条
	localList->ScrollBar->SetFixedWidth(9);
	localList->ScrollBar->Style.Radius = 9;
	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(217, 217, 217);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(191, 191, 191);
	//美化搜索列表的滚动条
	searchList->ScrollBar->SetFixedWidth(9);
	searchList->ScrollBar->Style.Radius = 9;
	searchList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	searchList->ScrollBar->Style.ForeColor = Color(250, 200, 200, 200);
	searchList->ScrollBar->ActiveStyle.ForeColor = Color(250, 200, 200, 200);
	//集体设置右上角的最大化 最小化 关闭按钮
	$(this->FindControl("btns")->GetControls()).Css("font-family:\"Marlett\";font-size:13;color:#dddddd;");
	$(this->FindControl("btns")->GetControls()).Css("hover{font-size:13;color:#ffffff;}");

	for (size_t i = 0; i < 99; i++)
	{
		SongItem* it = new SongItem(utf8("TG小辉-女孩(DJ版)"));
		localList->AddControl(it);
	}
	searchList->ScrollBar->Rolling = [=](int a, int b)->void {
		NextPage(a, b);
	};

	FindControl("vlcDock")->AddControl(&player);

	SongView();
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
		global::page = 1;
		global::nextPage = true;

		FindControl("songView")->Trigger(Event::OnMouseClick);

		EString keyword = searchEdit->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		searchList->Clear(true);
		for (auto&& it : songs) {
			SongItem2* sit = new SongItem2(it);
			searchList->AddControl(sit);
		}
		searchList->RefreshLayout();
		searchList->Refresh();
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
			global::HttpGet(url, resp);

			JObject json(resp);
			EString playUrl = json["url"].asCString();
			if (!playUrl.empty()) {
				//::ShellExecuteA(0, "open", playUrl.c_str(), NULL, NULL, SW_SHOW);
				player.OpenUrl(playUrl);
				//player.OpenPath("E:\\CloudMusic\\MV\\Trouble Maker - Smile Again.mp4");
				player.Play();
			}
			else {
				::MessageBoxW(Hwnd(), L"歌曲收费", L"ERROR", 0);
			}
			global::GetSongLrc(hash);
		}
	}
	if (args.EventType == Event::OnMouseClick) {
		if (sender->GetAttribute("tablayout") == "rightView") {
			$(sender->Parent->GetControls()).Not(sender).Css("border-bottom:0").Refresh();
			$(sender).Css("border-bottom:3;border-color:rgb(55,174,254)").Refresh();

			if (sender->Index() == 0) {
				SongView();
			}
			else if (sender->Index() == 1) {
				LrcView();
			}
		}
	}
	return __super::OnNotify(sender, args);
}

void MainFrm::NextPage(int a, int b) {

	Debug::Log("%d %d", a, b);
	if (a != 0 && a >= b && global::nextPage) {
		global::page++;
		EString keyword = searchEdit->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		for (auto&& it : songs) {
			SongItem2* sit = new SongItem2(it);
			searchList->AddControl(sit);
		}

		if (!global::nextPage) {
			Label* end = new Label;
			end->Dock = DockStyle::Horizontal;
			end->SetFixedHeight(35);
			end->Style.BackgroundColor = Color(254, 249, 229);
			end->SetText(L"已经没有更多数据");
			searchList->AddControl(end);
		}
		searchList->RefreshLayout();
		searchList->Refresh();
	}
}

void  MainFrm::SongView() {
	auto main = FindControl("main");
	auto center = FindControl("center");
	auto centerLeft = FindControl("centerLeft");
	centerLeft->Style.BackgroundColor = Color(0, 0, 0, 0);

	auto tools = FindControl("tools");
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);

	main->Style.BackgroundImage.valid = false;

	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(217, 217, 217);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(191, 191, 191);

	searchEdit->Style.BackgroundColor = Color::White;
	center->Style.BackgroundColor = Color::White;
	center->Style.ForeColor = Color::Black;
	main->Refresh();
}
void  MainFrm::LrcView() {
	auto main = FindControl("main");
	auto center = FindControl("center");

	auto centerLeft = FindControl("centerLeft");
	centerLeft->Style.BackgroundColor = Color(200, 200, 200, 200);
	auto tools = FindControl("tools");
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);


	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(100, 255, 255, 255);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(150, 255, 255, 255);

	main->Style.BackgroundImage = new Image(L"imgs/jxy.png");
	searchEdit->Style.BackgroundColor = Color(200, 250, 250, 250);
	center->Style.BackgroundColor = Color(0, 0, 0, 0);
	center->Style.ForeColor = Color::White;

	main->Refresh();
}