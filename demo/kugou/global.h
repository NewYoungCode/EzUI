#pragma once
#include "EzUI/EzUI.h"
#include "EzUI/TextBox.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/Application.h"
#include "EzUI/UIManager.h"
#include "EzUI/HLayout.h"
#include "EzUI/Label.h"
#include "EzUI/VListView.h"
#include "EzUI/HLayout.h"
#include "EzUI/VLayout.h"
#include "EzUI/Button.h"
#include "EzUI/TabLayout.h"
#include "EzUI/NotifyIcon.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/LayeredWindow.h"
#include "EzUI/UIManager.h"
#include "EzUI/Task.h"
#include "EzUI/UISelector.h"

#include "WebClient.h"
#include "JsonCpp.h"
#include "HttpUtility.h"
#include "base64.h"
#include "ConfigIni.h"

#ifdef NDEBUG
#pragma comment(lib,"Common.lib")
#else
#pragma comment(lib,"CommonD.lib")
#endif

//自定义通知栏消息
#define WM_NOTIFYICON1 WM_APP+1

using namespace EzUI;

struct Song {
	UIString hash;//歌曲哈希值
	UIString SongName;//歌曲名称
	UIString SingerName;//歌手名称
	UIString MvHash;//mv哈希值
	int Duration;//音乐时长 单位:秒
	UIString AlbumID;//
	UIString AlbumName;//
	int QualityLevel;//歌曲热度
	UIString url;
	UIString imgUrl;
	UIString fileName;
};


namespace global {
	extern int pageSize;
	extern int page;
	extern bool nextPage;

	//歌曲长度转字符串显示
	extern UIString toTimeStr(long dur);
	//http对酷狗api的请求
	extern int HttpGet(const UIString& url, UIString& resp);
	//根据关键字进行歌曲搜索
	extern std::vector<Song> SearchSongs(const UIString& keyword);
	//使用歌曲的AlbumId寻找查找对应的歌词
	extern UIString GetSongLrc(const UIString& hash, const UIString& AlbumID = "");
	//获取歌曲信息
	extern bool GetSongInfo(const UIString& hash, UIString& errorInfo, Song& info);
	//获取mv信息
	extern bool GetMvInfo(const UIString& mvhash, Song& info);
	//获取歌手写真(酷我的接口)
	extern UIString GetSingerBackground(const UIString& SingerName);
};
