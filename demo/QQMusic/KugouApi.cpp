#include "KugouApi.h"

#include "JsonCpp.hpp"
#include "Util.h"
#include "WebClient.h"

using namespace ezui;

namespace kugou {
	namespace {

		constexpr const char* kSearchApi = "https://songsearch.kugou.com/song_search_v2?platform=WebFilter&pagesize=%d&page=%d&keyword=";

		UIString StripMarkup(UIString text)
		{
			text = text.replace("<em>", "");
			text = text.replace("</em>", "");
			text = text.replace("<EM>", "");
			text = text.replace("</EM>", "");
			return text;
		}

		int HttpGet(const UIString& url, UIString& response)
		{
			UIString requestUrl = url;
			UIString host;
			size_t schemePos = requestUrl.find("://");
			if (schemePos != UIString::npos) {
				host = requestUrl.substr(schemePos + 3);
				schemePos = host.find("/");
				if (schemePos != UIString::npos) {
					host = host.substr(0, schemePos);
				}
			}

			WebClient client;
			client.AddHeader("Accept", "*/*");
			client.AddHeader("Accept-Language", "zh-CN,zh;q=0.9,en;q=0.8");
			client.AddHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/124.0.0.0 Safari/537.36");
			client.AddHeader("Host", host);
			client.AddHeader("Connection", "Keep-Alive");
			client.AddHeader("Cache-Control", "no-cache");

			if (requestUrl.find("userid=") == UIString::npos) {
				requestUrl += requestUrl.find("?") == UIString::npos ? "?userid=1581500898" : "&userid=1581500898";
			}

			return client.HttpGet(requestUrl, &response, 10);
		}

		UIString GetSingerImageUrl(const UIString& singerName, bool preferWide, int width, int height)
		{
			if (singerName.trim().empty()) {
				return {};
			}

			UIString imageUrl = "https://artistpicserver.kuwo.cn/pic.web?type=big_artist_pic&pictype=url&content=list&&id=0&name=" +
				Util::UrlEncode(singerName) +
				"&from=pc&json=1&version=1&width=" + std::to_string(width) +
				"&height=" + std::to_string(height);

			UIString response;
			WebClient client;
			if (client.HttpGet(imageUrl, &response, 5) != 200) {
				return {};
			}

			auto json = Json::Parse(response);
			const char* preferredKey = preferWide ? "wpurl" : "bkurl";
			const char* fallbackKey = preferWide ? "bkurl" : "wpurl";

			for (auto&& item : json["array"]) {
				if (!item[preferredKey].isNull()) {
					return item[preferredKey].asString();
				}
			}

			for (auto&& item : json["array"]) {
				if (!item[fallbackKey].isNull()) {
					return item[fallbackKey].asString();
				}
			}

			return {};
		}

		bool GetMvPlaybackInfo(const UIString& mvhash, SongPlaybackInfo& info, UIString* error)
		{
			if (mvhash.trim().empty()) {
				if (error) {
					*error = L"歌曲暂时无法播放。";
				}
				return false;
			}

			UIString response;

			{
				UIString resp;
				WebClient wc;
				wc.HttpGet("http://m.kugou.com/app/i/mv.php?cmd=100&hash=" + mvhash + "&ismp3=1&ext=mp4", &response);
			}
		

			auto w = response.unicode();

			auto json = Json::Parse(response);
			std::vector<UIString> urls;
			urls.reserve(6);
			for (auto& it : json["mvdata"]) {
				UIString streamUrl = it["downurl"].asString();
				if (!streamUrl.empty()) {
					urls.push_back(streamUrl);
				}
			}

			if (urls.empty()) {
				if (error) {
					*error = L"歌曲暂时无法播放。";
				}
				return false;
			}

			info.imageUrl = json["mvicon"].asString();
			info.singerName = StripMarkup(json["singer"].asString());
			info.songName = StripMarkup(json["songname"].asString());
			info.displayName = info.songName;
			info.durationSeconds = json["timelength"].asInt() / 1000;
			info.streamUrl = urls.back();
			return true;
		}

	} // namespace

	UIString ToTimeString(int durationSeconds)
	{
		UIString minutes = std::to_string(durationSeconds / 60);
		if (minutes.size() < 2) {
			minutes = "0" + minutes;
		}

		UIString seconds = std::to_string(durationSeconds % 60);
		if (seconds.size() < 2) {
			seconds = "0" + seconds;
		}

		return minutes + ":" + seconds;
	}

