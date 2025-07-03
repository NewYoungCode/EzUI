#include "mainFrm.h"
MainFrm::MainFrm() :Form(1020, 690)
{
	InitForm();
	//托盘初始化
	ntfi.SetText(L"酷苟音乐");
	ntfi.SetIcon(nullptr);//托盘图标

	Menu* exitMenu = new Menu(L"退出");
	exitMenu->Append(new Menu(L"打开主程序"));

	ntfi.SetMenu(exitMenu);

	ntfi.MessageCallback = [=](UINT menId)->bool {
		
		int pause = 0;
		return true;
		};

	this->SetMiniSize({ 800,600 });
}
void MainFrm::InitForm() {
	this->SetResizable(true);
	this->SetText(L"酷苟音乐");

	umg.LoadXmlFile("res/xml/main.htm");
	umg.SetupUI(this);

	//设置窗口边框样式
	this->Border.Radius = 15;
	this->Border.Color = Color(128, 128, 128, 100);
	this->Border = 1;

	//关闭阴影
	//this->CloseShadowBox();

	cfg = new ConfigIni(Path::StartPath() + "\\list.ini");
	//找到每一个控件先
	main = FindControl("main");
	tools = FindControl("tools");
	center = FindControl("center");
	centerLeft = FindControl("centerLeft");
	control = (TabLayout*)FindControl("control");
	time = (Label*)FindControl("time");
	singer = (Label*)FindControl("singer");
	playerBar2 = this->FindControl("rate");
	playerBar = this->FindControl("playerBar");
	tabCtrl = (TabLayout*)FindControl("rightView");
	localList = (VListView*)this->FindControl("playList");
	searchList = (VListView*)this->FindControl("searchList");
	searchEdit = (TextBox*)this->FindControl("searchEdit");
	labelDeskLrc = (CheckBox*)this->FindControl("deskLrc");

	player.Name = "player";
	this->FindControl("vlcDock")->Add(&player);
	this->FindControl("lrcView2")->Add(&lrcCtl);//添加歌词控件

	//创建桌面歌词视频窗口
	deskTopWnd = new DesktopLrcFrm(&player);

	//给默认背景图片设置缩放属性
	if (main->Style.BackImage) {
		main->Style.BackImage->SizeMode = ImageSizeMode::CenterImage;
	}

	//集体设置右上角的最大化 最小化 关闭按钮 的悬浮效果
	$(this->FindControl("btns")->GetControls()).CssHover("color:#ffffff;");

	//加载左侧播放过的音乐
	for (auto& _it : cfg->GetSections()) {

		UIString name = cfg->ReadString("name", "", _it);
		int  dur = cfg->ReadInt("dur", 0, _it);
		UIString  singer = cfg->ReadString("singer", "", _it);
		LocalItem* it = new LocalItem(name, global::toTimeStr(dur));
		it->SetAttribute("FileHash", _it);
		it->SetAttribute("SingerName", singer);
		it->SetTips(name);

		Song s;
		s.SongName = cfg->ReadString("name", "", _it);
		s.hash = _it;
		s.Duration = cfg->ReadInt("dur", 0, _it);
		s.SingerName = cfg->ReadString("singer", "", _it);
		songs.push_back(s);

		localList->Add(it);
	}
	//滚动条滚动事件 滚动条滚动到底部加载剩余音乐
	searchList->GetScrollBar()->Scroll = [=](IScrollBar* sb, float pos, Event type)->void {
		if (type == Event::OnMouseWheel) {
			NextPage(pos);
		}
		};
	//可穿透父控件
	playerBar2->EventPassThrough = Event::OnHover | Event::OnActive | Event::OnMouseClick;
	//创建启动一个实时获取歌曲进度以及状态
	timer = new ThreadTimer;
	timer->Interval = 10;
	timer->Tick = [=](ThreadTimer*) {
		TimerTick();
		};
	//添加一些事件到窗口中的OnNotify函数进行拦截
	player.EventFilter = player.EventFilter | Event::OnPaint;
	main->EventFilter = main->EventFilter | Event::OnPaint;
	//播放视频的时候每一帧的回调
	player.PlayingCallback = [&](Bitmap* bitmap)->void {
		BeginInvoke([&]() {
			this->Invalidate();
			if (deskTopWnd->IsVisible()) {
				this->deskTopWnd->GetLayout()->Style.BackImage = NULL;
				this->deskTopWnd->Invalidate();
			}
			});
		};
	//打开默认显示界面
	OpenSongView();
}
MainFrm::~MainFrm()
{
	if (timer) {
		delete timer;
	}
	if (downloadTask) {
		delete downloadTask;
	}
	if (cfg) {
		delete cfg;
	}
	if (headImg) {
		delete headImg;
	}
	if (bkImg) {
		delete bkImg;
	}
	if (searchList) {
		searchList->Clear(true);
	}
	if (localList) {
		localList->Clear(true);
	}
	if (deskTopWnd) {
		delete deskTopWnd;
	}
}
void MainFrm::OnClose(bool& cal) {
	//关闭窗口时 退出消息循环 程序结束
	Application::Exit(0);
}

