#include "global.h"


LocalItem::~LocalItem() {
	if (del.Style.ForeImage) {
		delete del.Style.ForeImage;
	}
}
void LocalItem::OnChildPaint(PaintEventArgs& args) {
	__super::OnChildPaint(args);
	//置顶 绘制删除线
	//args.Graphics.SetColor(Color::Red);
	//args.Graphics.FillRectangle(Rect(0,17, 308, 2),0);
}
LocalItem::LocalItem(const EString& _songName, const EString& _songTime) {

	Style.FontSize = 12;
	//this->Style.Angle = 5;
	//this->HoverStyle.Angle =6;
	//time.HoverStyle.Angle = 180;
	//this->ShadowWidth = 5;
	del.Style.ForeImage = Image::Make(L"imgs/del.png");
	del.SetFixedSize({ 20,20 });
	del.Style.Cursor = LoadCursor(Cursor::HAND);
	del.Name = "dellocal";
	del.EventPassThrough = Event::OnHover;

	this->Name = "songItem";

	songName.Name = "autosz";
	songName.SetText(_songName);
	songName.Ellipsis = "...";//文本超出容器之外采用省略号
	songName.TextAlign = TextAlign::MiddleLeft;
	songName.SetTips("child_" + _songName);
	songName.EventPassThrough = time.EventPassThrough = Event::OnHover | Event::OnActive | Event::OnMouseDoubleClick;
	songName.HoverStyle.FontSize = 15;

	/*	songName.Padding.Left = 10;
		songName.SetAutoHeight(true);
		songName.SetUnderline(0,5);*/

	time.SetFixedWidth(50);
	time.SetText(_songTime);
	time.TextAlign = TextAlign::MiddleRight;
	time.HoverStyle.FontSize = 20;

	this->SetFixedHeight(33);
	HoverStyle.BackColor = Color(230, 230, 230, 100);
	ActiveStyle.BackColor = Color(255, 230, 230, 100);
	ActiveStyle.FontSize = 10;

	Add(new HSpacer(15));
	Add(&songName);
	Add(&time);
	Add(new HSpacer(15));
	Add(&del);
	Add(new HSpacer(10));
}

SearchItem::~SearchItem() {
	if (mv.Style.ForeImage) {
		delete mv.Style.ForeImage;
	}
	if (del.Style.BackImage) {
		delete del.Style.BackImage;
	}
}
SearchItem::SearchItem(const Song& s) {
	song = s;
	this->Tag = &song;
	SetAttribute("FileHash", s.hash);
	SetAttribute("SingerName", s.SingerName);

	SetTips(s.SongName);
	SetFixedHeight(35);
	Style.Border.Bottom = 1;
	Style.Border.Color = Color(245, 245, 245);
	HoverStyle.BackColor = Color(245, 245, 245);

	songName.Ellipsis = L"...";
	songName.SetText(s.SongName);
	songName.TextAlign = TextAlign::MiddleLeft;
	songName.EventPassThrough = Event::OnHover | Event::OnMouseDoubleClick;
	songName.HoverStyle.ForeColor = Color(200, 100, 1);
	AlbumName.SetFixedWidth(180);
	AlbumName.SetText(s.AlbumName);
	AlbumName.TextAlign = TextAlign::MiddleLeft;
	AlbumName.Style.Cursor = LoadCursor(Cursor::HAND);
	AlbumName.Style.ForeColor = Color(150, 150, 150);
	AlbumName.EventPassThrough = Event::OnHover;

	mv.SetFixedWidth(35);
	mv.EventPassThrough = Event::OnHover;
	if (!s.MvHash.empty()) {
		mv.SetAttribute("mvhash", s.MvHash);
		mv.Style.ForeImage = Image::Make(L"imgs/mvicon.png");;
		mv.Margin = 8;
		mv.Style.Cursor = LoadCursor(Cursor::HAND);
	}
	else {
		mv.EventPassThrough = Event::OnHover | Event::OnMouseDoubleClick;
	}

	time.SetFixedWidth(60);
	time.SetText(global::toTimeStr(s.Duration));
	time.TextAlign = TextAlign::MiddleLeft;
	time.EventPassThrough = Event::OnHover | Event::OnMouseDoubleClick;
	time.Style.ForeColor = Color(150, 150, 150);

	del.SetFixedWidth(33);

	EString fileName = "imgs/" + std::to_string(s.QualityLevel) + ".png";
	Image* img = Image::Make(fileName.unicode());
	del.Style.BackImage = img;
	del.Margin = 8;

	Add(new HSpacer(15));
	Add(&songName);
	Add(&AlbumName);
	Add(&mv);
	Add(&del);
	Add(new HSpacer(5));
	Add(&time);
	Add(new HSpacer(5));
}

