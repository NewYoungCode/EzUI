#include "mainForm.h"
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

namespace {
	TaskFactory& KugouTaskPool()
	{
		static TaskFactory pool(4);
		return pool;
	}
}

MainFrm::MainFrm() : Form()
{
	InitForm();
	InitTrayIcon();
	InitControls();
	InitLocalPlaylist();
	InitEventHandlers();
	InitTimer();

	this->SetMinSize({ 800, 600 });
	this->SetSize({ 1020, 690 });

	// 启动后0.5秒自动搜索
	auto weakThis = this->GetWeakPtr<MainFrm>();
	Timer::Timeout(500, [weakThis]() {
		Invoke([weakThis] {
			if (!weakThis.IsAlive()) return;
			weakThis->editSearch->SetText("blackpink");
			weakThis->SearchSongs(weakThis->editSearch->GetText());
			});
		});
}

MainFrm::~MainFrm()
{
	++searchToken_;
	++playToken_;
	// Timer 需要先停止再删除
	if (timer) {
		timer->Stop();
		delete timer;
	}
	delete downloadTask;
	delete listFile;

	if (vlistSearch) vlistSearch->RemoveAll(true);
	if (vlistLocal) vlistLocal->RemoveAll(true);

	delete deskTopWnd;
}

//========== 初始化函数 ==========

void MainFrm::InitForm()
{
	this->SetResizable(true);
	this->SetText(L"酷苟音乐");
	this->LoadXml("res/xml/main.htm");

	// 找到三个 Frame
	titleFrame = this->FindControl<Frame>("titleFrame");
	centerFrame = this->FindControl<Frame>("centerFrame");
	bottomFrame = this->FindControl<Frame>("bottomFrame");

	// 统一设置通知处理
	auto notifyHandler = [this](Control* sender, EventArgs* args) {
		this->OnNotify(sender, args);
		};
	titleFrame->SetNotifyHandler(notifyHandler);
	centerFrame->SetNotifyHandler(notifyHandler);
	bottomFrame->SetNotifyHandler(notifyHandler);

	//////新api方式设置窗口圆角
	//DWMWINDOWATTRIBUTE attr = DWMWA_WINDOW_CORNER_PREFERENCE;
	//DWM_WINDOW_CORNER_PREFERENCE val = DWMWCP_ROUND;
	//HRESULT hr = DwmSetWindowAttribute(GetWindowHandle(), attr, &val, sizeof(val));
	//if (SUCCEEDED(hr)) {
	//	// 调用成功，但不保证视觉上真的生效（可能窗口类型不支持）
	//	this->CloseShadow();
	//}
	//else {
		// 不支持该属性或失败
	this->EnableAlphaBlending();
	this->GetLayout()->SetStyleSheet("border:1px solid rgba(128, 128, 128, 30%);border-radius:10px", VisualState::Normal);
	//}
}

void MainFrm::InitTrayIcon()
{
	ntfi.SetTips(L"酷苟音乐");
	ntfi.SetIcon(ezui::LoadIcon("res/icon.ico"));

	// 创建托盘菜单
	SystemMenu* menu = new SystemMenu(&ntfi);
	UINT_PTR id_open = menu->Append(L"打开主程序");
	UINT_PTR id_exit = menu->Append(L"退出");
	ntfi.SetMenu(menu);

	menu->MenuClick = [this, id_open, id_exit](UINT_PTR menuId) {
		if (menuId == id_open) {
			this->Restore();
		}
		else if (menuId == id_exit) {
			Application::Exit();
		}
		};

	// 双击托盘图标恢复窗口
	ntfi.EventHandler = [this](const MouseEventArgs& args) {
		if (args.EventType() == Event::MouseDoubleClick && args.Button() == MouseButton::Left) {
			this->Restore();
		}
		};
}