size_t MainFrm::FindLocalSong(const UIString& hash)
{
	for (size_t i = 0; i < songs.size(); i++)
	{
		if (songs[i].hash == hash) {
			return i;
		}
	}
	return size_t(-1);
}
void MainFrm::DownLoadImage(UIString _SingerName, UIString headImageUrl)
{
	auto  SingerName = _SingerName.split(",")[0];
	std::string headFileData;
	//下载歌手头像 酷狗的接口
	{
		headImg = NULL;
		WebClient wc2;
		auto code = wc2.HttpGet(headImageUrl.replace("{size}", "400"), headFileData, 5);
		if (code == 200) {
			headImg = new Image(headFileData.c_str(), headFileData.size());
			headImg->SizeMode = ImageSizeMode::CenterImage;
		}
	}

	//下载歌手写真
	{
		bkImg = NULL;
		auto rect = GetClientRect();
		UIString bkurl = global::GetSingerBackground(_SingerName);
		if (!bkurl.empty()) {
			std::string fileData;
			WebClient wc2;
			auto code = wc2.HttpGet(bkurl, fileData, 5);
			if (code == 200) {
				bkImg = new Image(fileData.c_str(), fileData.size());
				bkImg->SizeMode = ImageSizeMode::CenterImage;
			}
		}
	}

	//回到主线程去设置歌手头像 歌手背景图
	BeginInvoke([=]() {
		if (headImg) {
			singer->Style.ForeImage = headImg;
			singer->Style.BackImage->Visible = false;
		}
		else {
			singer->Style.BackImage->Visible = true;
		}
		if (bkImg) {
			main->Style.ForeImage = bkImg;
			main->Style.BackImage->Visible = false;
			deskTopWnd->GetLayout()->Style.BackImage = bkImg;
		}
		else {
			main->Style.BackImage->Visible = true;
		}
		singer->Invalidate();
		main->Invalidate();
		});

}



