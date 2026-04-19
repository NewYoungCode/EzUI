#pragma once

#include <atomic>
#include <memory>
#include <vector>

#include "EzUI/EzUI.h"
#include "Common.h"

#include "KugouApi.h"

class LrcPanel;
class MusicSeekBar;
class RecordCover;
class SongRowItem;
class VlcPlayer;

class MainForm : public ezui::BorderlessWindow {
public:
    MainForm();
    ~MainForm() override;

protected:
    void OnShow() override;
    void OnClose(bool& cancel) override;

private:
	struct DownloadTaskState {
		kugou::SongSummary song;
		float progress = 0.0f;
		long long updatedAt = 0;
	};

    enum class ContentPage {
        Home = 0,
        Search = 1,
        Library = 2,
        Detail = 3,
    };

    enum class LibraryView {
        Likes,
        Recent,
        Download,
        Daily30,
    };

    void InitControls();
    void InitEvents();
    void LoadLibraryData();
    void ShowPage(ContentPage page, bool rememberPrevious = true);
    void NavigateBack();
    void OpenLibrary(LibraryView view);

    void StartSearch(const ezui::UIString& keyword, bool autoPlayFirst = false);
    void SearchSinger(const ezui::UIString& singerName);
    void RequestSearchPage(int page, bool append);
    void RenderSearchResults(const std::vector<kugou::SongSummary>& songs, bool append);

    void PopulateLibraryList();
    void RefreshDailySongs(bool forceRefresh = false);
    void RefreshHomeRecommendations();
    void OpenRecommendedList();
    void AppendSongRows(ezui::VListView* list, const std::vector<kugou::SongSummary>& songs, int startIndex, bool libraryMode);
    void RenderEmptyState(ezui::VListView* list, const ezui::UIString& text);
    void UpdateSearchRowStates();
    void UpdateLibraryRowStates();
    void UpdateRowStates(ezui::VListView* list, const ezui::UIString& selectedHash);

    void PlaySongFromSearch(int index);
    void PlaySongFromLibrary(int index);
    void PlayRecommendedSong();
    void PlaySongAt(int index);
    void TogglePlayPause();
    void PlayPreviousSong();
    void PlayNextSong();
    void SeekFromSlider(ezui::Slider* slider);
    void UpdatePlayButtonState();
    void UpdateNowPlaying(const kugou::SongSummary& song, const kugou::SongPlaybackInfo& info);
    void UpdateProgress();
    void LoadLyricsAsync(const kugou::SongSummary& song, int token);
    void UpdateArtworkAsync(const kugou::SongSummary& song, const kugou::SongPlaybackInfo& info, int token);
    void SetControlBackImage(ezui::Control* control, ezui::Image* image, ezui::ImageSizeMode sizeMode = ezui::ImageSizeMode::Cover);

    void ToggleFavorite(const kugou::SongSummary& song);
    void PushRecent(const kugou::SongSummary& song);
    void MarkDownloaded(const kugou::SongSummary& song, const ezui::UIString& localPath);
    void DownloadSong(const kugou::SongSummary& song);
    void DownloadSongs(const std::vector<kugou::SongSummary>& songs, size_t maxCount);
    void ClearCurrentLibrary();

    void UpdateSidebarCounts();
    void UpdateSidebarSelection();
    void UpdateHomeSummary();
    void UpdateFavoriteButtons();
    void UpdateDetailStatus(const ezui::UIString& text);
    void SetSearchMeta(const ezui::UIString& text);
    void SetLibraryMeta(const ezui::UIString& text);
    void SetSidebarStatus(const ezui::UIString& text);
    void AnimateControlOpacity(std::unique_ptr<ezui::Animation>& animation,
        const std::vector<ezui::Control*>& controls,
        float from,
        float to,
        int durationMs,
        ezui::EasingCurve easing = ezui::EasingCurve::EaseOut);
    void AnimatePageReveal(ezui::Control* control, float from = 0.72f, int durationMs = 240);