void MainFrm::InitControls()
{
	// 查找所有控件
	mainLayout = this->FindControl("mainLayout");
	tools = centerFrame->FindControl("tools");
	centerLayout = centerFrame->FindControl("centerLayout");
	centerLeft = centerFrame->FindControl("centerLeft");
	mediaCtl = bottomFrame->FindControl<TabControl>("mediaCtl");
	labelTime = bottomFrame->FindControl<Label>("labelTime");
	labelSinger = bottomFrame->FindControl<Label>("labelSinger");
	playerBar = bottomFrame->FindControl<Slider>("playerBar");
	tabCtrl = centerFrame->FindControl<TabControl>("rightView");
	vlistLocal = centerFrame->FindControl<VListView>("playList");
	vlistSearch = centerFrame->FindControl<VListView>("searchList");
	editSearch = titleFrame->FindControl<TextBox>("searchEdit");
	labelDeskLrc = bottomFrame->FindControl<CheckBox>("deskLrc");
	player = centerFrame->FindControl<VlcPlayer>("player");
	lrcPanel = centerFrame->FindControl<LrcPanel>("lrcPanel");

	// 设置滚动条自动隐藏
	vlistSearch->GetScrollBar()->SetAutoHide(500);

	// 创建桌面歌词窗口
	deskTopWnd = new DesktopLrcFrm(player);

	// 设置背景图片缩放属性
	if (mainLayout->Style->BackImage) {
		mainLayout->Style->BackImage->SizeMode = ImageSizeMode::Cover;
	}

	// 打开默认显示界面
	OpenSongView();
}

void MainFrm::InitLocalPlaylist()
{
	listFile = new IniConfig(Path::StartPath() + "\\list.ini");

	for (const auto& section : listFile->GetSections()) {

		Song s;
		s.SongName = listFile->ReadString(section, "name");
		s.hash = section;
		s.Duration = listFile->ReadInt(section, "dur");
		s.SingerName = listFile->ReadString(section, "singer");
		songLsit.push_back(s);

		// 创建列表项
		LocalItem* item = new LocalItem(s.SongName, global::toTimeStr(s.Duration));
		item->SetAttribute("FileHash", section);
		item->SetAttribute("SingerName", s.SingerName);
		item->SetToolTip(s.SongName);
		vlistLocal->AddChild(item, true);

		//vlistLocal->Append(
		//	L"<label class=\"testXml\" text=\"测试xml加载文字\" "
		//	L"style=\"width:100px;height:20px;font-size:12px;\" />"
		//);
	}
}

void MainFrm::InitEventHandlers()
{
	// 搜索框回车事件
	editSearch->AddEventHandler(Event::KeyDown, [this](Control*, EventArgs* args_) {
		auto* args = static_cast<KeyEventArgs*>(args_);
		if (args->KeyCode() == VK_RETURN) {
			UIString keyword = editSearch->GetText();
			SearchSongs(keyword);
		}
		});

	// 滚动条滚动到底部加载更多
	vlistSearch->GetScrollBar()->ValueChanged = [this](ScrollBar*, int pos) {
		if (pos >= vlistSearch->GetScrollBar()->GetMaxValue()) {
			NextPage(pos);
		}
		};

	// 视频帧回调
	player->PlayingCallback = [this](Image*) {
		if (playType == 2 && tabCtrl->GetPageIndex() == 1) {
			this->Invalidate();
		}
		if (deskTopWnd->IsVisible()) {
			deskTopWnd->GetLayout()->Style->BackImage = nullptr;
			deskTopWnd->Invalidate();
		}
		};

	// 主布局绘制事件
	mainLayout->AddEventHandler(Event::Paint, [this](Control*, EventArgs* args_) {
		if (playType == 2 && tabCtrl->GetPageIndex() == 1) {
			if (deskTopWnd->IsVisible()) return;

			player->play_mtx.lock();
			auto* args = static_cast<PaintEventArgs*>(args_);
			if (player->play_img) {
				player->play_img->SizeMode = ImageSizeMode::Cover;
				args->Graphics()->DrawImage(player->play_img, mainLayout->GetRect());
			}
			player->play_mtx.unlock();
		}
		});

	// 进度条点击事件
	playerBar->AddEventHandler(Event::MouseUp, [this](void*, void*) {
		player->SetPosition(playerBar->GetRatio());
		player->Play();
		});
}

