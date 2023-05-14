#include "MainFrm.h"
#define refreshImage WM_UIMESSAGE+1
#include "ComBox.h"
MainFrm::MainFrm() :Form(1020, 690)
{
	InitForm();
	//textArea.Style.SetBorder(Color::White, 1);
	//textArea.SetLocation({ 100, 0 });
	//textArea.SetFixedSize({ 200,50 });
	//textArea.SetText(L"hello");
	////textArea.HoverStyle.FontSize = 22;
	//MainLayout->AddControl(&textArea);
}
void MainFrm::InitForm() {
	this->Zoom = true;
	umg.LoadFile("xml/main.htm");
	umg.SetupUI(this);
	//create object
	playingImage = new Image(L"imgs/play.png");
	pauseImage = new Image(L"imgs/pause.png");
	bkImage = new Image(L"imgs/defaultBackground.jpg");
	bkImage->SizeMode = ImageSizeMode::CenterImage;
	headImg = new Image(L"imgs/headImg.jpg");
	headImg->SizeMode = ImageSizeMode::CenterImage;

	cfg = new ConfigIni(Path::StartPath() + "\\list.ini");
	//findControl
	main = FindControl("main");
	main2 = FindControl("main2");
	tools = FindControl("tools");
	center = FindControl("center");
	centerLeft = FindControl("centerLeft");
	control = (TabLayout*)FindControl("control");
	time = (Label*)FindControl("time");
	singer = (Label*)FindControl("singer");
	playerBar2 = this->FindControl("rate");
	playerBar = this->FindControl("playerBar");
	tabCtrl = (TabLayout*)FindControl("rightView");
	localList = (VList*)this->FindControl("playList");
	searchList = (VList*)this->FindControl("searchList");
	searchEdit = (TextBox*)this->FindControl("searchEdit");

	this->FindControl("vlcDock")->AddControl(&player);
	this->FindControl("lrcView2")->AddControl(&lrcCtl);//添加歌词控件

	//美化左侧本地列表的滚动条
	localList->GetScrollBar()->Name = "testBar";
	localList->GetScrollBar()->SetWidth(9);
	localList->GetScrollBar()->Style.Radius = 9;
	localList->GetScrollBar()->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->GetScrollBar()->Style.ForeColor = Color(217, 217, 217);
	localList->GetScrollBar()->ActiveStyle.ForeColor = Color(191, 191, 191);
	//美化搜索列表的滚动条
	searchList->GetScrollBar()->SetWidth(9);
	searchList->GetScrollBar()->Style.Radius = 9;
	searchList->GetScrollBar()->Style.BackgroundColor = Color(50, 200, 200, 200);
	searchList->GetScrollBar()->Style.ForeColor = Color(250, 200, 200, 200);
	searchList->GetScrollBar()->ActiveStyle.ForeColor = Color(250, 200, 200, 200);
	//集体设置右上角的最大化 最小化 关闭按钮 的悬浮效果
	$(this->FindControl("btns")->GetControls()).CssHover("color:#ffffff;");
	main2->Style.BackgroundColor = Color(100, 0, 0, 0);
	singer->Style.BackgroundImage = headImg;
	//加载左侧播放过的音乐
	for (auto&& _it : cfg->GetSections()) {
		EString name = cfg->ReadString("name", "", _it);
		int  dur = cfg->ReadInt("dur", 0, _it);
		EString  singer = cfg->ReadString("singer", "", _it);
		SongItem* it = new SongItem(name, toTimeStr(dur));
		it->SetAttribute("FileHash", _it);
		it->SetAttribute("SingerName", singer);
		it->SetTips(name);
		localList->AddControl(it);
	}
	//滚动条滚动事件 滚动条滚动到底部加载剩余音乐
	searchList->GetScrollBar()->Rolling = [=](int a, int b)->void {
		NextPage(a, b);
	};

	//忽略一些事件 可穿透父控件
	playerBar2->MousePassThrough = Event::OnHover | Event::OnActive | Event::OnMouseClick;
	//创建启动一个实时获取歌曲进度以及状态
	timer = new Thread::Timer;
	timer->Interval = 10;
	timer->Tick = [=](Thread::Timer*) {
		Task();
	};
	//添加一些事件到窗口中的OnNotify函数进行拦截
	player.Tag = (UINT_PTR)main;
	player.AddEventNotify(Event::OnPaint);
	main->AddEventNotify(Event::OnPaint);
	SongView();//
}
MainFrm::~MainFrm()
{
	if (timer) {
		timer->Stop();
		delete timer;
	}
	if (downloadTask) {
		downloadTask->get();
		delete downloadTask;
	}
	if (cfg) {
		delete cfg;
	}
	if (bkImage) {
		delete bkImage;
	}
	if (headImg) {
		delete headImg;
	}
	if (playingImage) {
		delete playingImage;
	}
	if (pauseImage) {
		delete pauseImage;
	}
	if (searchList) {
		searchList->Clear(true);
	}
	if (localList) {
		localList->Clear(true);
	}
}
void MainFrm::OnClose(bool& cal) {
	Application::exit(0);
}

