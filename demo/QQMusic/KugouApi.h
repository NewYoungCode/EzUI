#pragma once

#include <vector>

#include "EzUI/EzUI.h"


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

namespace kugou {

struct SongSummary {
    ezui::UIString hash;
    ezui::UIString songName;
    ezui::UIString singerName;
    ezui::UIString albumId;
    ezui::UIString albumName;
    ezui::UIString mvHash;
    ezui::UIString localPath;
    long long updatedAt = 0;
    int durationSeconds = 0;
    int qualityLevel = 0;
};

struct SongPlaybackInfo {
    ezui::UIString hash;
    ezui::UIString songName;
    ezui::UIString singerName;
    ezui::UIString displayName;
    ezui::UIString streamUrl;
    ezui::UIString imageUrl;
    int durationSeconds = 0;
};

ezui::UIString ToTimeString(int durationSeconds);
std::vector<SongSummary> SearchSongs(const ezui::UIString& keyword, int page, int pageSize, bool* hasMore, ezui::UIString* error = nullptr);
bool GetSongPlaybackInfo(const ezui::UIString& hash, const ezui::UIString& mvhash, SongPlaybackInfo& info, ezui::UIString* error = nullptr);
ezui::UIString GetSingerAvatar(const ezui::UIString& singerName);
ezui::UIString GetSongLyrics(const ezui::UIString& hash, const ezui::UIString& albumId = "", ezui::UIString* error = nullptr);
ezui::UIString GetSingerBackground(const ezui::UIString& singerName);
ezui::Image* DownloadImage(const ezui::UIString& url, int timeoutSeconds = 5);
bool DownloadFile(const ezui::UIString& url,
	const ezui::UIString& filePath,
	ezui::UIString* error = nullptr,
	int timeoutSeconds = 60,
	const std::function<void(long long dltotal, long long dlnow)>& progressCallback = nullptr);

} // namespace kugou
