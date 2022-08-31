#include "MainFrm.h"
void MainFrm::InitForm() {
	this->Zoom = true;
	//CloseShadow();
	this->SetLayout(ui::UIManager::LoadLayout("xml/main.htm"));
	auto main2 = FindControl("main2");
	main2->Style.BackgroundColor = Color(120, 0, 0, 0);

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

	FindControl("vlcDock")->AddControl(&player);
	SongView();
}
MainFrm::MainFrm() :Form(1004, 670)
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

				WCHAR buf[513]{ 0 };
				::GetTempPathW(512, buf);
				std::wstring file(buf);
				Path::Create(EString(file + L"KuGou"));
				file += L"KuGou\\a.png";

				WebClient wc2;
				wc2.DownloadFile(bkurl, EString(file).c_str());

				auto main = FindControl("main");

				bkImage = new Image(file);
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
			//global::GetSongLrc(hash);
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



#include <cmath>
using namespace std;

int gcd(int a, int b)
{
	if (b == 0) { return a; }
	return gcd(b, a % b);
}
std::pair<int, int> ratioFunc(int a, int b)
{
	std::pair<int, int> ret;
	if (a == 0)
	{
		ret.first = 0; ret.second = b;
	}
	else
	{
		int gc = gcd(abs(a), abs(b));
		a /= gc; b /= gc;
		ret.first = a; ret.second = b;
	}
	return ret;
}
void MainFrm::OnPaint(HDC DC, const Rect& rect) {
	
	if (0) {
		Painter pt(DC);
		//static Bitmap* img = Gdiplus::Bitmap::FromFile(L"d:\\dd.png");
		auto img = bkImage;
		//客户端数据
		auto& _rect = GetRect();
		int destWidth = _rect.Width;
		int destHeight = _rect.Height;
		//auto destRatioWidth = ratioFunc(destWidth, destHeight).first;
		//auto destRatioHeight = ratioFunc(destWidth, destHeight).second;
		double destRate = destWidth * 1.0 / destHeight;
		//图片数据
		int srcWidth = img->GetWidth();
		int srcHeight = img->GetHeight();
		//auto srctRatioWidth = ratioFunc(srcWidth, srcHeight).first;
		//auto srcRatioHeight = ratioFunc(srcWidth, srcHeight).second;

		StopWatch st;

		double srcRate = srcWidth * 1.0 / srcHeight;
		if (destRate < srcRate) {
			int mabyeWidth = destHeight * 1.0 / srcHeight * srcWidth + 0.5;//图片应该这么宽才对
			int offset = mabyeWidth - destWidth;
			EBitmap temp(mabyeWidth, destHeight);
			Gdiplus::Graphics gp(temp.GetHDC());
			gp.DrawImage(img, 0, 0, mabyeWidth, destHeight);
			//RECT l{ 0,0,mabyeWidth, destHeight };
			//SaveHDCToFile(temp.GetHDC(),&l, L"image/jpeg", L"d:/test.jpg");
			::BitBlt(DC, 0, 0, destWidth, destHeight, temp.GetHDC(), offset / 2, 0, SRCCOPY);



			Debug::Log("slow %d ms", st.ElapsedMilliseconds());
		}
		else {
			//1000 * 200 客户端
		   //1000  * 300 图片
			int mabyeHeight = destWidth * 1.0 / srcWidth * srcHeight + 0.5;//图片应该这么宽才对
			int offset = mabyeHeight - destHeight;
			EBitmap temp(destWidth, mabyeHeight);
			Gdiplus::Graphics gp(temp.GetHDC());
			gp.DrawImage(img, 0, 0, destWidth, mabyeHeight);
			//RECT l{ 0,0,mabyeHeight, destHeight };
			//SaveHDCToFile(temp.GetHDC(),&l, L"image/jpeg", L"d:/test.jpg");
			::BitBlt(DC, 0, 0, destWidth, destHeight, temp.GetHDC(), 0, offset / 2, SRCCOPY);
			Debug::Log("slow %d ms", st.ElapsedMilliseconds());
		}
	}
	__super::OnPaint(DC, rect);
	//::StretchBlt(DC, 0, 0, destWidth, destHeight, hdc, 0, 0, 1000, 800, SRCCOPY);
	//::BitBlt(DC, 0, 0, destWidth, destHeight, hdc, srcHeight-aa,0, SRCCOPY);
}