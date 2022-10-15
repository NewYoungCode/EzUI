#include "MainFrm.h"
#define refreshImage WM_UIMESSAGE+1
#define delImage WM_UIMESSAGE+2

void MainFrm::InitForm() {
	this->Zoom = true;

	this->SetLayout(ui::UIManager::LoadLayout("xml/main.htm"));

	playingImage = new Image(L"imgs/play.png");
	pauseImage = new Image(L"imgs/pause.png");
	control = (TabLayout*)FindControl("control");
	//���������Ҫʹ��win32ԭ���ؼ������������ ������ Ȼ��Ϳ������win32ԭ���ؼ���(ע:layeredwindow��֧��ԭ���ؼ�)
	/*HWND btn = ::CreateWindowW(DATETIMEPICK_CLASS, L"hello world", WS_POPUP | WS_VISIBLE, 600, 10, 100, 30, NULL, NULL, GetModuleHandle(NULL), 0);
	::SetParent(btn, Hwnd());*/

	auto main2 = FindControl("main2");
	main2->Style.BackgroundColor = Color(120, 0, 0, 0);

	main = FindControl("main");
	center = FindControl("center");
	centerLeft = FindControl("centerLeft");
	tools = FindControl("tools");

	tabCtrl = (TabLayout*)FindControl("rightView");
	//MainLayout->Style.Radius =1;//Բ�Ǵ���
	//CloseShadow();//�رմ�����Ӱ

	//this is test
	FindControl("lrcView2")->AddControl(&lrcCtl);//��Ӹ�ʿؼ�

	localList = (VList*)this->FindControl("playList");

	//localList->AutoHeight = true;

	searchList = (VList*)this->FindControl("searchList");
	searchEdit = (Edit*)FindControl("searchEdit");
	//������౾���б�Ĺ�����
	localList->ScrollBar->SetFixedWidth(9);
	localList->ScrollBar->Style.Radius = 9;
	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(217, 217, 217);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(191, 191, 191);
	//���������б�Ĺ�����
	//searchList->ScrollBar->SetFixedWidth(9);
	searchList->ScrollBar->Style.Radius = 9;
	searchList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	searchList->ScrollBar->Style.ForeColor = Color(250, 200, 200, 200);
	searchList->ScrollBar->ActiveStyle.ForeColor = Color(250, 200, 200, 200);
	//�����������Ͻǵ���� ��С�� �رհ�ť
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

		it->SetTips(name);
		localList->AddControl(it);
	}

	searchList->ScrollBar->Rolling = [=](int a, int b)->void {
		NextPage(a, b);
	};

	FindControl("vlcDock")->AddControl(&player);
	SongView();

	playerBar = FindControl("playerBar");
	playerBar2 = FindControl("rate");
	playerBar2->MousePassThrough = Event::OnHover | Event::OnActive|Event::OnMouseClick;

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

	player.AddEventNotify(Event::OnPaint);
	main->AddEventNotify(Event::OnPaint);
}
MainFrm::MainFrm() :Form(1022, 670)
{
	InitForm();

	auto main = FindControl("main");
	//main->Style.Radius =5;
	//main->Style.BackgroundColor = Color(100, 255, 0, 0);
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
void MainFrm::DownLoadImage(EString _SingerName, EString headImageUrl)
{

	auto  SingerName =_SingerName.Split(",")[0];

	::SendMessageW(Hwnd(), delImage, 0, 0);

	WCHAR temp[256]{ 0 };
	::GetTempPathW(256, temp);
	EString cache = EString(temp) + "KuGou_Cache";
	::CreateDirectoryW(cache.utf16().c_str(), NULL);
	//���ظ���ͷ��
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
		//���ظ���д��
		auto rect = GetClientRect();
		EString imageUrl = "https://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&&id=0&name=" + HttpUtility::UrlEncode(SingerName) + "&from=pc&json=1&version=1&width=" + std::to_string(1920) + "&height=" + std::to_string(1080);
		EString resp;
		WebClient wc;
		wc.HttpGet(imageUrl, resp);
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
			wc2.DownloadFile(bkurl, Text::UTF8ToANSI(singerBkImg).c_str());
			bkImage = new Image(singerBkImg.utf16());
		}
		else {
			bkImage = new Image(L"imgs/defaultBackground.jpg");
		}
		bkImage->SizeMode = ImageSizeMode::CenterImage;
	}

	::SendMessageW(Hwnd(), refreshImage, 0, 0);
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
bool MainFrm::OnNotify(Control* sender,  EventArgs& args) {

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
				arg.Painter.DrawImage(&img, main->GetRect(), ImageSizeMode::CenterImage);
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

					localList->ScrollBar->Move(localList->ScrollBar->RollingTotal());
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
				::MessageBoxW(Hwnd(), L"�����շ�", L"ERROR", 0);
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
			return false;
		}
		if (sender->Name == "pause") {
			player.Pause();
			return false;
		}
		if (sender->Name == "del") {
			searchList->RemoveControl(sender->Parent);
			delete sender->Parent;
			return false;
		}
		if (sender->Name == "dellocal") {//ɾ������
			auto songItem = sender->Parent;
			EString hash = songItem->GetAttribute("FileHash");
			if (!hash.empty()) {
				cfg->DeleteSection(hash);
			}
			this;
			auto it = localList->RemoveControl(songItem);
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
			end->SetText(L"�Ѿ�û�и�������");
			searchList->AddControl(end);
		}
		searchList->Invalidate();
	}
}
void  MainFrm::SongView() {
	centerLeft->Style.BackgroundColor = Color(0, 0, 0, 0);
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);
	main->Style.BackgroundImage.valid = false;
	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(217, 217, 217);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(191, 191, 191);
	center->Style.BackgroundColor = Color::White;
	center->Style.ForeColor = Color::Black;
	main->Invalidate();
}
void  MainFrm::LrcView() {
	centerLeft->Style.BackgroundColor = Color(100, 200, 200, 200);
	tools->Style.BorderBottom = 1;
	tools->Style.BorderColor = Color(238, 238, 238);
	localList->ScrollBar->Style.BackgroundColor = Color(50, 200, 200, 200);
	localList->ScrollBar->Style.ForeColor = Color(100, 255, 255, 255);
	localList->ScrollBar->ActiveStyle.ForeColor = Color(150, 255, 255, 255);
	main->Style.BackgroundImage = bkImage;
	center->Style.BackgroundColor = Color(0, 0, 0, 0);
	center->Style.ForeColor = Color::White;
	main->Invalidate();
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
