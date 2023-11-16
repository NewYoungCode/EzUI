#include "MainFrm.h"
MainFrm::MainFrm() :BorderlessWindow(1020, 690), ntfi(WM_NOTIFYICON1)
{
	InitForm();
	//托盘
	ntfi.SetText(L"酷狗音乐");
	ntfi.SetIcon(nullptr);//托盘图标
	this->SetMiniSize({ 800,600 });
}
void MainFrm::InitForm() {
	this->Zoom = true;
	umg.LoadXml("xml/main.htm");
	umg.SetupUI(this);
	cfg = new ConfigIni(Path::StartPath() + "\\list.ini");
	//findControl
	main = FindControl("main");
	//第一次不显示背景图 测试无图绘制的性能


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

	auto aaa = $(this->GetLayout(), "#centerLeft label");

	player.Name = "player";
	this->FindControl("vlcDock")->Add(&player);
	this->FindControl("lrcView2")->Add(&lrcCtl);//添加歌词控件

	std::list<MonitorInfo> monitorInfo;
	GetMonitor(&monitorInfo);
	const MonitorInfo& def = *monitorInfo.begin();
	deskTopWnd = new LayeredWindow(def.Rect.Width, def.Rect.Height);
	deskTopWnd->CloseShadowBox();
	deskTopLrc = new LrcControl();
	deskTopLrc->Style.FontSize = 20;
	deskTopLrc->Style.ForeColor = Color::White;
	//deskTopLrc->Style.BackColor = Color::Black;
	deskTopWnd->SetLayout(deskTopLrc);

	//HWND pWnd = ::FindWindow(L"CvChartWindow", L"");
	HWND pWnd = global::GetWorkerW();

	::SetParent(deskTopWnd->Hwnd(), pWnd);
	//deskTopWnd->Show();

	//给默认背景图片设置缩放属性
	if (main->Style.BackImage) {
		main->Style.BackImage->SizeMode = ImageSizeMode::CenterImage;
	}

	int scrollBarWidth = 9 * this->GetScale() + 0.5;
	////美化左侧本地列表的滚动条
	localList->GetScrollBar()->SetWidth(scrollBarWidth);
	localList->GetScrollBar()->Style.Border.Radius = scrollBarWidth;
	//美化搜索列表的滚动条
	searchList->GetScrollBar()->SetWidth(scrollBarWidth);
	searchList->GetScrollBar()->Style.Border.Radius = scrollBarWidth;
	searchList->GetScrollBar()->Style.BackColor = Color(200, 200, 200, 50);
	//集体设置右上角的最大化 最小化 关闭按钮 的悬浮效果
	$(this->FindControl("btns")->GetControls()).CssHover("color:#ffffff;");
	int pos = 0;
	for (size_t i = 0; i < 1; i++)
	{
		//加载左侧播放过的音乐
		for (auto&& _it : cfg->GetSections()) {

			EString name = cfg->ReadString("name", "", _it);
			int  dur = cfg->ReadInt("dur", 0, _it);
			EString  singer = cfg->ReadString("singer", "", _it);
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
			pos++;
		}
	}

	//滚动条滚动事件 滚动条滚动到底部加载剩余音乐
	searchList->GetScrollBar()->Scroll = [=](ScrollBar* sb, float pos, Event type)->void {
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
	player.Tag = main;


	player.EventNotify = player.EventNotify | Event::OnPaint;
	main->EventNotify = main->EventNotify | Event::OnPaint;

	OpenSongView();//
	//设置阴影
	//this->SetShadow(20);
   // main->Style.Border.Radius = 40;
	main->Style.BackImage->Visible = false;
	//main->Style.Border = 1;
	main->Style.Border.Color = Color(100, 100, 100, 100);
	//关闭窗口阴影
	//this->CloseShadow();
	//WM_TIMER
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
	if (deskTopLrc) {
		delete deskTopLrc;
	}
	if (deskTopWnd) {
		delete deskTopWnd;
	}
}
void MainFrm::OnClose(bool& cal) {
	//关闭窗口时 退出消息循环 程序结束
	Application::Exit(0);
}
void MainFrm::OnPaint(PaintEventArgs& _arg) {
	__super::OnPaint(_arg);


	/*Font font(L"宋体", 20);
	TextLayout text(L"你好hello word!", font);
	Size box = text.GetFontBox();
	_arg.Graphics.SetFont(font);
	_arg.Graphics.SetColor(Color::Black);
	_arg.Graphics.DrawString(text, { 500,200 });
	TextLayout text2(L"你好hello word!", font, box);
	_arg.Graphics.DrawString(text2, { 500,300 });*/

	//{
	//	Rect rect2({ 100,100,100,30 });
	//	D2D_RECT_F rect{ (FLOAT)rect2.X,(FLOAT)rect2.Y,(FLOAT)rect2.GetRight(),(FLOAT)rect2.GetBottom() };
	//	_arg.Graphics.SetColor(Color::Black);
	//	_arg.Graphics.SetStrokeStyle(StrokeStyle::Dash, 3);
	//	_arg.Graphics.FillRectangle(rect2);
	//}

	/*Rect rect2({ 100,100,100,30 });
	D2D_RECT_F rect{ (FLOAT)rect2.X,(FLOAT)rect2.Y,(FLOAT)rect2.GetRight(),(FLOAT)rect2.GetBottom() };
	_arg.Graphics.SetColor(Color::Red);
	_arg.Graphics.SetStrokeStyle(StrokeStyle::Dash, 3);
	_arg.Graphics.FillRectangle(rect2);*/
}
size_t MainFrm::FindLocalSong(const EString& hash)
{
	for (size_t i = 0; i < songs.size(); i++)
	{
		if (songs[i].hash == hash) {
			return i;
		}
	}
	return size_t(-1);
}
void MainFrm::DownLoadImage(EString _SingerName, EString headImageUrl)
{
	auto  SingerName = _SingerName.Split(",")[0];
	std::string headFileData;
	//下载歌手头像 酷狗的接口
	{
		headImg = NULL;
		WebClient wc2;
		auto code = wc2.HttpGet(headImageUrl.Replace("{size}", "400"), headFileData, 5);
		if (code == 200) {
			headImg = new Image(headFileData.c_str(), headFileData.size());
			headImg->SizeMode = ImageSizeMode::CenterImage;
		}
	}
	//下载歌手写真 酷我的接口
	{
		bkImg = NULL;
		auto rect = GetClientRect();
		EString imageUrl = "https://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&&id=0&name=" + HttpUtility::UrlEncode(SingerName) + "&from=pc&json=1&version=1&width=" + std::to_string(1920) + "&height=" + std::to_string(1080);
		EString resp;
		WebClient wc;
		wc.HttpGet(imageUrl, resp, 5);
		JObject json(resp);
		EString bkurl;
		//使用最清晰的图片
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
			std::string fileData;
			WebClient wc2;
			auto code = wc2.HttpGet(bkurl, fileData, 5);
			if (code == 200) {
				bkImg = new Image(fileData.c_str(), fileData.size());
				bkImg->SizeMode = ImageSizeMode::CenterImage;
			}
		}
		else {
			//如果没下载到歌手写真就使用头像
			//bkImg = new Image(headFileData.c_str(), headFileData.size());
			//bkImg->SizeMode = ImageSizeMode::CenterImage;
		}
	}

	this->BeginInvoke([=]() {
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
void MainFrm::OnKeyDown(WPARAM wparam, LPARAM lParam)
{
	if (wparam == 13) {
		global::page = 1;
		global::nextPage = true;
		FindControl("songView")->DispatchEvent(MouseEventArgs(Event::OnMouseClick));
		EString keyword = searchEdit->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		searchList->Clear(true);
		for (auto&& it : songs) {
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
			return false;
		}
		return false;
	}
	if (args.EventType == Event::OnMouseDoubleClick) {
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
				::MessageBoxW(Hwnd(), EString(json["error"].asString()).unicode().c_str(), L"音乐API错误", 0);
				return 0;
			}

			int dur = json["timeLength"].asInt();
			EString playUrl = json["url"].asCString();

			if (!playUrl.empty()) {
				EString SingerName = sender->GetAttribute("SingerName");

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

				FindControl("lrcView")->DispatchEvent(Event::OnMouseClick);

				downloadTask = new Task([this](EString singname, EString imgUrl) {
					this->DownLoadImage(singname, imgUrl);
					}, SingerName, json["imgUrl"].asString());

				if (dynamic_cast<SearchItem*>(sender)) {
					Song* tag = (Song*)sender->Tag;
					if (this->FindLocalSong(tag->hash) == size_t(-1)) {
						songs.push_back(*tag);
						LocalItem* it = new LocalItem(tag->SongName, global::toTimeStr(dur));
						it->SetAttribute("FileHash", hash);
						it->SetAttribute("SingerName", SingerName);
						localList->Add(it);
						localList->RefreshLayout();

						localList->GetScrollBar()->ScrollTo(it);
						//localList->GetScrollBar()->Move(localList->GetContentSize().Height);
						localList->Invalidate();

						cfg->WriteValue("name", tag->SongName, hash);
						cfg->WriteValue("singer", tag->SingerName, hash);
						cfg->WriteValue("dur", std::to_string(dur), hash);
					}
				}

				this->nowSong = hash;
				auto it = localList->FindSingleChild("FileHash", hash);
				$(localList->GetControls()).Css("background-color:rgba(0,0,0,0)").Not(it);
				$(it).Css("background-color:rgba(255,255,255,100)");

				this->SetText(json["fileName"].asString());
				((Label*)FindControl("songName"))->SetText(json["fileName"].asString());
				ntfi.ShowBalloonTip(L"播放音乐", EString(json["fileName"].asString()), 2000);
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
			deskTopLrc->LoadLrc(lrcData);
			timer->Start();
		}
	}

	if (args.EventType == Event::OnMouseClick) {
		if (sender->Name == "login") {
			this->ShowFullScreen();
			return false;
		}
		if (sender->Name == "singer") {
			/*MainFrm* m = new MainFrm();
			m->SetSize({ 800,600 });
			m->Show();*/
			player.OpenPath("C:\\Users\\ly\\Videos\\TubeGet\\[4K60FPS] Girls' Generation - The Boys.webm");
			player.Play();
		}
		if (sender->Name == "next") {
			int pos = this->FindLocalSong(this->nowSong);
			pos++;
			EString hash;
			if (pos >= songs.size()) {
				hash = songs[0].hash;
			}
			else {
				hash = songs[pos].hash;
			}
			auto it = localList->FindSingleChild("FileHash", hash);
			localList->GetScrollBar()->ScrollTo(it);
			it->DispatchEvent(MouseEventArgs(Event::OnMouseDoubleClick));
		}
		if (sender->Name == "up") {
			int pos = this->FindLocalSong(this->nowSong);
			pos--;
			EString hash;
			if (pos < 0) {
				hash = songs[songs.size() - 1].hash;
			}
			else {
				hash = songs[pos].hash;
			}
			auto it = localList->FindSingleChild("FileHash", hash);
			localList->GetScrollBar()->ScrollTo(it);
			it->DispatchEvent(MouseEventArgs(Event::OnMouseDoubleClick));
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

			EString hash = songItem->GetAttribute("FileHash");
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
			FindControl("mvView")->DispatchEvent(Event::OnMouseClick);
			player.OpenUrl(urls[urls.size() - 1]);
			player.Play();
			this->SetText(json["songname"].asString());
			((Label*)FindControl("songName"))->SetText(json["songname"].asString());
			((Label*)FindControl("songName"))->Invalidate();

			player.SetDuration(json["timelength"].asInt() / 1000);

			EString filehash = sender->Parent->GetAttribute("FileHash");
			EString lrcData = global::GetSongLrc(filehash);
			lrcCtl.LoadLrc(lrcData);
			deskTopLrc->LoadLrc(lrcData);
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

	this->Invoke([=]() {
		if (player.GetState() == libvlc_state_t::libvlc_Playing) {
			long long position = player.Position();
			double rate = position / (player.Duration() * 1000.0);
			int w = playerBar->Width() * rate;

			if (deskTopWnd->IsVisible()) {
				deskTopLrc->ChangePostion(position);
			}
			else {
				lrcCtl.ChangePostion(position);
			}

			EString f1 = global::toTimeStr(position / 1000);
			EString f2 = global::toTimeStr(player.Duration());
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
		});
}

void MainFrm::NextPage(float scrollPos) {
	//Debug::Info(L"滚动条当前位置:%d 可滚动距离:%d", a, b);
	if (scrollPos >= 1.0f && global::nextPage) {
		global::page++;
		EString keyword = searchEdit->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		for (auto&& it : songs) {
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
void  MainFrm::OpenSongView() {
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
void  MainFrm::OpenLrcView() {
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
