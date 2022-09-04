#include "MainFrm.h"
void MainFrm::InitForm() {
	this->Zoom = true;
	CloseShadow();
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

	for (size_t i = 0; i < 5; i++)
	{
		SongItem* it = new SongItem(utf8("TG小辉-女孩(DJ版)"));
		localList->AddControl(it);
	}
	searchList->ScrollBar->Rolling = [=](int a, int b)->void {
		NextPage(a, b);
	};

	player.Style.FontSize = 15;
	player.Style.ForeColor = Color::White;
	player.TextAlign = TextAlign::TopLeft;
	player.SetText(L"EzUI框架QQ群:758485934");

	FindControl("vlcDock")->AddControl(&player);
	SongView();

	SetTimer(10);
}
MainFrm::MainFrm() :Form(1000, 670)
{
	InitForm();
}
MainFrm::~MainFrm()
{
}
void MainFrm::OnClose(bool& cal) {
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

				EString SingerName = sender->GetAttribute("SingerName");
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



				if (bkImage) {
					delete bkImage;
				}

				if (!bkurl.empty()) {
					WCHAR buf[513]{ 0 };
					::GetTempPathW(512, buf);
					std::wstring file(buf);
					Path::Create(EString(file + L"KuGou"));
					file += L"KuGou\\a.png";
					WebClient wc2;
					wc2.DownloadFile(bkurl, EString(file).c_str());
					bkImage = new Image(file);
				}
				else {
					bkImage = new Image(L"imgs/defaultBackground.png");
				}

				auto main = FindControl("main");
				bkImage->SizeMode = Image::SizeMode::CenterImage;

				Song* tag = (Song*)sender->Tag;
				SongItem* it = new SongItem(tag->SongName, toTimeStr(tag->Duration));
				localList->AddControl(it);
				localList->RefreshLayout();
				localList->ScrollBar->Move(localList->ScrollBar->RollingTotal());
				localList->Refresh();
				player.OpenUrl(playUrl);
				player.Play();
			}
			else {
				::MessageBoxW(Hwnd(), L"歌曲收费", L"ERROR", 0);
			}
			EString lrcData = global::GetSongLrc(hash);
			lrcCtl.LoadLrc(lrcData);

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
			//player.OpenPath("D:\\aa.mp4");
			player.Play();
		}
	}
	return __super::OnNotify(sender, args);
}
void MainFrm::OnTimer() {
	if (player.GetState() == libvlc_state_t::libvlc_Playing) {
		lrcCtl.ChangePostion(player.Position());
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
	main->Refresh();
}
void MainFrm::OnPaint(HDC DC, const Rect& rect) {

	if (0) {
		Painter pt(DC);
		static Bitmap* image = Gdiplus::Bitmap::FromFile(L"d:\\test.jpg");
		//客户端数据
		const int& clientWidth = rect.Width;
		const int& clientHeight = rect.Height;
		double clientRate = clientWidth * 1.0 / clientHeight;
		//图片数据
		int imgWidth = image->GetWidth();
		int imgHeight = image->GetHeight();
		double imgRate = imgWidth * 1.0 / imgHeight;
		if (clientRate < imgRate) {
			//1000 670 客户端
			//1000 300 图片
			//2233 670     缩放后的图片大小 
			int mabyeWidth = clientHeight * 1.0 / imgHeight * imgWidth + 0.5;//图片应该这么宽才对
			int x = (mabyeWidth - clientWidth) * 1.0 / 2 + 0.5;
			pt.DrawImage(image, { rect.X - x,rect.Y,mabyeWidth,clientHeight });
		}
		else {
			//1000 600 客户端
			//400  600 图片
			//1000 1500     缩放后的图片大小 
			int mabyeHeight = clientWidth * 1.0 / imgWidth * imgHeight + 0.5;//图片应该这么高才对
			int y = (mabyeHeight - clientHeight) * 1.0 / 2 + 0.5;
			pt.DrawImage(image, { rect.X,  rect.Y - y  , clientWidth, mabyeHeight });
		}
		return;
	}
	__super::OnPaint(DC, rect);
	//::StretchBlt(DC, 0, 0, destWidth, destHeight, hdc, 0, 0, 1000, 800, SRCCOPY);
	//::BitBlt(DC, 0, 0, destWidth, destHeight, hdc, srcHeight-aa,0, SRCCOPY);
}