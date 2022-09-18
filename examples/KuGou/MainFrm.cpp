#include "MainFrm.h"
#define refreshImage WM_UIMESSAGE+1
#define delImage WM_UIMESSAGE+2

void MainFrm::InitForm() {
	this->Zoom = true;
	//CloseShadow();
	this->SetLayout(ui::UIManager::LoadLayout("xml/main.htm"));
	auto main2 = FindControl("main2");
	main2->Style.BackgroundColor = Color(120, 0, 0, 0);


	FindControl("lrcView2")->AddControl(&lrcCtl);//添加歌词控件
	lrcCtl._hWnd = _hWnd;

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

	cfg = new ConfigIni(Path::StartPath() + "\\list.ini");

	for (auto&& _it : cfg->GetSections()) {
		EString name = cfg->ReadString("name", "", _it);
		int  dur = cfg->ReadInt("dur", 0, _it);
		EString  singer = cfg->ReadString("singer", "", _it);
		SongItem* it = new SongItem(name, toTimeStr(dur));
		it->SetAttribute("FileHash", _it);
		it->SetAttribute("SingerName", singer);
		localList->AddControl(it);
	}

	searchList->ScrollBar->Rolling = [=](int a, int b)->void {
		NextPage(a, b);
	};

	FindControl("vlcDock")->AddControl(&player);
	SongView();

	playerBar = FindControl("playerBar");
	playerBar2 = playerBar->GetControl(0);
	playerBar2->MousePassThrough = Event::OnHover | Event::OnActive;

	time = (Label*)FindControl("time");
	singer = (Label*)FindControl("singer");

	bkImage = new Image(L"imgs/defaultBackground.jpg");
	bkImage->SizeMode = ImageSizeMode::CenterImage;

	headImg = new Image(L"imgs/headImg.jpg");
	headImg->SizeMode = ImageSizeMode::CenterImage;
	singer->Style.BackgroundImage = headImg;

	timer = new Timer;
	timer->Interval = 10;
	timer->Tick = [=]() {
		Task();
	};

	auto main = FindControl("main");
	player.Tag = (UINT_PTR)main;
}
MainFrm::MainFrm() :Form(1022, 670)
{
	InitForm();

	auto main = FindControl("main");
	main->BackgroundPainting = [=](PaintEventArgs& arg)->bool {
		if (player.BuffBitmap) {
			Image img(player.BuffBitmap->_bitmap);
			arg.Painter.DrawImage(&img, main->GetRect(), ImageSizeMode::CenterImage);
			return true;
		}
		return false;

	};


	//MainLayout->Style.Radius = 50;
	//CloseShadow();
}
MainFrm::~MainFrm()
{

	timer->Stop();
	if (downloadTask) {
		downloadTask->join();
		delete downloadTask;
	}

}
void MainFrm::OnClose(bool& cal) {
	Application::exit(0);
}
void MainFrm::DownLoadImage(EString SingerName, EString headImageUrl)
{

	::SendMessageW(_hWnd, delImage, 0, 0);

	WCHAR temp[256]{ 0 };
	::GetTempPathW(256, temp);
	EString cache = EString(temp) + "KuGou_Cache";
	::CreateDirectoryW(cache.utf16().c_str(), NULL);
	//下载歌手头像
	{
		EString singerBkImg = cache + "\\" + SingerName + "_headImg.jpg";
		WebClient wc2;
		auto code = wc2.DownloadFile(headImageUrl.Replace("{size}", "400"), Text::UTF8ToANSI(singerBkImg).c_str());
		if (code == 200) {
			headImg = new Image(singerBkImg.utf16());
		}
		else
		{
			headImg = new Image(L"imgs/headImg.jpg");
		}
	}

	{
		//下载歌手写真
		auto rect = GetClientRect();
		EString imageUrl = "https://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&&id=0&name=" + HttpUtility::UrlEncode(SingerName) + "&from=pc&json=1&version=1&width=" + std::to_string(1920) + "&height=" + std::to_string(1080);
		EString resp;
		WebClient wc;
		wc.HttpGet(imageUrl, resp);
		JObject json(resp);
		EString bkurl;
		/*	for (auto&& it : json["array"]) {
				if (!it["wpurl"].isNull()) {
					bkurl = it["wpurl"].asString();
					break;
				}
			}*/
		if (bkurl.empty()) {
			for (auto&& it : json["array"]) {
				if (!it["bkurl"].isNull()) {
					bkurl = it["bkurl"].asString();
					break;
				}
			}
		}

		if (!bkurl.empty()) {
			EString singerBkImg = cache + "\\" + SingerName + ".jpg";
			WebClient wc2;
			wc2.DownloadFile(bkurl, Text::UTF8ToANSI(singerBkImg).c_str());
			bkImage = new Image(singerBkImg.utf16());
		}
		else {
			bkImage = new Image(L"imgs/defaultBackground.jpg");
		}
		bkImage->SizeMode = ImageSizeMode::CenterImage;
	}

	::SendMessageW(_hWnd, refreshImage, 0, 0);
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
		searchList->ResumeLayout();
		searchList->Invalidate();
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
			auto gb = Text::UTF8ToANSI(resp);
			auto id = std::this_thread::get_id();
			int idd = *(int*)&id;
			timer->Stop();
			JObject json(resp);
			int dur = json["timeLength"].asInt();
			EString playUrl = json["url"].asCString();
			if (!playUrl.empty()) {
				EString SingerName = sender->GetAttribute("SingerName");
				if (downloadTask) {
					downloadTask->join();
					delete downloadTask;
				}
				downloadTask = new std::thread(&MainFrm::DownLoadImage, this, SingerName, json["imgUrl"].asString());
				if (dynamic_cast<SongItem2*>(sender)) {
					Song* tag = (Song*)sender->Tag;
					SongItem* it = new SongItem(tag->SongName, toTimeStr(dur));
					it->SetAttribute("FileHash", hash);
					it->SetAttribute("SingerName", SingerName);
					localList->AddControl(it);
					localList->ResumeLayout();

					localList->ScrollBar->Move(localList->ScrollBar->RollingTotal());
					localList->Invalidate();

					cfg->WriteValue("name", tag->SongName, hash);
					cfg->WriteValue("singer", tag->SingerName, hash);
					cfg->WriteValue("dur", std::to_string(dur), hash);
				}

				this->SetText(json["fileName"].asString());

				player.OpenUrl(playUrl);
				player.SetDuration(dur);
				player.Play();
			}
			else {
				::MessageBoxW(Hwnd(), L"歌曲收费", L"ERROR", 0);
				return 0;
			}
			EString lrcData = global::GetSongLrc(hash);
			lrcCtl.LoadLrc(lrcData);
			timer->Start();
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
		if (!sender->GetAttribute("mvhash").empty()) {
			timer->Stop();
			EString resp;
			WebClient wc;
			wc.HttpGet("http://m.kugou.com/app/i/mv.php?cmd=100&hash=" + sender->GetAttribute("mvhash") + "&ismp3=1&ext=mp4", resp);
			JObject json(resp);
			std::vector<EString> urls;
			urls.reserve(6);
			for (auto&& it : json["mvdata"]) {
				urls.push_back(it["downurl"].asString());
			}
			FindControl("mvView")->Trigger(Event::OnMouseClick);
			player.OpenUrl(urls[urls.size() - 1]);
			player.Play();
			timer->Start();
		}
		if (sender == playerBar) {
			const MouseEventArgs& arg = (MouseEventArgs&)args;
			double f_pos = arg.Location.X * 1.0 / playerBar->Width() * (player.Duration());
			player.SetPosition(f_pos);
		}
	}
	return __super::OnNotify(sender, args);
}
void MainFrm::Task() {
	if (player.GetState() == libvlc_state_t::libvlc_Playing) {
		long long position = player.Position();
		double rate = position / (player.Duration() * 1000.0);
		int w = playerBar->Width() * rate;
		lrcCtl.ChangePostion(position);
		EString f1 = toTimeStr(position / 1000);
		EString f2 = toTimeStr(player.Duration());
		EString fen = f1 + "/" + f2;

		if (fen != lastFen) {
			lastFen = fen;
			time->SetText(fen);
			time->Invalidate();

			auto id = std::this_thread::get_id();
			int idd = *(int*)&id;
			int pause = 0;
		}
		if (w != lastWidth) {
			lastWidth = w;
			playerBar2->SetFixedWidth(w);
			((Layout*)playerBar)->ResumeLayout();
			playerBar->Invalidate();
		}
	}
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
		searchList->ResumeLayout();
		searchList->Invalidate();
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
	main->Invalidate();
}
void  MainFrm::LrcView() {
	auto main = FindControl("main");
	auto center = FindControl("center");
	auto centerLeft = FindControl("centerLeft");
	centerLeft->Style.BackgroundColor = Color(100, 200, 200, 200);
	auto tools = FindControl("tools");
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);
	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(100, 255, 255, 255);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(150, 255, 255, 255);
	main->Style.BackgroundImage = bkImage;
	searchEdit->Style.BackgroundColor = Color(200, 250, 250, 250);
	center->Style.BackgroundColor = Color(0, 0, 0, 0);
	center->Style.ForeColor = Color::White;
	main->Invalidate();
}


