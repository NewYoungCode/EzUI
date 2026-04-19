#pragma once
#include "EzUI/EzUI.h"

#include "Common.h"
#include "Util.h"


#ifdef _WIN64

#ifdef NDEBUG
#pragma comment(lib,"x64/Common.lib")
#else
#pragma comment(lib,"x64/Commond.lib")
#endif

#else

#ifdef NDEBUG
#pragma comment(lib,"x86/Common.lib")
#else
#pragma comment(lib,"x86/Commond.lib")
#endif

#endif



#include <random>
class Random {
	std::mt19937 randomer;
public:
	Random() :randomer(std::random_device()()) {
	}
	int Next(int lower, int upper) {
		std::uniform_int_distribution<int> distribution(lower, upper);
		return distribution(randomer);
	}
	float Next(float lower, float upper) {
		std::uniform_real_distribution<float> distribution(lower, upper);
		return distribution(randomer);
	}
};

using namespace ezui;

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

	//歌曲长度转字符串显示
	extern UIString toTimeStr(long dur);
	//http对酷狗api的请求
	extern int HttpGet(const UIString& url, UIString& resp);
	//根据关键字进行歌曲搜索
	extern std::vector<Song> SearchSongs(const UIString& keyword, int page, bool* hasNextPage = nullptr);
	//使用歌曲的AlbumId寻找查找对应的歌词
	extern UIString GetSongLrc(const UIString& hash, const UIString& AlbumID = "");
	//获取歌曲信息
	extern bool GetSongInfo(const UIString& hash, UIString& errorInfo, Song& info);
	//获取mv信息
	extern bool GetMvInfo(const UIString& mvhash, Song& info);
	//获取歌手写真(酷我的接口)
	extern UIString GetSingerBackground(const UIString& SingerName);
};