namespace global {
	int pageSize = 50;
	int page = 1;
	bool nextPage = true;
	HWND _workerw = nullptr;

	EString toTimeStr(long dur) {
		EString fen = std::to_string(dur / 60);
		if (fen.size() <= 1) fen = "0" + fen;
		EString yu = std::to_string(dur % 60);
		if (yu.size() <= 1) yu = "0" + yu;
		return fen + ":" + yu;
	}
	int HttpGet(const EString& url, EString& resp) {
		EString newUrl = url;
		size_t pos1 = newUrl.find("://");
		EString host;
		if (pos1 != EString::npos) {
			host = newUrl.substr(pos1 + 3);
			pos1 = host.find("/");
			if (pos1 != EString::npos) {
				host = host.substr(0, pos1);
			}
		}
		WebClient wc;
		wc.AddHeader("Accept", " */*");
		wc.AddHeader("Accept-Language", " en-US,en;q=0.8,zh-Hans-CN;q=0.5,zh-Hans;q=0.2");
		wc.AddHeader("User-Agent", " Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		wc.AddHeader("Host", host);
		wc.AddHeader("Connection", " Keep-Alive");
		wc.AddHeader("Cache-Control", " no-cache");
		EString userid = "1581500898";//
		newUrl += "&userid=" + userid;
		return wc.HttpGet(newUrl, resp);
	}
	std::vector<Song> SearchSongs(const EString& keyword) {
		char buf[999]{ 0 };
		EString resp;
		sprintf(buf, "https://songsearch.kugou.com/song_search_v2?platform=WebFilter&pagesize=%d&page=%d&keyword=%s", pageSize, page, HttpUtility::UrlEncode(keyword).c_str());
		HttpGet(buf, resp);
		JObject json(resp);
		int total = json["data"]["total"].asInt();
		int pageCount = total * 1.0 / pageSize + 0.9;
		if (page >= pageCount) {
			nextPage = false;
		}
		std::vector<Song> songs;
		for (auto&& it : json["data"]["lists"]) {
			Song s;
			s.hash = it["FileHash"].asString();
			s.Duration = it["Duration"].asInt();
			s.MvHash = it["MvHash"].asString();
			s.SongName = it["FileName"].asString();
			s.SingerName = it["SingerName"].asString();
			s.AlbumID = it["AlbumID"].asString();
			s.AlbumName = it["AlbumName"].asString();
			s.QualityLevel = it["QualityLevel"].asInt();
			songs.push_back(s);
		}
		return songs;
	}
	EString GetSongLrc(const EString& hash, const EString& AlbumID) {
		EString url = "http://krcs.kugou.com/search?ver=1&man=yes&client=mobi&keyword=&duration=&hash=" + hash + "&album_audio_id=" + AlbumID;
		EString resp;
		HttpGet(url, resp);
		JObject json(resp);
		if (json["status"].asInt() != 200 || json["candidates"].size() == 0) {
			return EString(L"[00:00.00]无歌词");
		}
		EString id = (*json["candidates"].begin())["id"].asString();
		EString accesskey = (*json["candidates"].begin())["accesskey"].asString();
		resp.clear();
		url = "http://lyrics.kugou.com/download?ver=1&client=pc&id=" + id + "&accesskey=" + accesskey + "&fmt=lrc&charset=utf8";
		HttpGet(url, resp);
		JObject json2(resp);
		EString base64Text = json2["content"].asString();
		base64Text = base64_decode(base64Text);
		auto gbkLrc = Text::UTF8ToANSI(base64Text);
		return base64Text;
	}
	BOOL CALLBACK EnumWindowsProc(_In_ HWND tophandle, _In_ LPARAM topparamhandle)
	{
		HWND defview = FindWindowEx(tophandle, 0, L"SHELLDLL_DefView", nullptr);
		if (defview != nullptr)
		{
			_workerw = FindWindowEx(0, tophandle, L"WorkerW", 0);
		}
		return true;
	}
	HWND GetWorkerW() {
		int result;
		HWND windowHandle = FindWindow(L"Progman", nullptr);
		::SendMessageTimeout(windowHandle, 0x052c, 0, 0, SMTO_NORMAL, 0x3e8, (PDWORD_PTR)&result);
		EnumWindows(EnumWindowsProc, (LPARAM)nullptr);
		ShowWindow(_workerw, SW_HIDE);
		return windowHandle;
	}
}