bool MainFrm::PlayForHash(const UIString& hash, Song& info)
{
	timer->Stop();

	UIString errStr;
	bool ret = global::GetSongInfo(hash, errStr, info);
	if (!ret) {
		::MessageBoxW(Hwnd(), errStr.unicode().c_str(), L"无法播放", MB_OK);
		return false;
	}

	UIString singerName = info.SingerName;
	auto singers = singerName.split("、");
	if (!singers.empty()) {
		singerName = singers[0];
	}

	if (downloadTask) {
		delete downloadTask;
	}

	if (headImg) {
		delete headImg;
		singer->Style.ForeImage = NULL;
		headImg = NULL;
		singer->Invalidate();
	}
	if (bkImg) {
		delete bkImg;
		main->Style.ForeImage = NULL;
		deskTopWnd->GetLayout()->Style.BackImage = NULL;
		bkImg = NULL;
		main->Invalidate();
	}

	FindControl("lrcView")->SendNotify(Event::OnMouseClick);

	downloadTask = new Task([this](UIString singname, UIString imgUrl) {
		this->DownLoadImage(singname, imgUrl);
		}, singerName, info.imgUrl);

	this->nowSong = hash;
	auto it = localList->FindSingleChild("FileHash", hash);

	//暂时取消这样子设置样式
	//$(localList->GetControls()).Css("background-color:rgba(0,0,0,0)").Not(it);
	//$(it).Css("background-color:rgba(255,255,255,100)");

	this->SetText(info.fileName);
	((Label*)FindControl("songName"))->SetText(info.fileName);
	//系统托盘处弹出正在播放音乐的提示
	ntfi.ShowBalloonTip(L"播放音乐", info.fileName, 2000);
	//打开URL 准备开始播放音乐
	player.OpenUrl(info.url);
	player.SetDuration(info.Duration);
	player.Play();
	UIString lrcData = global::GetSongLrc(hash);
	lrcCtl.LoadLrc(lrcData);
	deskTopWnd->LoadLrc(lrcData);
	timer->Start();
	return 1;

}
void MainFrm::OnKeyDown(WPARAM wparam, LPARAM lParam)
{
	//回车搜索歌曲
	if (wparam == VK_RETURN) {
		global::page = 1;
		global::nextPage = true;
		FindControl("songView")->SendNotify(MouseEventArgs(Event::OnMouseClick));
		UIString keyword = searchEdit->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		searchList->Clear(true);
		for (auto& it : songs) {
			SearchItem* sit = new SearchItem(it);
			searchList->Add(sit);
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
				Image img(player.BuffBitmap->GetHBITMAP());
				img.SizeMode = ImageSizeMode::CenterImage;
				arg.Graphics.DrawImage(&img, main->GetRect());
				return true;
			}
			else if (deskTopWnd->IsVisible()) {
				deskTopWnd->Invalidate();
			}
			return false;
		}
		return false;
	}
	if (args.EventType == Event::OnMouseDoubleClick) {
		if (!sender->GetAttribute("FileHash").empty()) {
			UIString hash = sender->GetAttribute("FileHash");

			Song info;
			bool ret = this->PlayForHash(hash, info);
			if (ret) {
				if (dynamic_cast<SearchItem*>(sender)) {
					//将播放的歌曲记录到本地文件中
					Song* tag = (Song*)sender->Tag;
					if (this->FindLocalSong(tag->hash) == size_t(-1)) {
						songs.push_back(*tag);
						LocalItem* it = new LocalItem(tag->SongName, global::toTimeStr(info.Duration));
						it->SetAttribute("FileHash", hash);
						it->SetAttribute("SingerName", info.SingerName);
						localList->Add(it);
						localList->RefreshLayout();
						localList->GetScrollBar()->ScrollTo(it);
						localList->Invalidate();
						cfg->WriteValue("name", tag->SongName, hash);
						cfg->WriteValue("singer", tag->SingerName, hash);
						cfg->WriteValue("dur", std::to_string(info.Duration), hash);
					}
				}
			}
		}
	}

	if (args.EventType == Event::OnMouseClick) {
		if (sender->Name == "login") {
			Application::Exit();
		}
		if (sender->Name == "next") {
			int pos = this->FindLocalSong(this->nowSong);
			pos++;
			UIString hash;
			if (pos >= songs.size()) {
				hash = songs[0].hash;
			}
			else {
				hash = songs[pos].hash;
			}
			auto it = localList->FindSingleChild("FileHash", hash);
			localList->GetScrollBar()->ScrollTo(it);
			it->SendNotify(MouseEventArgs(Event::OnMouseDoubleClick));
		}
		if (sender->Name == "up") {
			int pos = this->FindLocalSong(this->nowSong);
			pos--;
			UIString hash;
			if (pos < 0) {
				hash = songs[songs.size() - 1].hash;
			}
			else {
				hash = songs[pos].hash;
			}
			auto it = localList->FindSingleChild("FileHash", hash);
			localList->GetScrollBar()->ScrollTo(it);
			it->SendNotify(MouseEventArgs(Event::OnMouseDoubleClick));
		}
		if (sender->Name == "deskLrc") {
			if (deskTopWnd->IsVisible()) {
				deskTopWnd->SetVisible(false);
			}
			else {
				deskTopWnd->SetVisible(true);
				deskTopWnd->Invalidate();
			}
			//sender->Invalidate();
		}
		if (sender->Name == "play") {
			player.Play();
			control->SetPageIndex(1);
			control->Invalidate();
			return false;
		}
		if (sender->Name == "pause") {
			player.Pause();
			control->SetPageIndex(0);
			control->Invalidate();
			return false;
		}
		if (sender->Name == "dellocal") {//删除本地
			LocalItem* songItem = (LocalItem*)sender->Parent;
			localList->Remove(songItem);

			UIString hash = songItem->GetAttribute("FileHash");
			if (!hash.empty()) {
				cfg->DeleteSection(hash);
			}
			delete songItem;
			localList->Invalidate();
			return false;
		}
		if (sender->GetAttribute("tablayout") == "rightView") {
			size_t pos = sender->Parent->IndexOf(sender);
			if (pos == 0) {
				OpenSongView();
			}
			else if (pos == 1) {
				OpenLrcView();
			}
			else {
				Invalidate();
			}
		}
		if (!sender->GetAttribute("mvhash").empty()) {
			timer->Stop();
			UIString mvhash = sender->GetAttribute("mvhash");

			Song info;
			global::GetMvInfo(mvhash, info);

			FindControl("mvView")->SendNotify(Event::OnMouseClick);

			this->SetText(info.SongName);
			((Label*)FindControl("songName"))->SetText(info.SongName);
			((Label*)FindControl("songName"))->Invalidate();


			UIString filehash = sender->Parent->GetAttribute("FileHash");
			UIString lrcData = global::GetSongLrc(filehash);

			player.OpenUrl(info.url);
			player.Play();
			player.SetDuration(info.Duration / 1000);

			lrcCtl.LoadLrc(lrcData);
			deskTopWnd->LoadLrc(lrcData);
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
void MainFrm::TimerTick() {

	Invoke([=]() {
		if (player.GetState() == libvlc_state_t::libvlc_Playing) {
			long long position = player.Position();
			auto duration = player.Duration();
			double rate = position / (duration * 1000.0);
			int w = playerBar->Width() * rate;

			lrcCtl.ChangePostion(position);
			if (deskTopWnd->IsVisible()) {
				deskTopWnd->ChangePostion(position);
			}

			UIString f1 = global::toTimeStr(position / 1000);
			UIString f2 = global::toTimeStr(duration);
			UIString fen = f1 + "/" + f2;

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
		});
}

void MainFrm::NextPage(float scrollPos) {
	//Debug::Info(L"滚动条当前位置:%d 可滚动距离:%d", a, b);
	if (scrollPos >= 1.0f && global::nextPage) {
		global::page++;
		UIString keyword = searchEdit->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		for (auto& it : songs) {
			SearchItem* sit = new SearchItem(it);
			searchList->Add(sit);
		}
		if (!global::nextPage) {
			Label* end = new Label;
			end->SetFixedHeight(35);
			end->Style.BackColor = Color(254, 249, 229);
			end->SetText(L"已经没有更多数据");
			searchList->Add(end);
		}
		searchList->Invalidate();
	}
}
void MainFrm::OpenSongView() {
	centerLeft->Style.BackColor = Color::Transparent;
	tools->Style.Border.Bottom = 1;
	tools->Style.Border.Color = Color(238, 238, 238);
	localList->GetScrollBar()->Style.BackColor = Color(200, 200, 200, 50);
	localList->GetScrollBar()->Style.ForeColor = Color(217, 217, 217);
	localList->GetScrollBar()->ActiveStyle.ForeColor = Color(191, 191, 191);
	center->Style.BackColor = Color::White;
	center->Style.ForeColor = Color::Black;
	Invalidate();
}
void MainFrm::OpenLrcView() {
	centerLeft->Style.BackColor = Color(200, 200, 200, 100);
	tools->Style.Border.Bottom = 1;
	tools->Style.Border.Color = Color(238, 238, 238);
	localList->GetScrollBar()->Style.BackColor = Color(200, 200, 200, 50);
	localList->GetScrollBar()->Style.ForeColor = Color(255, 255, 255, 100);
	localList->GetScrollBar()->ActiveStyle.ForeColor = Color(255, 255, 255, 150);
	center->Style.BackColor = Color::Transparent;
	center->Style.ForeColor = Color::White;
	Invalidate();
}