void MainFrm::InitTimer()
{
	timer = new Timer;
	timer->SetInterval(100);

	auto weakThis = this->GetWeakPtr<MainFrm>();
	timer->SetTickHandler([weakThis](Timer*) {
		BeginInvoke([weakThis]() {
			if (weakThis.IsAlive()) {
				weakThis->TimerTick();
			}
			});
		});
}

//========== 事件处理 ==========

void MainFrm::OnPaint(PaintEventArgs* args)
{
	__super::OnPaint(args);
}

void MainFrm::OnClose(bool& bClose)
{
	++searchToken_;
	++playToken_;
	/*bClose = false;
	Animation* ani = new Animation(this);
	ani->SetStartValue(1.0);
	ani->SetEndValue(0);
	ani->ValueChanged = [](float value) {
		if (value <= 0.1) {
			Application::Exit(0);
		}
		};
	ani->Start(200);*/
	auto ret = ::MessageBoxW(GetWindowHandle(), L"真的要退出吗？", L"提示", MB_OKCANCEL);
	if (ret == IDOK) {
		bClose = true;
		Application::Exit(0);
	}
	else {
		bClose = false;
	}
	__super::OnClose(bClose);
}

void MainFrm::OnShow()
{
	//Animation* ani = new Animation(this);
	//ani->SetStartValue(0);
	//ani->SetEndValue(1);
	//ani->ValueChanged = [this](float) {
	//	this->Invalidate();
	//	};
	//ani->Start(200);

	__super::OnShow();
}

void MainFrm::OnKeyDown(WPARAM wparam, LPARAM lParam)
{
	__super::OnKeyDown(wparam, lParam);
}