    const kugou::SongSummary* GetSelectedSearchSong() const;
    const kugou::SongSummary* GetSelectedLibrarySong() const;
    const kugou::SongSummary* GetPrimaryRecommendation() const;
    int FindSongIndexByHash(const std::vector<kugou::SongSummary>& songs, const ezui::UIString& hash) const;
    int FindActiveDownloadIndex(const ezui::UIString& hash) const;
    bool IsFavorite(const ezui::UIString& hash) const;
    float GetActiveDownloadProgress(const ezui::UIString& hash) const;
    void BeginDownloadTracking(const kugou::SongSummary& song);
    void UpdateDownloadTracking(const ezui::UIString& hash, float progress);
    void FinishDownloadTracking(const ezui::UIString& hash);
    std::vector<kugou::SongSummary> BuildDownloadLibrarySongs() const;
    void RefreshVisibleDownloadProgress();

    std::vector<kugou::SongSummary> LoadSongsFromIni(IniConfig* ini) const;
    void SaveSongToIni(IniConfig* ini, const kugou::SongSummary& song) const;
    void DeleteSongFromIni(IniConfig* ini, const ezui::UIString& hash) const;
    void ClearIniSections(IniConfig* ini) const;

    ezui::Control* windowRoot_ = nullptr;
    ezui::TabControl* rootTabs_ = nullptr;
    ezui::TabControl* contentTabs_ = nullptr;
    ezui::Frame* titleFrame_ = nullptr;
    ezui::Frame* homeFrame_ = nullptr;
    ezui::Frame* searchFrame_ = nullptr;
    ezui::Frame* detailFrame_ = nullptr;
    ezui::Frame* libraryFrame_ = nullptr;
    ezui::Frame* playerFrame_ = nullptr;

    ezui::HBox* navLike_ = nullptr;
    ezui::HBox* navRecent_ = nullptr;
    ezui::HBox* navDownload_ = nullptr;
    ezui::HBox* navDaily_ = nullptr;
    ezui::Label* navLikeText_ = nullptr;
    ezui::Label* navRecentText_ = nullptr;
    ezui::Label* navDownloadText_ = nullptr;
    ezui::Label* sidebarStatus_ = nullptr;

    ezui::TextBox* searchBox_ = nullptr;
    ezui::Label* titleBackBtn_ = nullptr;
    ezui::Label* titleHomeBtn_ = nullptr;

    ezui::Label* homeLikeCount_ = nullptr;
    ezui::Label* homeRecentCount_ = nullptr;
    ezui::Label* homeDownloadCount_ = nullptr;
    ezui::Label* homeCurrentMeta_ = nullptr;
    ezui::Label* homeHeroTitle_ = nullptr;
    ezui::Label* homeHeroDesc_ = nullptr;
    ezui::Label* homeHeroCover_ = nullptr;
    ezui::Control* homeHeroCard_ = nullptr;
    ezui::Control* homeCardSearch_ = nullptr;
    ezui::Control* homeCardLike_ = nullptr;
    ezui::Control* homeCardDaily_ = nullptr;
    ezui::Control* homeCardRecent_ = nullptr;
    ezui::Control* homeCardDownload_ = nullptr;
    ezui::Control* homeCardLyrics_ = nullptr;
    ezui::Control* homeCardSearchMore_ = nullptr;

    ezui::Label* searchTitle_ = nullptr;
    ezui::Label* searchMeta_ = nullptr;
    ezui::Label* searchResultCount_ = nullptr;
    ezui::Label* searchPlayBtn_ = nullptr;
    ezui::Label* searchDownloadBtn_ = nullptr;
    ezui::Label* searchBatchBtn_ = nullptr;
    ezui::VListView* searchSongList_ = nullptr;

    ezui::Label* libraryTitle_ = nullptr;
    ezui::Label* libraryMeta_ = nullptr;
    ezui::Label* libraryPlayBtn_ = nullptr;
    ezui::Label* libraryDownloadBtn_ = nullptr;
    ezui::Label* libraryClearBtn_ = nullptr;
    ezui::VListView* librarySongList_ = nullptr;

