#pragma once
#include "EzUI/EzUI.h"
#include "EzUI/TextBox.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/Application.h"
#include "EzUI/UIManager.h"
#include "EzUI/HLayout.h"
#include "EzUI/Label.h"
#include "EzUI/VList.h"
#include "EzUI/HLayout.h"
#include "EzUI/VLayout.h"
#include "EzUI/Button.h"
#include "EzUI/TabLayout.h"
#include "EzUI/NotifyIcon.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/LayeredWindow.h"
#include "EzUI/UIManager.h"
#include "EzUI/Task.h"

#include "resource.h"

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
	EString hash;//歌曲哈希值
	EString SongName;//歌曲名称
	EString SingerName;//歌手名称
	EString MvHash;//mv哈希值
	int Duration;//音乐时长 单位:秒
	EString AlbumID;//
	EString AlbumName;//
	int QualityLevel;//歌曲热度
};

/// <summary>
/// 左侧本地歌曲列表中的Item
/// </summary>
class LocalItem :public HBox {
public:
	Label songName;
	Label time;
	Label del;
	virtual ~LocalItem();
	virtual void OnChildPaint(PaintEventArgs& args)override;
	LocalItem(const EString& _songName, const EString& _songTime = "03:56");
};
/// <summary>
/// 搜索列表中的Item
/// </summary>
class SearchItem :public HBox {
	Label songName;
	Label AlbumName;
	Label mv;
	Label time;
	Song song;
	Label del;
public:
	virtual ~SearchItem();
	SearchItem(const Song& s);
};

namespace global {
	extern int pageSize;
	extern int page;
	extern bool nextPage;
	extern HWND _workerw;

	//歌曲长度转字符串显示
	extern EString toTimeStr(long dur);
	//http对酷狗api的请求
	extern int HttpGet(const EString& url, EString& resp);
	//根据关键字进行歌曲搜索
	extern std::vector<Song> SearchSongs(const EString& keyword);
	//使用歌曲的AlbumId寻找查找对应的歌词
	extern EString GetSongLrc(const EString& hash, const EString& AlbumID = "");
	//获取桌面句柄 用于桌面歌词显示
	extern HWND GetWorkerW();
};