void MainFrm::OnPaint(HDC winHDC, const Rect& rePaintRect)
{
	StopWatch sw;
	__super::OnPaint(winHDC, rePaintRect);
	char buf[256]{ 0 };
	//Painter pt(winHDC, GetClientRect().Width, GetClientRect().Height);
	//pt.DrawString(L"(LayeredWindow) This is a sample program", L"", 20, Color::Green, GetClientRect(), TextAlign::MiddleCenter);
	sprintf_s(buf, "Opaint %dms\n", sw.ElapsedMilliseconds());
	OutputDebugStringA(buf);
}


LRESULT MainFrm::WndProc(UINT msg, WPARAM W, LPARAM L)
{
	if (delImage == msg) {
		if (headImg) {
			singer->Style.BackgroundImage = NULL;
			delete headImg;
			headImg = NULL;
		}
		if (bkImage) {
			auto main = FindControl("main");
			main->Style.BackgroundImage = NULL;
			delete bkImage;
			bkImage = NULL;
		}
		FindControl("lrcView")->Trigger(Event::OnMouseClick);
	}
	if (refreshImage == msg) {
		headImg->SizeMode = ImageSizeMode::CenterImage;
		singer->Style.BackgroundImage = headImg;
		singer->Invalidate();
		FindControl("lrcView")->Trigger(Event::OnMouseClick);

		return 0;
	}
	return __super::WndProc(msg, W, L);
}