    ezui::Control* detailRoot_ = nullptr;
    ezui::Control* detailBackBtn_ = nullptr;
    RecordCover* detailCover_ = nullptr;
    ezui::Label* detailSingerAvatar_ = nullptr;
    ezui::Label* detailTitle_ = nullptr;
    ezui::Label* detailSinger_ = nullptr;
    ezui::Label* detailMeta_ = nullptr;
    ezui::Label* detailLikeBtn_ = nullptr;
    ezui::Label* detailDownloadBtn_ = nullptr;
    ezui::Label* detailStatus_ = nullptr;
    ezui::Label* detailPrevBtn_ = nullptr;
    ezui::Label* detailPlayPauseBtn_ = nullptr;
    ezui::Label* detailNextBtn_ = nullptr;
    ezui::Label* detailElapsedLabel_ = nullptr;
    ezui::Label* detailTotalLabel_ = nullptr;
    MusicSeekBar* detailProgressSlider_ = nullptr;
    LrcPanel* detailLrc_ = nullptr;

    ezui::Label* dockCover_ = nullptr;
    ezui::Label* dockSingerAvatar_ = nullptr;
    ezui::Label* dockTitle_ = nullptr;
    ezui::Label* dockSinger_ = nullptr;
    ezui::Label* dockLikeBtn_ = nullptr;
    ezui::Label* dockDownloadBtn_ = nullptr;
    ezui::Label* dockDetailBtn_ = nullptr;
    ezui::Label* prevBtn_ = nullptr;
    ezui::Label* playPauseBtn_ = nullptr;
    ezui::Label* nextBtn_ = nullptr;
    ezui::Label* elapsedLabel_ = nullptr;
    ezui::Label* totalLabel_ = nullptr;
    MusicSeekBar* progressSlider_ = nullptr;

    std::unique_ptr<VlcPlayer> player_;
    std::unique_ptr<ezui::Timer> progressTimer_;
    std::unique_ptr<ezui::Timer> artworkRotationTimer_;
    std::unique_ptr<ezui::DebounceTimer> searchDebounce_;
    std::unique_ptr<ezui::Animation> pageRevealAnimation_;
    std::unique_ptr<IniConfig> favoritesIni_;
    std::unique_ptr<IniConfig> recentIni_;
    std::unique_ptr<IniConfig> downloadsIni_;
    std::unique_ptr<IniConfig> daily30Ini_;
    std::unique_ptr<IniConfig> daily30MetaIni_;

    std::vector<kugou::SongSummary> searchResults_;
    std::vector<kugou::SongSummary> favorites_;
    std::vector<kugou::SongSummary> recentPlays_;
    std::vector<kugou::SongSummary> downloadedSongs_;
    std::vector<kugou::SongSummary> dailySongs_;
    std::vector<kugou::SongSummary> recommendedSongs_;
    std::vector<kugou::SongSummary> libraryViewSongs_;
    std::vector<kugou::SongSummary> playQueue_;
    std::vector<DownloadTaskState> activeDownloads_;

    kugou::SongPlaybackInfo currentPlaybackInfo_;

    ezui::UIString currentQuery_;
    ezui::UIString currentSongHash_;
    ezui::UIString selectedSearchHash_;
    ezui::UIString selectedLibraryHash_;

    int currentSongIndex_ = -1;
    int currentSearchPage_ = 0;
    int searchToken_ = 0;
    int playToken_ = 0;
    bool hasMoreResults_ = false;
    bool isLoadingResults_ = false;
    bool isRefreshingDaily30_ = false;
    bool isRefreshingRecommendations_ = false;
    bool pendingAutoPlayFirstResult_ = false;
    float artworkRotationAngle_ = 0.0f;
    std::atomic<bool> isClosing_ = false;

    ContentPage currentPage_ = ContentPage::Home;
    ContentPage previousPage_ = ContentPage::Home;
    LibraryView currentLibraryView_ = LibraryView::Likes;
};
