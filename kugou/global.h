#pragma once
#include "EzUI.h"
#include "TextBox.h"
#include "Form.h"
#include "Application.h"
#include "UIManager.h"
#include "HLayout.h"
#include "Label.h"
#include "VList.h"
#include "HLayout.h"
#include "VLayout.h"
#include "WebClient.h"
#include "JsonCpp.h"
#include "HttpUtility.h"
#include "base64.h"
#include "ConfigIni.h"

#ifdef _DEBUG
#pragma comment(lib,"CommonD.lib")
#else
#pragma comment(lib,"Common.lib")
#endif

using namespace EzUI;
struct Song {
	EString hash;
	EString SongName;
	EString SingerName;
	EString MvHash;
	int Duration;
	EString AlbumID;//ר��ID
	EString AlbumName;//ר��
	int QualityLevel;//�ȶ�
};

class SongItem :public HBox {
public:
	Label songName;
	Label time;
	Label del;
	virtual ~SongItem() {
		if (del.Style.ForeImage) {
			delete del.Style.ForeImage;
		}
	}
	SongItem(const EString& _songName, const EString& _songTime = "03:56") {


		Style.FontSize = 12;

		//this->ShadowWidth = 5;
		del.Style.ForeImage = new Image(L"imgs/del.png");
		del.SetFixedSize({ 20,20 });
		del.Cursor = Cursor::HAND;
		del.Name = "dellocal";
		del.MousePassThrough = Event::OnHover;

		songName.SetText(_songName);
		songName.EllipsisText = "...";
		songName.TextAlign = TextAlign::MiddleLeft;
		songName.MousePassThrough = time.MousePassThrough = Event::OnHover | Event::OnActive | Event::OnMouseDoubleClick;
		songName.HoverStyle.FontSize = 15;

		time.SetFixedWidth(50);
		time.SetText(_songTime);
		time.TextAlign = TextAlign::MiddleRight;

		//time.Style.Radius = 33;
		//time.Style.BackgroundColor = Color(100,255, 255, 0);

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
		AddControl(&del);
		AddControl(new HSpacer(10));
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
	Label del;
public:

	virtual ~SongItem2() {
		if (mv.Style.ForeImage) {
			delete mv.Style.ForeImage;
		}
		if (del.Style.BackgroundImage) {
			delete del.Style.BackgroundImage;
		}
	}

	SongItem2(const Song& s) {

		song = s;
		this->Tag = (UINT_PTR)&song;
		SetAttribute("FileHash", s.hash);
		SetAttribute("SingerName", s.SingerName);

		//SetTips(s.SongName);
		SetFixedHeight(35);
		Style.BorderBottom = 1;
		Style.BorderColor = Color(245, 245, 245);
		HoverStyle.BackgroundColor = Color(245, 245, 245);

		songName.EllipsisText = L"...";
		songName.SetText(s.SongName);
		songName.TextAlign = TextAlign::MiddleLeft;
		songName.MousePassThrough = Event::OnHover | Event::OnMouseDoubleClick;
		songName.HoverStyle.ForeColor = Color::Orange;

		AlbumName.SetFixedWidth(180);
		AlbumName.SetText(s.AlbumName);
		AlbumName.TextAlign = TextAlign::MiddleLeft;
		AlbumName.Cursor = Cursor::HAND;
		AlbumName.Style.ForeColor = Color(150, 150, 150);
		AlbumName.MousePassThrough = Event::OnHover;

		mv.SetFixedWidth(35);
		mv.MousePassThrough = Event::OnHover;
		if (!s.MvHash.empty()) {
			mv.SetAttribute("mvhash", s.MvHash);
			mv.Style.ForeImage = new Image(L"imgs/mvicon.png");;
			mv.Style.ForeImage->Padding = 8;
			mv.Cursor = Cursor::HAND;
		}
		else {
			mv.MousePassThrough = Event::OnHover | Event::OnMouseDoubleClick;
		}

		time.SetFixedWidth(60);
		time.SetText(toTimeStr(s.Duration));
		time.TextAlign = TextAlign::MiddleLeft;
		time.MousePassThrough = Event::OnHover | Event::OnMouseDoubleClick;
		time.Style.ForeColor = Color(150, 150, 150);

		del.SetFixedWidth(33);

		EString fileName = "imgs/" + std::to_string(s.QualityLevel) + ".png";
		Image* img = new Image(fileName.utf16());
		img->Padding = 8;
		del.Style.BackgroundImage = img;

		AddControl(new HSpacer(15));
		AddControl(&songName);
		AddControl(&AlbumName);
		AddControl(&mv);
		AddControl(&del);
		AddControl(new HSpacer(5));
		AddControl(&time);
		AddControl(new HSpacer(5));
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
		WebClient wc;
		wc.AddHeader("Accept", " */*");
		wc.AddHeader("Accept-Language", " en-US,en;q=0.8,zh-Hans-CN;q=0.5,zh-Hans;q=0.2");
		//wc.AddHeader("Accept-Encoding", " gzip, deflate");//ע�͵� ��Ϊ�������᷵��gzip
		wc.AddHeader("User-Agent", " Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
		wc.AddHeader("Host", host);
		wc.AddHeader("Connection", " Keep-Alive");
		wc.AddHeader("Cache-Control", " no-cache");
		EString userid = "1581500898";//�ṷ���û�ID ������������󲻵���ȷ������
		newUrl += "&userid=" + userid;
		return wc.HttpGet(newUrl, resp);
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
};

class test {
public:
	int a;
	int b;
private:
	int c;
};