#include "global.h"

namespace global {

	int pageSize = 50;
	int page = 1;
	bool nextPage = true;

	UIString toTimeStr(long dur) {
		UIString fen = std::to_string(dur / 60);
		if (fen.size() <= 1) fen = "0" + fen;
		UIString yu = std::to_string(dur % 60);
		if (yu.size() <= 1) yu = "0" + yu;
		return fen + ":" + yu;
	}
	int HttpGet(const UIString& url, UIString& resp) {
		UIString newUrl = url;
		size_t pos1 = newUrl.find("://");
		UIString host;
		if (pos1 != UIString::npos) {
			host = newUrl.substr(pos1 + 3);
			pos1 = host.find("/");
			if (pos1 != UIString::npos) {
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
		UIString userid = "1581500898";//
		newUrl += "&userid=" + userid;
		return wc.HttpGet(newUrl, &resp);
	}
	std::vector<Song> SearchSongs(const UIString& keyword) {
		char buf[999]{ 0 };
		UIString resp;
		sprintf(buf, "https://songsearch.kugou.com/song_search_v2?platform=WebFilter&pagesize=%d&page=%d&keyword=%s", pageSize, page, Util::UrlEncode(keyword).c_str());
		HttpGet(buf, resp);
		JsonValue json(resp);
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
	UIString GetSongLrc(const UIString& hash, const UIString& AlbumID) {
		UIString url = "http://krcs.kugou.com/search?ver=1&man=yes&client=mobi&keyword=&duration=&hash=" + hash + "&album_audio_id=" + AlbumID;
		UIString resp;
		HttpGet(url, resp);
		JsonValue json(resp);
		if (json["status"].asInt() != 200 || json["candidates"].size() == 0) {
			return UIString(L"[00:00.00]无歌词");
		}
		UIString id = (*json["candidates"].begin())["id"].asString();
		UIString accesskey = (*json["candidates"].begin())["accesskey"].asString();
		resp.clear();
		url = "http://lyrics.kugou.com/download?ver=1&client=pc&id=" + id + "&accesskey=" + accesskey + "&fmt=lrc&charset=utf8";
		HttpGet(url, resp);
		JsonValue json2(resp);
		UIString base64Text = json2["content"].asString();
		base64Text = base64_decode(base64Text);
		return base64Text;
	}
	bool GetSongInfo(const UIString& hash, UIString& errorInfo, Song& info)
	{
		UIString url = "http://m.kugou.com/app/i/getSongInfo.php?hash={hash}&cmd=playInfo";
		url = url.replace("{hash}", hash);
		UIString resp;
		global::HttpGet(url, resp);
		auto w = resp.unicode();
		JsonValue json(resp);
		if (json["errcode"].asInt() != 0) {
			errorInfo = UIString(json["error"].asString());
			return false;
		}
		int dur = json["timeLength"].asInt();
		UIString playUrl = json["url"].asCString();
		UIString SingerName = json["author_name"].asCString();

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
	bool GetMvInfo(const UIString& mvhash, Song& info) {
		UIString resp;
		WebClient wc;
		wc.HttpGet("http://m.kugou.com/app/i/mv.php?cmd=100&hash=" + mvhash + "&ismp3=1&ext=mp4", &resp);
		auto w = resp.unicode();
		JsonValue json(resp);
		std::vector<UIString> urls;
		urls.reserve(6);
		for (auto& it : json["mvdata"]) {
			UIString url = it["downurl"].asString();
			if (!url.empty()) {
				urls.push_back(url);
			}
		}

		info.imgUrl = json["mvicon"].asString();
		info.SingerName = json["singer"].asString();
		info.Duration = json["timelength"].asInt();
		info.SongName = json["songname"].asString();
		info.url = urls[urls.size() - 1];
		return true;
	}
	UIString GetSingerBackground(const UIString& SingerName) {
		UIString imageUrl = "https://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&&id=0&name=" + Util::UrlEncode(SingerName) + "&from=pc&json=1&version=1&width=" + std::to_string(1920) + "&height=" + std::to_string(1080);
		UIString resp;
		WebClient wc;
		wc.HttpGet(imageUrl, &resp, 5);
		JsonValue json(resp);
		UIString bkurl;
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