void MainFrm::OnNotify(Control* sender, EventArgs* args)
{
	__super::OnNotify(sender, args);

	UIString name = sender->GetName();
	UIString fileHash = sender->GetAttribute("FileHash");

	if (args->EventType() == Event::MouseDoubleClick && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
		if (!fileHash.empty()) {
			PlaySong(fileHash);
		}
		return;
	}
	if (args->EventType() == Event::MouseDown && args->As<MouseEventArgs>()->Button() == MouseButton::Left) {
		// 使用映射表简化条件判断
		if (name == "login") {
			OpenLoginFrm(sender);
		}
		else if (name == "next") {
			NextSong();
		}
		else if (name == "up") {
			UpSong();
		}
		else if (name == "deskLrc") {
			bool opened = OpenDesktopLrc();
			::MessageBoxW(GetWindowHandle(),
				opened ? L"桌面歌词已开启" : L"桌面歌词已关闭", L"", MB_OK);
		}
		else if (name == "play") {
			player->Play();
			mediaCtl->SetPageIndex(1);
			mediaCtl->Invalidate();
		}
		else if (name == "pause") {
			player->Pause();
			mediaCtl->SetPageIndex(0);
			mediaCtl->Invalidate();
		}
		else if (name == "dellocal") {
			LocalItem* songItem = static_cast<LocalItem*>(sender->GetParent());
			UIString hash = songItem->GetAttribute("FileHash");
			if (!hash.empty()) {
				listFile->DeleteSection(hash);
			}
			vlistLocal->RemoveChild(songItem, true);
			vlistLocal->Invalidate();
			vlistLocal->RefreshLayout();
		}
		else if (sender->HasClass("testXml") || name == "gif") {
			sender->GetParent()->RemoveChild(sender, true);
		}
		else if (sender->GetAttribute("tabcontrol") == "rightView") {
			size_t pos = sender->GetParent()->IndexOf(sender);
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
		else if (!sender->GetAttribute("mvhash").empty()) {
			timer->Stop();
			UIString mvhash = sender->GetAttribute("mvhash");
			UIString songHash = sender->GetParent()->GetAttribute("FileHash");
			PlayMv(mvhash, songHash);
		}
	}
}

//========== 播放控制 ==========

void MainFrm::PlaySong(const UIString& hash)
{
	timer->Stop();

	const int token = ++playToken_;
	auto weakThis = GetWeakPtr<MainFrm>();

	KugouTaskPool().AddToFirst([weakThis, token, hash]() {
		Song info;
		UIString errStr;
		const bool ok = global::GetSongInfo(hash, errStr, info);
		UIString lrcData;
		if (ok) {
			lrcData = global::GetSongLrc(hash);
		}

		BeginInvoke([weakThis, token, hash, ok, errStr, info, lrcData]() {
			if (!weakThis.IsAlive() || token != weakThis->playToken_) {
				return;
			}

			if (!ok) {
				::MessageBoxW(weakThis->GetWindowHandle(), L"无法播放(歌曲收费)", errStr.unicode().c_str(), MB_OK);
				return;
			}

			weakThis->playType = 1;

			MouseEventArgs args(Event::MouseDown, MouseButton::Left);
			weakThis->centerFrame->FindControl("lrcView")->SendEvent(&args);

			// 如果是新歌曲，添加到列表
			if (weakThis->FindLocalSong(hash) == size_t(-1)) {
				Song storedInfo = info;
				storedInfo.hash = hash;
				weakThis->songLsit.push_back(storedInfo);

				LocalItem* item = new LocalItem(info.fileName, global::toTimeStr(info.Duration));
				item->SetAttribute("FileHash", hash);
				item->SetAttribute("SingerName", info.SingerName);

				weakThis->vlistLocal->AddChild(item);
				weakThis->vlistLocal->RefreshLayout();
				weakThis->vlistLocal->GetScrollBar()->ScrollTo(item);
				weakThis->vlistLocal->Invalidate();

				weakThis->listFile->WriteString(hash, "name", info.fileName);
				weakThis->listFile->WriteString(hash, "singer", info.SingerName);
				weakThis->listFile->WriteString(hash, "dur", std::to_string(info.Duration));
			}

			weakThis->RequestNewImage(info);

			weakThis->nowSong = hash;
			weakThis->SetText(info.fileName);
			weakThis->bottomFrame->FindControl<Label>("songName")->SetText(info.fileName);
			weakThis->ntfi.ShowMessage(L"播放音乐", info.fileName, 2000);

			weakThis->player->OpenUrl(info.url);
			weakThis->player->SetDuration(info.Duration);
			weakThis->player->Play();

			weakThis->lrcPanel->LoadLrc(lrcData);
			weakThis->deskTopWnd->LoadLrc(lrcData);

			weakThis->timer->Start();
			});
		});
}

void MainFrm::PlayMv(const UIString& mvhash, const UIString& songHash)
{
	const int token = ++playToken_;
	auto weakThis = GetWeakPtr<MainFrm>();

	KugouTaskPool().AddToFirst([weakThis, token, mvhash, songHash]() {
		Song info;
		const bool ok = global::GetMvInfo(mvhash, info);
		UIString lrcData;
		if (ok) {
			lrcData = global::GetSongLrc(songHash);
		}

		BeginInvoke([weakThis, token, ok, info, lrcData]() {
			if (!weakThis.IsAlive() || token != weakThis->playToken_) {
				return;
			}

			if (!ok) {
				::MessageBoxW(weakThis->GetWindowHandle(), L"无法播放当前 MV", L"错误", MB_OK);
				return;
			}

			weakThis->playType = 2;
			weakThis->RequestNewImage(info);

			MouseEventArgs args(Event::MouseDown, MouseButton::Left);
			weakThis->centerFrame->FindControl("mvView")->SendEvent(&args);

			weakThis->SetText(info.SongName);
			weakThis->bottomFrame->FindControl<Label>("songName")->SetText(info.SongName);
			weakThis->bottomFrame->FindControl<Label>("songName")->Invalidate();

			weakThis->player->OpenUrl(info.url);
			weakThis->player->Play();
			weakThis->player->SetDuration(info.Duration / 1000);

			weakThis->lrcPanel->LoadLrc(lrcData);
			weakThis->deskTopWnd->LoadLrc(lrcData);
			weakThis->timer->Start();
			});
		});
}

void MainFrm::UpSong()
{
	int pos = static_cast<int>(FindLocalSong(nowSong)) - 1;
	UIString hash = (pos < 0)
		? songLsit.back().hash
		: songLsit[pos].hash;

	auto it = vlistLocal->FindChildren("FileHash", hash).First();
	if (it) {
		vlistLocal->GetScrollBar()->ScrollTo(it);
		MouseEventArgs args(Event::MouseDoubleClick, MouseButton::Left);
		it->SendEvent(&args);
	}
}

void MainFrm::NextSong()
{
	size_t pos = FindLocalSong(nowSong) + 1;
	UIString hash = (pos >= songLsit.size())
		? songLsit.front().hash
		: songLsit[pos].hash;

	auto it = vlistLocal->FindChildren("FileHash", hash).First();
	if (it) {
		vlistLocal->GetScrollBar()->ScrollTo(it);
		MouseEventArgs args(Event::MouseDoubleClick, { 0, 0 }, MouseButton::Left);
		it->SendEvent(&args);
	}
}

void MainFrm::TimerTick()
{
	if (player->GetState() == libvlc_state_t::libvlc_Playing) {
		long long position = player->Position();
		auto duration = player->Duration();

		lrcPanel->ChangePostion(position);
		if (deskTopWnd->IsVisible()) {
			deskTopWnd->ChangePostion(position);
		}

		UIString timeStr = global::toTimeStr(position / 1000) + "/" + global::toTimeStr(duration);

		if (mediaCtl->GetPageIndex() != 1) {
			mediaCtl->SetPageIndex(1);
			mediaCtl->Invalidate();
		}

		labelTime->SetText(timeStr);
		labelTime->Invalidate();

		if (playerBar->PressedButton() != MouseButton::Left) {
			playerBar->SetMinValue(0);
			playerBar->SetMaxValue(duration * 1000);
			playerBar->SetValue(position);
			playerBar->Invalidate();
		}
	}
	else {
		if (mediaCtl->GetPageIndex() != 0) {
			mediaCtl->SetPageIndex(0);
			mediaCtl->Invalidate();
		}
	}
}

//========== 搜索相关 ==========

void MainFrm::SearchSongs(const ezui::UIString& keyword)
{
	++searchToken_;
	const int token = searchToken_;
	searchPage_ = 0;
	hasMoreSearchResults_ = false;
	isSearchLoading_ = true;

	MouseEventArgs args(Event::MouseDown, MouseButton::Left);
	centerFrame->FindControl("songView")->SendEvent(&args);

	vlistSearch->RemoveAll(true);
	vlistSearch->Invalidate();

	auto weakThis = GetWeakPtr<MainFrm>();
	KugouTaskPool().Add([weakThis, token, keyword]() {
		bool hasMore = false;
		std::vector<Song> songs = global::SearchSongs(keyword, 1, &hasMore);

		BeginInvoke([weakThis, token, hasMore, songs]() {
			if (!weakThis.IsAlive() || token != weakThis->searchToken_) {
				return;
			}

			weakThis->searchPage_ = 1;
			weakThis->hasMoreSearchResults_ = hasMore;
			weakThis->isSearchLoading_ = false;
			weakThis->vlistSearch->RemoveAll(true);

			for (const auto& song : songs) {
				weakThis->vlistSearch->AddChild(new SearchItem(song));
			}

			if (songs.empty()) {
				Label* empty = new Label;
				empty->SetFixedHeight(35);
				empty->SetText(L"没有搜索到歌曲");
				weakThis->vlistSearch->AddChild(empty);
			}

			weakThis->vlistSearch->Invalidate();
			});
		});
}

void MainFrm::NextPage(float scrollPos)
{
	if (scrollPos < 1.0f || !hasMoreSearchResults_ || isSearchLoading_) return;

	isSearchLoading_ = true;
	const int token = searchToken_;
	const int nextPage = searchPage_ + 1;
	UIString keyword = editSearch->GetText();
	auto weakThis = GetWeakPtr<MainFrm>();

	KugouTaskPool().Add([weakThis, token, nextPage, keyword]() {
		bool hasMore = false;
		std::vector<Song> songs = global::SearchSongs(keyword, nextPage, &hasMore);

		BeginInvoke([weakThis, token, nextPage, hasMore, songs]() {
			if (!weakThis.IsAlive() || token != weakThis->searchToken_) {
				return;
			}

			weakThis->isSearchLoading_ = false;
			weakThis->searchPage_ = nextPage;
			weakThis->hasMoreSearchResults_ = hasMore;

			for (const auto& song : songs) {
				weakThis->vlistSearch->AddChild(new SearchItem(song));
			}

			if (!hasMore) {
				Label* end = new Label;
				end->SetFixedHeight(35);
				end->Style->BackColor = Color(254, 249, 229);
				end->SetText(L"已经没有更多数据");
				weakThis->vlistSearch->AddChild(end);
			}
			weakThis->vlistSearch->Invalidate();
			});
		});
}

size_t MainFrm::FindLocalSong(const UIString& hash)
{
	for (size_t i = 0; i < songLsit.size(); i++) {
		if (songLsit[i].hash == hash) {
			return i;
		}
	}
	return size_t(-1);
}

//========== 界面切换 ==========

void MainFrm::OpenSongView()
{
	centerLeft->Style->BackColor = Color::Transparent;
	tools->Style->Border.Bottom = 1;
	tools->Style->Border.Color = Color(238, 238, 238);
	vlistLocal->GetScrollBar()->Style->BackColor = Color(200, 200, 200, 50);
	vlistLocal->GetScrollBar()->Style->ForeColor = Color(217, 217, 217);
	vlistLocal->GetScrollBar()->ActiveStyle->ForeColor = Color(191, 191, 191);
	centerLayout->Style->BackColor = Color::White;
	centerLayout->Style->ForeColor = Color::Black;
	Invalidate();
}

void MainFrm::OpenLrcView()
{
	centerLeft->Style->BackColor = Color(200, 200, 200, 100);
	tools->Style->Border.Bottom = 1;
	tools->Style->Border.Color = Color(238, 238, 238);
	vlistLocal->GetScrollBar()->Style->BackColor = Color(200, 200, 200, 50);
	vlistLocal->GetScrollBar()->Style->ForeColor = Color(255, 255, 255, 100);
	vlistLocal->GetScrollBar()->ActiveStyle->ForeColor = Color(255, 255, 255, 150);
	centerLayout->Style->BackColor = Color::Transparent;
	centerLayout->Style->ForeColor = Color::White;
	Invalidate();
}

bool MainFrm::OpenDesktopLrc()
{
	bool visible = !deskTopWnd->IsVisible();
	deskTopWnd->SetVisible(visible);
	if (visible) {
		deskTopWnd->Invalidate();
	}
	return visible;
}

void MainFrm::OpenLoginFrm(ezui::Control* sender)
{
	LoginFrm loginFrm(GetWindowHandle());
	loginFrm.SetSize(Size(300, 215));
	loginFrm.CenterToWindow(this->GetWindowHandle());

	Animation* ant = new Animation(&loginFrm);
	ant->SetStartValue(0.1);
	ant->SetEndValue(1.0);
	ant->ValueChanged = [&loginFrm](double value) {
		loginFrm.SetOpacity(value);
		loginFrm.Invalidate();
		};
	ant->Start(300);

	if (loginFrm.ShowModal(true) == 1) {
		UIString text = UIString(L"欢迎您,") + loginFrm.m_userName;
		static_cast<Label*>(sender)->SetText(text);
		sender->Invalidate();
	}
}

//========== 资源管理 ==========

void MainFrm::ClearImages()
{
	// 释放歌手头像
	labelSinger->Free(labelSinger->Style->BackImage);
	labelSinger->Style->BackImage = nullptr;
	labelSinger->Invalidate();

	// 释放主界面的背景图
	mainLayout->Free(mainLayout->Style->BackImage);
	mainLayout->Style->BackImage = nullptr;
	mainLayout->Invalidate();

	// 释放桌面歌词窗口的背景图
	deskTopWnd->GetLayout()->Free(deskTopWnd->GetLayout()->Style->BackImage);
	deskTopWnd->GetLayout()->Style->BackImage = nullptr;
}

void MainFrm::RequestNewImage(const Song& info)
{
	ClearImages();

	auto weakThis = this->GetWeakPtr<MainFrm>();//用于判断this是否存活

	UIString singers = info.SingerName;
	UIString headImageUrl = info.imgUrl;

	delete downloadTask;
	//创建线程去下载图片
	downloadTask = new Task([=]() {
		// 因为一首歌有多个歌手 随机选一个歌手头像就行了
		auto strs = singers.split("、");
		Random rdom;
		int pos = rdom.Next(0, static_cast<int>(strs.size()) - 1);
		UIString SingerName = strs[pos];

		Image* headImg = nullptr;
		Image* bkImg = nullptr;

		// 下载歌手头像
		{
			std::string headFileData;
			WebClient wc;
			if (wc.HttpGet(headImageUrl.replace("{size}", "400"), &headFileData, 5) == 200) {
				headImg = new Image(headFileData.c_str(), headFileData.size());
				headImg->SizeMode = ImageSizeMode::Cover;
			}
		}

		// 下载歌手写真
		{
			UIString bkurl = global::GetSingerBackground(SingerName);
			if (!bkurl.empty()) {
				std::string fileData;
				WebClient wc;
				if (wc.HttpGet(bkurl, &fileData, 5) == 200) {
					bkImg = new Image(fileData.c_str(), fileData.size());
					bkImg->SizeMode = ImageSizeMode::Cover;
				}
			}
		}

		auto cleanup = ([headImg, bkImg]() {
			delete headImg;
			delete bkImg;
			});

		// 回到主线程设置图片
		bool postDone = BeginInvoke([=]() {
			if (!weakThis) {
				cleanup();
				return;// 如果窗口已经被销毁了，就不设置图片了
			}

			auto headImg_ = headImg;
			auto bkImg_ = bkImg;

			// 如果下载到了歌手头像就设置到界面上，否则显示默认背景色
			if (!headImg_) {
				headImg_ = Image::Make("res/imgs/headImg.jpg");
			}

			labelSinger->Style->BackImage = labelSinger->Attach(headImg_);
			// 如果下载到了背景图就设置到主界面和桌面歌词窗口
			if (!bkImg_) {
				bkImg_ = Image::Make("res/imgs/defaultBackground.png");
				bkImg_->SizeMode = ImageSizeMode::Cover;
			}
			mainLayout->Style->BackImage = mainLayout->Attach(bkImg_);
			deskTopWnd->GetLayout()->Style->BackImage = deskTopWnd->GetLayout()->Attach(bkImg_->Clone());

			labelSinger->Invalidate();
			mainLayout->Invalidate();
			});

		//向UI线程投递任务失败时释放图片资源
		if (!postDone) {
			cleanup();
		}
		});
}