void MainFrm::DownLoadImage(EString _SingerName, EString headImageUrl)
{
	auto  SingerName = _SingerName.Split(",")[0];

	WCHAR temp[256]{ 0 };
	::GetTempPathW(256, temp);
	EString cache = EString(temp) + "KuGou_Cache";
	::CreateDirectoryW(cache.utf16().c_str(), NULL);
	//下载歌手头像
	{
		EString singerBkImg = cache + "\\" + SingerName + "_headImg.jpg";
		WebClient wc2;
		auto code = wc2.DownloadFile(headImageUrl.Replace("{size}", "400"), singerBkImg.ansi(), NULL, 5);
		if (code == 200) {
			headImg = new Image(singerBkImg.utf16());
		}
		else
		{
			headImg = new Image(L"imgs/headImg.jpg");
		}
		headImg->SizeMode = ImageSizeMode::CenterImage;
		singer->Style.BackgroundImage = headImg;
	}
	//下载歌手写真
	{
		auto rect = GetClientRect();
		EString imageUrl = "https://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&&id=0&name=" + HttpUtility::UrlEncode(SingerName) + "&from=pc&json=1&version=1&width=" + std::to_string(1920) + "&height=" + std::to_string(1080);
		EString resp;
		WebClient wc;
		wc.HttpGet(imageUrl, resp, 5);
		JObject json(resp);
		EString bkurl;

		if (bkurl.empty()) {
			for (auto&& it : json["array"]) {
				if (!it["bkurl"].isNull()) {
					bkurl = it["bkurl"].asString();
					break;
				}
			}
		}
		for (auto&& it : json["array"]) {
			if (!it["wpurl"].isNull()) {
				bkurl = it["wpurl"].asString();
				break;
			}
		}
		if (!bkurl.empty()) {
			EString singerBkImg = cache + "\\" + SingerName + ".jpg";
			WebClient wc2;
			wc2.DownloadFile(bkurl, singerBkImg.ansi(), NULL, 5);
			bkImage = new Image(singerBkImg.utf16());
		}
		else {
			bkImage = new Image(L"imgs/defaultBackground.jpg");
		}
		bkImage->SizeMode = ImageSizeMode::CenterImage;
	}
	::PostMessage(Hwnd(), refreshImage, NULL, NULL);
}
void MainFrm::OnKeyDown(WPARAM wparam, LPARAM lParam)
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
		searchList->Invalidate();
	}
	__super::OnKeyDown(wparam, lParam);
}
bool MainFrm::OnNotify(Control* sender, EventArgs& args) {

	if (args.EventType == Event::OnPaint) {


		if (sender == &player) {
			if (tabCtrl->GetPageIndex() == 2) {
				return false;
			}
			return true;
		}
		if (sender == main && player.BuffBitmap) {
			if (tabCtrl->GetPageIndex() == 1) {
				PaintEventArgs& arg = (PaintEventArgs&)args;
				Image img(player.BuffBitmap->_bitmap);
				arg.Graphics.DrawImage(&img, main->GetRect(), ImageSizeMode::CenterImage);
				return true;
			}
			return false;
		}
		return false;
	}

	if (args.EventType == Event::OnMouseDoubleClick) {

		if (sender->Name == "gif") {
			sender->Parent->RemoveControl(sender);
			delete sender;
			return false;
		}

		if (!sender->GetAttribute("FileHash").empty()) {
			EString hash = sender->GetAttribute("FileHash");
			EString url = "http://m.kugou.com/app/i/getSongInfo.php?hash={hash}&cmd=playInfo";
			EString::Replace(&url, "{hash}", hash);
			EString resp;
			//{"errcode":30001,"status":0,"error":"data not found"}
			global::HttpGet(url, resp);
			//resp = resp.ansi();
			timer->Stop();
			JObject json(resp);

			if (json["errcode"].asInt() != 0) {
				::MessageBoxW(Hwnd(), EString(json["error"].asString()).utf16().c_str(), L"音乐API错误", 0);
				return 0;
			}

			int dur = json["timeLength"].asInt();
			EString playUrl = json["url"].asCString();
			if (!playUrl.empty()) {
				EString SingerName = sender->GetAttribute("SingerName");

				if (downloadTask) {
					downloadTask->get();
					delete downloadTask;
				}

				if (headImg) {
					singer->Style.BackgroundImage = NULL;
					delete headImg;
					headImg = NULL;
				}
				if (bkImage) {
					main->Style.BackgroundImage = NULL;
					delete bkImage;
					bkImage = NULL;
				}
				FindControl("lrcView")->Trigger(Event::OnMouseClick);

				//this->DownLoadImage(SingerName, json["imgUrl"].asString());
				downloadTask = new std::future<void>(std::async([&](EString singname, EString imgUrl)->void {
					this->DownLoadImage(singname, imgUrl);
					}, SingerName, json["imgUrl"].asString()));


				if (dynamic_cast<SongItem2*>(sender)) {
					Song* tag = (Song*)sender->Tag;
					SongItem* it = new SongItem(tag->SongName, toTimeStr(dur));
					it->SetAttribute("FileHash", hash);
					it->SetAttribute("SingerName", SingerName);
					localList->AddControl(it);
					localList->ResumeLayout();
					localList->GetScrollBar()->Move(localList->GetScrollBar()->RollingTotal());
					localList->Invalidate();

					cfg->WriteValue("name", tag->SongName, hash);
					cfg->WriteValue("singer", tag->SingerName, hash);
					cfg->WriteValue("dur", std::to_string(dur), hash);
				}

				this->SetText(json["fileName"].asString());
				((Label*)FindControl("songName"))->SetText(json["fileName"].asString());

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

		if (sender->Name == "play") {
			player.Play();
			control->SetPageIndex(1);
			control->ResumeLayout();
			control->Invalidate();
			return false;
		}
		if (sender->Name == "pause") {
			player.Pause();
			control->SetPageIndex(0);
			control->ResumeLayout();
			control->Invalidate();
			return false;
		}
		if (sender->Name == "del") {
			searchList->RemoveControl(sender->Parent);
			delete sender->Parent;
			return false;
		}
		if (sender->Name == "dellocal") {//删除本地
			auto songItem = sender->Parent;
			EString hash = songItem->GetAttribute("FileHash");
			if (!hash.empty()) {
				cfg->DeleteSection(hash);
			}
			this;
			localList;
			//auto it = localList->RemoveControl(songItem);
			delete songItem;
			return false;
		}
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
				EString url = it["downurl"].asString();
				if (!url.empty()) {
					urls.push_back(url);
				}
			}
			FindControl("mvView")->Trigger(Event::OnMouseClick);
			player.OpenUrl(urls[urls.size() - 1]);
			player.Play();
			this->SetText(json["songname"].asString());
			((Label*)FindControl("songName"))->SetText(json["songname"].asString());
			((Label*)FindControl("songName"))->Invalidate();

			player.SetDuration(json["timelength"].asInt() / 1000);
			timer->Start();
		}
		if (sender == playerBar) {
			const MouseEventArgs& arg = (MouseEventArgs&)args;
			double f_pos = arg.Location.X * 1.0 / playerBar->Width();
			player.SetPosition(f_pos);
			player.Play();
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

		if (control->GetPageIndex() != 1) {
			control->SetPageIndex(1);
			control->Invalidate();
		}
		if (fen != lastFen) {
			lastFen = fen;
			time->SetText(fen);
			time->Invalidate();
		}
		if (w != lastWidth) {
			lastWidth = w;
			playerBar2->SetFixedWidth(w);
			playerBar2->Invalidate();
		}
	}
	else {

		if (control->GetPageIndex() != 0) {
			control->SetPageIndex(0);
			control->Invalidate();
		}
	}
}

void MainFrm::OnPaint(PaintEventArgs& arg)
{
	__super::OnPaint(arg);

	/*DXImage img(200, 30);
	{
		DXRender pt(&img);
		pt.SetColor(Color(200, 10, 20, 200));
		pt.FillRectangle({ 0,0,200,30 });
	}
	arg.Graphics.DrawImage(&img, { 10,10,200,30 });*/

	//arg.Graphics.SetFont(L"宋体", 50);
	//arg.Graphics.SetColor(Color::Black);
	//arg.Graphics.SetTransform(GetClientRect().Width/2, GetClientRect().Height/2, 90);
	//arg.Graphics.DrawString(L"我TM反啦...", GetClientRect() ,TextAlign::MiddleCenter);
	//arg.Graphics.SetTransform(0,0,0);

}


void MainFrm::NextPage(int a, int b) {
	//Debug::Log("%d %d", a, b);
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
		searchList->Invalidate();
	}
}
void  MainFrm::SongView() {
	centerLeft->Style.BackgroundColor = Color(0, 0, 0, 0);
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);
	main->Style.BackgroundImage = NULL;
	localList->GetScrollBar()->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->GetScrollBar()->Style.ForeColor = Color(217, 217, 217);
	localList->GetScrollBar()->ActiveStyle.ForeColor = Color(191, 191, 191);
	center->Style.BackgroundColor = Color::White;
	center->Style.ForeColor = Color::Black;
	Invalidate();
}
void  MainFrm::LrcView() {
	centerLeft->Style.BackgroundColor = Color(100, 200, 200, 200);
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);
	localList->GetScrollBar()->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->GetScrollBar()->Style.ForeColor = Color(100, 255, 255, 255);
	localList->GetScrollBar()->ActiveStyle.ForeColor = Color(150, 255, 255, 255);
	main->Style.BackgroundImage = bkImage;
	center->Style.BackgroundColor = Color(0, 0, 0, 0);
	center->Style.ForeColor = Color::White;
	Invalidate();
}

LRESULT MainFrm::WndProc(UINT msg, WPARAM W, LPARAM L)
{
	if (refreshImage == msg) {
		if (headImg) {

			singer->Invalidate();
		}
		FindControl("lrcView")->Trigger(Event::OnMouseClick);
		return 0;
	}
	return __super::WndProc(msg, W, L);
}
