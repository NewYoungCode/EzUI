#include "mainFrm.h"
MainFrm::MainFrm() :Form(1020, 690)
{
	InitForm();
	//托盘初始化
	ntfi.SetTips(L"酷苟音乐");
	ntfi.SetIcon(nullptr);//托盘图标

	Menu* menu = new Menu(&ntfi);
	UINT_PTR id_open = menu->Append(L"打开主程序");
	UINT_PTR id_exit = menu->Append(L"退出");
	ntfi.SetMenu(menu);

	menu->MouseClick = [=](UINT_PTR menuId) {
		if (menuId == id_open) {
			::ShowWindow(Hwnd(), SW_RESTORE);
		}
		if (menuId == id_exit) {
			Application::Exit();
		}
		};

	//加载ico图标
	std::string fileData;
	HICON icon = ezui::LoadIcon("res/icon.ico");
	ntfi.SetIcon(icon);

	ntfi.EventHandler = [=](const MouseEventArgs& args)->void {
		if (args.EventType == Event::OnMouseDoubleClick && args.Button == MouseButton::Left) {
			::ShowWindow(Hwnd(), SW_RESTORE);
		}
		};

	this->SetMiniSize({ 800,600 });
}
void MainFrm::InitForm() {
	this->SetResizable(true);
	this->SetText(L"酷苟音乐");

	umg.LoadXml("res/xml/main.htm");
	umg.SetupUI(this);

	//设置窗口边框样式
	this->GetLayout()->Style.Border.Radius = 15;
	this->GetLayout()->Style.Border.Color = Color(128, 128, 128, 100);
	this->GetLayout()->Style.Border.Style = StrokeStyle::Solid;
	this->GetLayout()->Style.Border = 1;

	//关闭阴影
	//this->CloseShadowBox();

	//加载本地播放过的音乐
	listFile = new ConfigIni(Path::StartPath() + "\\list.ini");
	//找到每一个控件先
	mainLayout = FindControl("mainLayout");
	tools = FindControl("tools");
	centerLayout = FindControl("centerLayout");
	centerLeft = FindControl("centerLeft");
	mediaCtl = (TabLayout*)FindControl("mediaCtl");
	labelTime = (Label*)FindControl("labelTime");
	labelSinger = (Label*)FindControl("labelSinger");
	playerBar2 = this->FindControl("playerBar2");
	playerBar = this->FindControl("playerBar");
	tabCtrl = (TabLayout*)FindControl("rightView");
	vlistLocal = (VListView*)this->FindControl("playList");
	vlistSearch = (VListView*)this->FindControl("searchList");
	editSearch = (TextBox*)this->FindControl("searchEdit");
	labelDeskLrc = (CheckBox*)this->FindControl("deskLrc");

	player.Name = "player";
	this->FindControl("vlcDock")->Add(&player);
	this->FindControl("lrcView2")->Add(&lrcPanel);//添加歌词控件

	//创建桌面歌词视频窗口
	deskTopWnd = new DesktopLrcFrm(&player);

	//给默认背景图片设置缩放属性
	if (mainLayout->Style.BackImage) {
		mainLayout->Style.BackImage->SizeMode = ImageSizeMode::Cover;
	}

	//加载左侧播放过的音乐
	for (auto& item : listFile->GetSections()) {

		listFile->SetSection(item);
		UIString name = listFile->ReadString("name");
		int  dur = listFile->ReadInt("dur");
		UIString  singer = listFile->ReadString("singer");
		LocalItem* it = new LocalItem(name, global::toTimeStr(dur));
		it->SetAttribute("FileHash", item);
		it->SetAttribute("SingerName", singer);
		it->SetTips(name);

		Song s;
		s.SongName = listFile->ReadString("name");
		s.hash = item;
		s.Duration = listFile->ReadInt("dur");
		s.SingerName = listFile->ReadString("singer");

		songLsit.push_back(s);
		vlistLocal->Add(it);
	}
	//滚动条滚动事件 滚动条滚动到底部加载剩余音乐
	vlistSearch->GetScrollBar()->Scroll = [=](ScrollBar* sb, float pos, Event type)->void {
		if (type == Event::OnMouseWheel) {
			NextPage(pos);
		}
		};
	//可穿透父控件
	playerBar2->EventPassThrough = Event::OnHover | Event::OnActive | Event::OnMouseDown;
	//创建启动一个实时获取歌曲进度以及状态
	timer = new Timer;
	timer->Interval = 10;
	timer->Tick = [=](Timer*) {
		TimerTick();
		};
	//添加一些事件到窗口中的OnNotify函数进行拦截
	player.EventFilter = player.EventFilter | Event::OnPaint;
	mainLayout->EventFilter = mainLayout->EventFilter | Event::OnPaint;
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

void MainFrm::OnPaint(PaintEventArgs& args) {

	__super::OnPaint(args);

	//args.Graphics.SetColor(Color::Red);
	//Geometry path;

	//PointF p1(100, 150);   // 尖朝下（底部顶点）
	//PointF p2(50, 50);     // 左上角
	//PointF p3(150, 50);    // 右上角

	//path.BeginFigure(p1); // BeginFigure
	//path.AddLine(p2); // 
	//path.AddLine(p3); // 
	//path.CloseFigure();

	//args.Graphics.FillGeometry(&path);
}

MainFrm::~MainFrm()
{
	if (timer) {
		delete timer;
	}
	if (downloadTask) {
		delete downloadTask;
	}
	if (listFile) {
		delete listFile;
	}
	if (headImg) {
		delete headImg;
	}
	if (bkImg) {
		delete bkImg;
	}
	if (vlistSearch) {
		vlistSearch->Clear(true);
	}
	if (vlistLocal) {
		vlistLocal->Clear(true);
	}
	if (deskTopWnd) {
		delete deskTopWnd;
	}
}

void MainFrm::OnClose(bool& cal) {
	cal = false;
	//给窗口添加淡出效果
	Animation* ant = new Animation(this);//绑定父对象为frm,则ant无需手动释放
	ant->SetStartValue(1.0);
	ant->SetEndValue(0);
	ant->ValueChanged = [&](float value) {
		Invoke([this, value] {
			this->Opacity = value;//修改透明度
			this->Invalidate();//刷新
			if (value <= 0.1) {
				//退出消息循环 程序结束
				Application::Exit(0);
			}
			});
		};
	this->Opacity = 1;
	ant->Start(200);//开始动画
}

size_t MainFrm::FindLocalSong(const UIString& hash)
{
	for (size_t i = 0; i < songLsit.size(); i++)
	{
		if (songLsit[i].hash == hash) {
			return i;
		}
	}
	return size_t(-1);
}
void MainFrm::DownLoadImage(UIString singers, UIString headImageUrl)
{
	//随机选一个歌手
	auto strs = singers.split("、");
	Random rdom;
	int pos = rdom.Next(0, strs.size() - 1);
	UIString SingerName = strs[pos];

	std::string headFileData;
	//下载歌手头像 酷狗的接口
	{
		headImg = NULL;
		WebClient wc2;
		auto code = wc2.HttpGet(headImageUrl.replace("{size}", "400"), &headFileData, 5);
		if (code == 200) {
			headImg = new Image(headFileData.c_str(), headFileData.size());
			headImg->SizeMode = ImageSizeMode::Cover;
		}
	}

	//下载歌手写真
	{
		bkImg = NULL;
		auto rect = GetClientRect();
		UIString bkurl = global::GetSingerBackground(SingerName);
		if (!bkurl.empty()) {
			std::string fileData;
			WebClient wc2;
			auto code = wc2.HttpGet(bkurl, &fileData, 5);
			if (code == 200) {
				bkImg = new Image(fileData.c_str(), fileData.size());
				bkImg->SizeMode = ImageSizeMode::Cover;
			}
		}
	}

	//回到主线程去设置歌手头像 歌手背景图
	BeginInvoke([=]() {
		if (headImg) {
			labelSinger->Style.ForeImage = headImg;
			labelSinger->Style.BackImage->Visible = false;
		}
		else {
			labelSinger->Style.BackImage->Visible = true;
		}
		if (bkImg) {
			mainLayout->Style.ForeImage = bkImg;
			mainLayout->Style.BackImage->Visible = false;
			deskTopWnd->GetLayout()->Style.BackImage = bkImg;
		}
		else {
			mainLayout->Style.BackImage->Visible = true;
		}
		labelSinger->Invalidate();
		mainLayout->Invalidate();
		});

}

bool MainFrm::PlaySong(const UIString& hash, Song& info)
{
	timer->Stop();

	UIString errStr;
	bool ret = global::GetSongInfo(hash, errStr, info);
	if (!ret) {
		::MessageBoxW(Hwnd(), errStr.unicode().c_str(), L"无法播放", MB_OK);
		return false;
	}

	playType = 1;//当前正在播放音乐

	FindControl("lrcView")->SendEvent(Event::OnMouseDown);

	if (this->FindLocalSong(hash) == size_t(-1)) {
		info.hash = hash;
		songLsit.push_back(info);

		//创建左侧音乐Item
		LocalItem* item = new LocalItem(info.fileName, global::toTimeStr(info.Duration));
		item->SetAttribute("FileHash", hash);
		item->SetAttribute("SingerName", info.SingerName);

		//添加到左侧音乐列表
		vlistLocal->Add(item);
		vlistLocal->RefreshLayout();
		vlistLocal->GetScrollBar()->ScrollTo(item);
		vlistLocal->Invalidate();

		//写入本地文件
		listFile->SetSection(hash);
		listFile->WriteString("name", info.fileName);
		listFile->WriteString("singer", info.SingerName);
		listFile->WriteString("dur", std::to_string(info.Duration));
	}

	//请求歌手头像和写真
	RequestNewImage(info);

	//设置一些状态
	this->nowSong = hash;
	this->SetText(info.fileName);
	((Label*)FindControl("songName"))->SetText(info.fileName);
	//系统托盘处弹出正在播放音乐的提示
	ntfi.ShowBalloonTip(L"播放音乐", info.fileName, 2000);
	//打开URL 准备开始播放音乐
	player.OpenUrl(info.url);
	player.SetDuration(info.Duration);
	player.Play();
	UIString lrcData = global::GetSongLrc(hash);
	lrcPanel.LoadLrc(lrcData);
	deskTopWnd->LoadLrc(lrcData);
	timer->Start();
	return true;

}
void MainFrm::OnKeyDown(WPARAM wparam, LPARAM lParam)
{
	//回车搜索歌曲
	if (wparam == VK_RETURN) {
		global::page = 1;
		global::nextPage = true;
		FindControl("songView")->SendEvent(Event::OnMouseDown);
		UIString keyword = editSearch->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		vlistSearch->Clear(true);
		for (auto& it : songs) {
			SearchItem* sit = new SearchItem(it);
			vlistSearch->Add(sit);
		}
		vlistSearch->Invalidate();
	}
	__super::OnKeyDown(wparam, lParam);
}
bool MainFrm::OnNotify(Control* sender, EventArgs& args) {
	do {
		if (args.EventType == Event::OnPaint) {
			if (sender == &player) {
				if (tabCtrl->GetPageIndex() == 2) {
					break;
				}
				return true;
			}
			if (playType == 2 && sender == mainLayout && player.BuffBitmap) {
				if (tabCtrl->GetPageIndex() == 1) {
					PaintEventArgs& arg = (PaintEventArgs&)args;
					Image img(player.BuffBitmap->GetHBITMAP());
					img.SizeMode = ImageSizeMode::Cover;
					arg.Graphics.DrawImage(&img, mainLayout->GetRect());
					return  true;
				}
				else if (deskTopWnd->IsVisible()) {
					deskTopWnd->Invalidate();
				}
				break;
			}
			break;
		}

		if (args.EventType == Event::OnMouseDoubleClick) {
			if (!sender->GetAttribute("FileHash").empty()) {
				UIString hash = sender->GetAttribute("FileHash");
				Song info;
				this->PlaySong(hash, info);
			}
			break;
		}

		if (args.EventType == Event::OnMouseDown) {
			if (sender->Name == "login") {
				OpenLoginFrm(sender);
				break;
			}
			if (sender->Name == "next") {
				NextSong();
				break;
			}
			if (sender->Name == "up") {
				UpSong();
				break;
			}
			if (sender->Name == "deskLrc") {
				OpenDesktopLrc();
				break;
			}
			if (sender->Name == "play") {
				player.Play();
				mediaCtl->SetPageIndex(1);
				mediaCtl->Invalidate();
				break;
			}
			if (sender->Name == "pause") {
				player.Pause();
				mediaCtl->SetPageIndex(0);
				mediaCtl->Invalidate();
				break;
			}
			if (sender->Name == "dellocal") {
				LocalItem* songItem = (LocalItem*)sender->Parent;
				vlistLocal->Remove(songItem);

				UIString hash = songItem->GetAttribute("FileHash");
				if (!hash.empty()) {
					listFile->DeleteSection(hash);
				}
				delete songItem;
				vlistLocal->Invalidate();
				return true;
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
				break;
			}
			if (!sender->GetAttribute("mvhash").empty()) {
				timer->Stop();
				UIString mvhash = sender->GetAttribute("mvhash");
				UIString songHash = sender->Parent->GetAttribute("FileHash");
				PlayMv(mvhash, songHash);
				break;
			}
			if (sender == playerBar) {
				const MouseEventArgs& arg = (MouseEventArgs&)args;
				double f_pos = arg.Location.X * 1.0 / playerBar->Width();
				player.SetPosition(f_pos);
				player.Play();
				break;
			}
		}
	} while (false);

	return __super::OnNotify(sender, args);
}

void MainFrm::OpenDesktopLrc()
{
	if (deskTopWnd->IsVisible()) {
		deskTopWnd->SetVisible(false);
	}
	else {
		deskTopWnd->SetVisible(true);
		deskTopWnd->Invalidate();
	}
}
void MainFrm::OpenLoginFrm(ezui::Control* sender)
{
	//测试代码
	LoginFrm loginFrm(Hwnd());

	//给窗口添加淡入效果
	Animation* ant = new Animation(&loginFrm);
	ant->SetStartValue(0.1);
	ant->SetEndValue(1.0);
	ant->ValueChanged = [&](double value) {
		HWND hWnd = loginFrm.Hwnd();
		BeginInvoke([&, value, hWnd] {
			if (!::IsWindow(hWnd))return;
			loginFrm.Opacity = value;//修改透明度
			loginFrm.Invalidate();//刷新
			});
		};
	loginFrm.Opacity = 0.1;
	ant->Start(300);//开始动画

	int code = loginFrm.ShowModal(true);//阻塞函数内部进行消息循环

	if (code == 1) {
		UIString text = UIString(L"欢迎您,%s").format(loginFrm.m_userName.c_str());
		((Label*)sender)->SetText(text);
		sender->Invalidate();
	}

}
void MainFrm::UpSong()
{
	int pos = this->FindLocalSong(this->nowSong);
	pos--;
	UIString hash;
	if (pos < 0) {
		hash = songLsit[songLsit.size() - 1].hash;
	}
	else {
		hash = songLsit[pos].hash;
	}
	auto it = vlistLocal->FindSingleChild("FileHash", hash);
	if (it) {
		vlistLocal->GetScrollBar()->ScrollTo(it);
		it->SendEvent(MouseEventArgs(Event::OnMouseDoubleClick));
	}
}
void MainFrm::NextSong()
{
	int pos = this->FindLocalSong(this->nowSong);
	pos++;
	UIString hash;
	if (pos >= songLsit.size()) {
		hash = songLsit[0].hash;
	}
	else {
		hash = songLsit[pos].hash;
	}
	auto it = vlistLocal->FindSingleChild("FileHash", hash);
	if (it) {
		vlistLocal->GetScrollBar()->ScrollTo(it);
		it->SendEvent(MouseEventArgs(Event::OnMouseDoubleClick));
	}
}
void MainFrm::PlayMv(const UIString& mvhash, const UIString& songHash)
{
	playType = 2;//当前正在播放视频

	Song info;
	global::GetMvInfo(mvhash, info);

	RequestNewImage(info);

	FindControl("mvView")->SendEvent(Event::OnMouseDown);

	this->SetText(info.SongName);
	((Label*)FindControl("songName"))->SetText(info.SongName);
	((Label*)FindControl("songName"))->Invalidate();

	UIString filehash = songHash;
	UIString lrcData = global::GetSongLrc(filehash);

	player.OpenUrl(info.url);
	player.Play();
	player.SetDuration(info.Duration / 1000);

	lrcPanel.LoadLrc(lrcData);
	deskTopWnd->LoadLrc(lrcData);
	timer->Start();
}
void MainFrm::RequestNewImage(Song& info)
{
	//重置头像和写真
	if (headImg) {
		delete headImg;
		labelSinger->Style.ForeImage = NULL;
		headImg = NULL;
		labelSinger->Invalidate();
	}
	if (bkImg) {
		delete bkImg;
		mainLayout->Style.ForeImage = NULL;
		deskTopWnd->GetLayout()->Style.BackImage = NULL;
		bkImg = NULL;
		mainLayout->Invalidate();
	}
	if (downloadTask) {
		delete downloadTask;
	}
	downloadTask = new Task([this](UIString singname, UIString imgUrl) {
		this->DownLoadImage(singname, imgUrl);
		}, info.SingerName, info.imgUrl);
}
void MainFrm::TimerTick() {

	Invoke([=]() {
		if (player.GetState() == libvlc_state_t::libvlc_Playing) {
			long long position = player.Position();
			auto duration = player.Duration();
			double rate = position / (duration * 1000.0);
			int w = playerBar->Width() * rate;

			lrcPanel.ChangePostion(position);
			if (deskTopWnd->IsVisible()) {
				deskTopWnd->ChangePostion(position);
			}

			UIString f1 = global::toTimeStr(position / 1000);
			UIString f2 = global::toTimeStr(duration);
			UIString fen = f1 + "/" + f2;

			if (mediaCtl->GetPageIndex() != 1) {
				mediaCtl->SetPageIndex(1);
				mediaCtl->Invalidate();
			}

			labelTime->SetText(fen);
			labelTime->Invalidate();

			playerBar2->SetFixedWidth(w);
			playerBar2->SetVisible(w > 0);
			playerBar2->Invalidate();
		}
		else {
			if (mediaCtl->GetPageIndex() != 0) {
				mediaCtl->SetPageIndex(0);
				mediaCtl->Invalidate();
			}
		}
		});
}

void MainFrm::NextPage(float scrollPos) {
	if (scrollPos >= 1.0f && global::nextPage) {
		global::page++;
		UIString keyword = editSearch->GetText();
		std::vector<Song> songs = global::SearchSongs(keyword);
		for (auto& it : songs) {
			SearchItem* sit = new SearchItem(it);
			vlistSearch->Add(sit);
		}
		if (!global::nextPage) {
			Label* end = new Label;
			end->SetFixedHeight(35);
			end->Style.BackColor = Color(254, 249, 229);
			end->SetText(L"已经没有更多数据");
			vlistSearch->Add(end);
		}
		vlistSearch->Invalidate();
	}
}
void MainFrm::OpenSongView() {
	centerLeft->Style.BackColor = Color::Transparent;
	tools->Style.Border.Bottom = 1;
	tools->Style.Border.Color = Color(238, 238, 238);
	vlistLocal->GetScrollBar()->Style.BackColor = Color(200, 200, 200, 50);
	vlistLocal->GetScrollBar()->Style.ForeColor = Color(217, 217, 217);
	vlistLocal->GetScrollBar()->ActiveStyle.ForeColor = Color(191, 191, 191);
	centerLayout->Style.BackColor = Color::White;
	centerLayout->Style.ForeColor = Color::Black;
	Invalidate();
}
void MainFrm::OpenLrcView() {
	centerLeft->Style.BackColor = Color(200, 200, 200, 100);
	tools->Style.Border.Bottom = 1;
	tools->Style.Border.Color = Color(238, 238, 238);
	vlistLocal->GetScrollBar()->Style.BackColor = Color(200, 200, 200, 50);
	vlistLocal->GetScrollBar()->Style.ForeColor = Color(255, 255, 255, 100);
	vlistLocal->GetScrollBar()->ActiveStyle.ForeColor = Color(255, 255, 255, 150);
	centerLayout->Style.BackColor = Color::Transparent;
	centerLayout->Style.ForeColor = Color::White;
	Invalidate();
}