	std::vector<SongSummary> SearchSongs(const UIString& keyword, int page, int pageSize, bool* hasMore, UIString* error)
	{
		std::vector<SongSummary> songs;
		if (hasMore) {
			*hasMore = false;
		}

		const UIString trimmed = keyword.trim();

		char buffer[1024] = { 0 };
		sprintf(buffer, kSearchApi, pageSize, page);

		UIString response;
		const UIString url = UIString(buffer) + Util::UrlEncode(trimmed);
		if (HttpGet(url, response) != 200) {
			if (error) {
				*error = L"搜索请求失败，请稍后再试。";
			}
			return songs;
		}

		auto json = Json::Parse(response);
		if (json["error_code"].asInt() != 0 || json["status"].asInt() != 1) {
			if (error) {
				*error = L"搜索接口返回异常。";
			}
			return songs;
		}

		const int total = json["data"]["total"].asInt();
		const int pageCount = static_cast<int>(total / static_cast<double>(pageSize) + 0.999);
		if (hasMore) {
			*hasMore = page < pageCount;
		}

		for (auto&& item : json["data"]["lists"]) {
			SongSummary song;
			song.hash = item["FileHash"].asString();
			song.durationSeconds = item["Duration"].asInt();
			song.mvHash = item["MvHash"].asString();
			song.songName = StripMarkup(item["SongName"].asString());
			if (song.songName.empty()) {
				song.songName = StripMarkup(item["FileName"].asString());
			}
			song.singerName = StripMarkup(item["SingerName"].asString());
			song.albumId = item["AlbumID"].asString();
			song.albumName = StripMarkup(item["AlbumName"].asString());
			song.qualityLevel = item["QualityLevel"].asInt();
			songs.push_back(song);
		}

		return songs;
	}

	bool GetSongPlaybackInfo(const UIString& hash, const ezui::UIString& mvhash, SongPlaybackInfo& info, UIString* error)
	{
		info = {};
		if (!GetMvPlaybackInfo(mvhash, info, error)) {
			return false;
		}

		info.hash = hash;
		if (info.displayName.empty()) {
			info.displayName = info.songName.empty() ? hash : info.songName;
		}
		return !info.streamUrl.empty();
	}

	UIString GetSingerAvatar(const UIString& singerName)
	{
		return GetSingerImageUrl(singerName, false, 640, 640);
	}

	UIString GetSongLyrics(const UIString& hash, const UIString& albumId, UIString* error)
	{
		UIString url = "http://krcs.kugou.com/search?ver=1&man=yes&client=mobi&keyword=&duration=&hash=" + hash + "&album_audio_id=" + albumId;
		UIString response;
		if (HttpGet(url, response) != 200) {
			if (error) {
				*error = L"歌词获取失败。";
			}
			return UIString(L"[00:00.00]暂无歌词");
		}

		auto json = Json::Parse(response);
		if (json["status"].asInt() != 200 || json["candidates"].size() == 0) {
			if (error) {
				*error = L"当前歌曲没有可用歌词。";
			}
			return UIString(L"[00:00.00]暂无歌词");
		}

		const UIString id = (*json["candidates"].begin())["id"].asString();
		const UIString accesskey = (*json["candidates"].begin())["accesskey"].asString();

		response.clear();
		url = "http://lyrics.kugou.com/download?ver=1&client=pc&id=" + id + "&accesskey=" + accesskey + "&fmt=lrc&charset=utf8";
		if (HttpGet(url, response) != 200) {
			if (error) {
				*error = L"歌词下载失败。";
			}
			return UIString(L"[00:00.00]暂无歌词");
		}

		auto content = Json::Parse(response)["content"].asString();
		content = Util::Base64Decode(content);
		return content.empty() ? UIString(L"[00:00.00]暂无歌词") : content;
	}

	UIString GetSingerBackground(const UIString& singerName)
	{
		return GetSingerImageUrl(singerName, true, 1920, 1080);
	}

	Image* DownloadImage(const UIString& url, int timeoutSeconds)
	{
		if (url.empty()) {
			return nullptr;
		}

		UIString imageUrl = url;
		if (imageUrl.contains("{size}")) {
			imageUrl = imageUrl.replace("{size}", "400");
		}

		std::string fileData;
		WebClient client;
		if (client.HttpGet(imageUrl, &fileData, timeoutSeconds) != 200 || fileData.empty()) {
			return nullptr;
		}

		auto* image = new Image(fileData.c_str(), static_cast<int>(fileData.size()));
		image->SizeMode = ImageSizeMode::Cover;
		return image;
	}

	bool DownloadFile(const UIString& url,
		const UIString& filePath,
		UIString* error,
		int timeoutSeconds,
		const std::function<void(long long dltotal, long long dlnow)>& progressCallback)
	{
		if (url.empty() || filePath.empty()) {
			if (error) {
				*error = L"下载参数不完整。";
			}
			return false;
		}

		WebClient client;
		if (client.DownloadFile(url, filePath.unicode(), progressCallback, timeoutSeconds) != 200) {
			if (error) {
				*error = L"下载失败，请稍后再试。";
			}
			return false;
		}

		return true;
	}

} // namespace kugou
