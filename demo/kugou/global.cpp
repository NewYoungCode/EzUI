#include "global.h"



namespace global {

	int pageSize = 50;
	int page = 1;
	bool nextPage = true;

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
	bool GetSongInfo(const EString& hash, EString& errorInfo, Song& info)
	{
		EString url = "http://m.kugou.com/app/i/getSongInfo.php?hash={hash}&cmd=playInfo";
		Text::Replace(&url, "{hash}", hash);
		EString resp;
		global::HttpGet(url, resp);
		auto w = resp.unicode();
		JObject json(resp);
		if (json["errcode"].asInt() != 0) {
			errorInfo = EString(json["error"].asString());
			return false;
		}
		int dur = json["timeLength"].asInt();
		EString playUrl = json["url"].asCString();
		EString SingerName = json["author_name"].asCString();

		info.fileName = json["fileName"].asString();
		info.Duration = dur;
		info.url = playUrl;
		info.SingerName = SingerName;
		info.imgUrl = json["imgUrl"].asCString();

		if (playUrl.empty()) {
			errorInfo = L"歌曲收费";
			return false;
		}
		return true;
	}
	bool GetMvInfo(const EString& mvhash, Song& info) {
		EString resp;
		WebClient wc;
		wc.HttpGet("http://m.kugou.com/app/i/mv.php?cmd=100&hash=" + mvhash + "&ismp3=1&ext=mp4", resp);
		auto w = resp.unicode();
		JObject json(resp);
		std::vector<EString> urls;
		urls.reserve(6);
		for (auto& it : json["mvdata"]) {
			EString url = it["downurl"].asString();
			if (!url.empty()) {
				urls.push_back(url);
			}
		}

		info.Duration = json["timelength"].asInt();
		info.SongName = json["songname"].asString();
		info.url = urls[urls.size() - 1];
		return true;
	}
	EString GetSingerBackground(const EString& SingerName) {
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
		return bkurl;
	}
}
