#pragma once
#include "EzUI.h"
#include "Edit.h"
#include "Form.h"
#include "Application.h"
#include "UIManager.h"
#include "HLayout.h"
#include "Label.h"
#include "VList.h"

#include "os.h"
#include "WebClient.h"
#include "JsonCpp.h"
#include "HttpUtility.h"
#include "base64.h"
#include "ConfigIni.h"

using namespace EzUI;
struct Song {
	EString hash;
	EString SongName;
	EString SingerName;
	EString MvHash;
	int Duration;
	EString AlbumID;//专辑ID
	EString AlbumName;//专辑
	int QualityLevel;//热度
};

extern Image* mvicon;

class SongItem :public HBox {
public:
	Label songName;
	Label time;

	SongItem(const EString& _songName, const EString& _songTime = "03:56") {
		songName.SetText(_songName);
		songName.TextAlign = TextAlign::MiddleLeft;
		songName.MousePassThrough = time.MousePassThrough = Event::OnHover | Event::OnActive | Event::OnMouseDoubleClick;

		time.SetFixedWidth(50);
		time.SetText(_songTime);
		time.TextAlign = TextAlign::MiddleRight;

		this->SetFixedHeight(33);
		this->Dock = DockStyle::Horizontal;
		HoverStyle.BackgroundColor = Color(100, 230, 230, 230);

		ActiveStyle.BackgroundColor = Color(100, 255, 230, 230);

		//Style.BackgroundColor = Color(100, 255, 20, 0);
		//Style.Radius = 33;

		AddControl(new HSpacer(15));
		AddControl(&songName);
		AddControl(&time);
		AddControl(new HSpacer(15));

	}
};

inline EString toTimeStr(long dur) {
	EString fen = std::to_string(dur / 60);
	if (fen.size() <= 1) fen = "0" + fen;
	EString yu = std::to_string(dur % 60);
	if (yu.size() <= 1) yu = "0" + yu;
	return fen + ":" + yu;
}

class SongItem2 :public HBox {
	Label songName;
	Label AlbumName;
	Label mv;
	Label time;
	Song song;
public:
	SongItem2(const Song& s) {
		song = s;
		this->Tag = (UINT_PTR)&song;
		SetAttribute("FileHash", s.hash);
		SetAttribute("SingerName", s.SingerName);

		SetFixedHeight(35);
		Dock = DockStyle::Horizontal;
		Style.BorderBottom = 1;
		Style.BorderColor = Color(245, 245, 245);
		HoverStyle.BackgroundColor = Color(245, 245, 245);

		songName.SetText(s.SongName);
		songName.TextAlign = TextAlign::MiddleLeft;
		songName.MousePassThrough = Event::OnHover | Event::OnMouseDoubleClick;
		songName.HoverStyle.ForeColor = Color::Orange;

		AlbumName.SetFixedWidth(180);
		AlbumName.SetText(utf8("《") + s.AlbumName + utf8("》"));
		AlbumName.TextAlign = TextAlign::MiddleLeft;
		AlbumName.Cursor = Cursor::HAND;
		AlbumName.Style.ForeColor = Color(150, 150, 150);
		AlbumName.MousePassThrough = Event::OnHover;

		mv.SetFixedWidth(35);
		mv.MousePassThrough = Event::OnHover;
		if (!s.MvHash.empty()) {
			mv.SetAttribute("mvhash", s.MvHash);
			mv.Style.ForeImage = mvicon;
			mv.Style.ForeImage.value->Margin = 8;
			//mv.Style.ForeImage.value->Box = Rect(8, 8, 19, 19);
			mv.Cursor = Cursor::HAND;

		}
		else {
			mv.MousePassThrough = Event::OnHover | Event::OnMouseDoubleClick;
		}

		time.SetFixedWidth(80);
		time.SetText(toTimeStr(s.Duration));
		time.TextAlign = TextAlign::MiddleLeft;
		time.MousePassThrough = Event::OnHover | Event::OnMouseDoubleClick;
		time.Style.ForeColor = Color(150, 150, 150);

		AddControl(new HSpacer(15));
		AddControl(&songName);
		AddControl(&AlbumName);
		AddControl(&mv);
		AddControl(&time);
		AddControl(new HSpacer(15));

	}
};
namespace global {

	extern int pageSize;
	extern int page;
	extern bool nextPage;

	inline int HttpGet(const EString& url, EString& resp) {
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
		EString userid = "1581500868";//酷狗的用户ID 不传入可能请求不到正确的数据
		WebClient wc;
		wc.AddHeader("Accept", " */*");
		wc.AddHeader("Accept-Language", " en-US,en;q=0.8,zh-Hans-CN;q=0.5,zh-Hans;q=0.2");
		//wc.AddHeader("Accept-Encoding", " gzip, deflate");//注释掉 因为服务器会返回gzip
		wc.AddHeader("User-Agent", " Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		wc.AddHeader("Host", host);
		wc.AddHeader("Connection", " Keep-Alive");
		wc.AddHeader("Cache-Control", " no-cache");
		return wc.HttpGet(newUrl + "&userid=" + userid, resp);
	}

	inline std::vector<Song> SearchSongs(const EString& keyword) {
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

	inline EString GetSongLrc(const EString& hash, const EString& AlbumID = "") {
		EString url = "http://krcs.kugou.com/search?ver=1&man=yes&client=mobi&keyword=&duration=&hash=" + hash + "&album_audio_id=" + AlbumID;
		EString resp;
		HttpGet(url, resp);
		JObject json(resp);

		if (json["status"].asInt() != 200) {
			return utf8("[00:00.00]酷苟音乐");
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
};