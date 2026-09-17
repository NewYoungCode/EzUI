#include "MainForm.h"

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <cwchar>
#include <ctime>
#include <cstring>
#include <unordered_set>

#include "LrcPanel.h"
#include "MusicSeekBar.h"
#include "RecordCover.h"
#include "SongRowItem.h"
#include "VlcPlayer.h"

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

using namespace ezui;

namespace {

	long long CurrentUnixTimestamp()
	{
		return static_cast<long long>(std::time(nullptr));
	}

	UIString FirstSingerName(const UIString& singerName)
	{
		UIString primary = singerName.trim();
		const UIString separators[] = { " / ", "/", "、", ",", "，" };
		for (const auto& separator : separators) {
			const auto pos = primary.find(separator);
			if (pos != UIString::npos) {
				primary = primary.substr(0, pos);
				break;
			}
		}
		return primary.trim();
	}

	UIString SanitizeFileName(const UIString& text)
	{
		auto value = text.unicode();
		for (auto& ch : value) {
			if (wcschr(L"\\/:*?\"<>|", ch)) {
				ch = L'_';
			}
		}
		return UIString(value).trim();
	}

	UIString GuessDownloadExtension(const UIString& url, const UIString& fallback)
	{
		UIString cleanUrl = url;
		const auto queryPos = cleanUrl.find("?");
		if (queryPos != UIString::npos) {
			cleanUrl = cleanUrl.substr(0, queryPos);
		}

		const auto dotPos = cleanUrl.find_last_of('.');
		if (dotPos != UIString::npos) {
			const auto extension = cleanUrl.substr(dotPos);
			if (!extension.empty() && extension.size() <= 5) {
				return extension;
			}
		}
		return fallback;
	}

	UIString ToUiString(long long value)
	{
		return UIString(std::to_string(value));
	}

	UIString CurrentDayKey()
	{
		std::time_t now = std::time(nullptr);
		std::tm localTime {};
		localtime_s(&localTime, &now);

		wchar_t buffer[16] = {};
		std::wcsftime(buffer, sizeof(buffer) / sizeof(buffer[0]), L"%Y-%m-%d", &localTime);
		return UIString(buffer);
	}

	int ClampInt(int value, int minimum, int maximum)
	{
		return (std::max)(minimum, (std::min)(value, maximum));
	}

	Image* CloneImage(Image* image)
	{
		return image ? dynamic_cast<Image*>(image->Clone()) : nullptr;
	}

	bool CopyImagePixels(Image* image, std::vector<uint8_t>& pixels, int& width, int& height)
	{
		if (!image) {
			return false;
		}

		width = image->Width();
		height = image->Height();
		if (width <= 0 || height <= 0) {
			return false;
		}

		const uint32_t stride = static_cast<uint32_t>(width * 4);
		pixels.assign(static_cast<size_t>(stride) * height, 0);
		return image->LockPixels([&](uint8_t* sourcePixels, uint32_t sourceStride) {
			for (int y = 0; y < height; ++y) {
				std::memcpy(
					pixels.data() + static_cast<size_t>(y) * stride,
					sourcePixels + static_cast<size_t>(y) * sourceStride,
					stride);
			}
			});
	}

	void BoxBlurPass(const std::vector<uint8_t>& source,
		std::vector<uint8_t>& target,
		int width,
		int height,
		int radius,
		bool horizontal)
	{
		if (radius <= 0) {
			target = source;
			return;
		}

		const int stride = width * 4;
		const int sampleCount = radius * 2 + 1;

		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				uint32_t b = 0;
				uint32_t g = 0;
				uint32_t r = 0;
				uint32_t a = 0;

				for (int offset = -radius; offset <= radius; ++offset) {
					const int sampleX = horizontal ? ClampInt(x + offset, 0, width - 1) : x;
					const int sampleY = horizontal ? y : ClampInt(y + offset, 0, height - 1);
					const size_t index = static_cast<size_t>(sampleY) * stride + sampleX * 4;
					b += source[index + 0];
					g += source[index + 1];
					r += source[index + 2];
					a += source[index + 3];
				}

				const size_t outputIndex = static_cast<size_t>(y) * stride + x * 4;
				target[outputIndex + 0] = static_cast<uint8_t>(b / sampleCount);
				target[outputIndex + 1] = static_cast<uint8_t>(g / sampleCount);
				target[outputIndex + 2] = static_cast<uint8_t>(r / sampleCount);
				target[outputIndex + 3] = static_cast<uint8_t>(a / sampleCount);
			}
		}
	}

	Image* CreateBlurredBackgroundImage(Image* image)
	{
		std::vector<uint8_t> sourcePixels;
		int sourceWidth = 0;
		int sourceHeight = 0;
		if (!CopyImagePixels(image, sourcePixels, sourceWidth, sourceHeight)) {
			return nullptr;
		}

		const int maxDimension = 320;
		const float scale = static_cast<float>(maxDimension) / static_cast<float>((std::max)(sourceWidth, sourceHeight));
		const float clampedScale = (std::min)(1.0f, scale);
		const int targetWidth = (std::max)(1, static_cast<int>(sourceWidth * clampedScale));
		const int targetHeight = (std::max)(1, static_cast<int>(sourceHeight * clampedScale));
		const int sourceStride = sourceWidth * 4;
		const int targetStride = targetWidth * 4;

		std::vector<uint8_t> reduced(static_cast<size_t>(targetStride) * targetHeight, 0);
		for (int y = 0; y < targetHeight; ++y) {
			const int sampleY = (std::min)(sourceHeight - 1, static_cast<int>((static_cast<long long>(y) * sourceHeight) / targetHeight));
			for (int x = 0; x < targetWidth; ++x) {
				const int sampleX = (std::min)(sourceWidth - 1, static_cast<int>((static_cast<long long>(x) * sourceWidth) / targetWidth));
				const size_t sourceIndex = static_cast<size_t>(sampleY) * sourceStride + sampleX * 4;
				const size_t targetIndex = static_cast<size_t>(y) * targetStride + x * 4;
				reduced[targetIndex + 0] = sourcePixels[sourceIndex + 0];
				reduced[targetIndex + 1] = sourcePixels[sourceIndex + 1];
				reduced[targetIndex + 2] = sourcePixels[sourceIndex + 2];
				reduced[targetIndex + 3] = sourcePixels[sourceIndex + 3];
			}
		}

		std::vector<uint8_t> blurred = reduced;
		std::vector<uint8_t> scratch(blurred.size(), 0);
		const int blurRadius = 12;
		for (int i = 0; i < 2; ++i) {
			BoxBlurPass(blurred, scratch, targetWidth, targetHeight, blurRadius, true);
			BoxBlurPass(scratch, blurred, targetWidth, targetHeight, blurRadius, false);
		}

		for (size_t i = 0; i + 3 < blurred.size(); i += 4) {
			blurred[i + 0] = static_cast<uint8_t>(blurred[i + 0] * 58 / 100);
			blurred[i + 1] = static_cast<uint8_t>(blurred[i + 1] * 58 / 100);
			blurred[i + 2] = static_cast<uint8_t>(blurred[i + 2] * 58 / 100);
			blurred[i + 3] = static_cast<uint8_t>((std::max)(static_cast<int>(blurred[i + 3]), 220));
		}

		auto* blurredImage = new Image(targetWidth, targetHeight);
		if (!blurredImage->LockPixels([&](uint8_t* targetPixels, uint32_t targetBitmapStride) {
			for (int y = 0; y < targetHeight; ++y) {
				std::memcpy(
					targetPixels + static_cast<size_t>(y) * targetBitmapStride,
					blurred.data() + static_cast<size_t>(y) * targetStride,
					targetStride);
			}
			})) {
			delete blurredImage;
			return nullptr;
		}

		blurredImage->SizeMode = ImageSizeMode::Cover;
		return blurredImage;
	}

	void ApplyNavStyle(HBox* row, bool active, bool enabled = true)
	{
		if (!row) {
			return;
		}

		row->Style->BackColor = active ? Color(29, 51, 42) : Color::Transparent;
		row->HoverStyle->BackColor = enabled ? Color(33, 33, 33) : Color::Transparent;

		for (auto* child : row->GetChildren()) {
			if (auto* label = dynamic_cast<Label*>(child)) {
				if (!enabled) {
					label->Style->ForeColor = Color(120, 120, 120);
				}
				else if (active) {
					label->Style->ForeColor = Color(25, 223, 127);
				}
				else {
					label->Style->ForeColor = Color(229, 229, 229);
				}
				label->Invalidate();
			}
		}

		row->Invalidate();
	}

	kugou::SongSummary MergeSongSummary(const kugou::SongSummary& song, const kugou::SongPlaybackInfo& info)
	{
		auto merged = song;
		if (merged.songName.empty()) {
			merged.songName = info.songName.empty() ? info.displayName : info.songName;
		}
		if (merged.singerName.empty()) {
			merged.singerName = info.singerName;
		}
		if (merged.durationSeconds <= 0) {
			merged.durationSeconds = info.durationSeconds;
		}
		return merged;
	}

	ezui::ThreadPool& QQMusicTaskPool()
	{
		static ezui::ThreadPool pool(4);
		return pool;
	}

} // namespace

bool TryEnableSystemRoundCorner(HWND hwnd)
{
	typedef HRESULT(WINAPI* PFN_DwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);

	static PFN_DwmSetWindowAttribute pfn = []() -> PFN_DwmSetWindowAttribute {
		HMODULE hDwm = ::LoadLibraryW(L"dwmapi.dll");
		return hDwm ? (PFN_DwmSetWindowAttribute)::GetProcAddress(hDwm, "DwmSetWindowAttribute") : nullptr;
		}();

	if (!pfn) return false;

	constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
	constexpr DWORD DWMWCP_ROUND = 2;
	return SUCCEEDED(pfn(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &DWMWCP_ROUND, sizeof(DWMWCP_ROUND)));
}

MainForm::MainForm() : BorderlessWindow()
{

	SetTitle(L"QQ音乐");
	SetResizable(true);
	LoadXml("res/mainForm.htm");
	SetMinSize(Size(1049, 690));
	SetSize(1049, 690);

	player_ = std::make_unique<VlcPlayer>();
	player_->SetVolume(72);

	progressTimer_ = std::make_unique<Timer>();
	artworkRotationTimer_ = std::make_unique<Timer>();
	searchDebounce_ = std::make_unique<DebounceTimer>();

	InitControls();
	InitEvents();
	LoadLibraryData();

	ShowPage(ContentPage::Home, false);
	UpdatePlayButtonState();
	UpdateSidebarCounts();
	UpdateHomeSummary();
	PopulateLibraryList();
	SetSearchMeta(L"在顶部输入关键词开始搜索。");
	SetLibraryMeta(L"这里会展示持久化保存的歌曲列表。");
	UpdateDetailStatus(L"播放歌曲后这里会显示当前状态。");
	SetSidebarStatus(L"主页推荐已就绪");

	if (TryEnableSystemRoundCorner(GetWindowHandle())) {
		CloseShadow();
	}
	else {
		EnableAlphaBlending();
		this->GetLayout()->Style->Border.Radius = 10;
	}
}

MainForm::~MainForm()
{
	isClosing_.store(true);
	if (progressTimer_) {
		progressTimer_->Stop();
	}
	if (artworkRotationTimer_) {
		artworkRotationTimer_->Stop();
	}
	if (searchDebounce_) {
		searchDebounce_->Cancel();
	}
	if (player_) {
		player_->PlayingCallback = nullptr;
		player_->Stop();
	}
}

void MainForm::OnClose(bool& allowClose)
{
	allowClose = false;
	if (isClosing_.exchange(true)) {
		return;
	}
	++searchToken_;
	++playToken_;

	if (progressTimer_) {
		progressTimer_->Stop();
	}
	if (artworkRotationTimer_) {
		artworkRotationTimer_->Stop();
	}
	if (searchDebounce_) {
		searchDebounce_->Cancel();
	}
	if (player_) {
		player_->PlayingCallback = nullptr;
		player_->Stop();
	}
	if (!BeginInvoke([] {
		Application::Exit(0);
		})) {
		Application::Exit(0);
	}
}

bool MainForm::IsClosingRequested() const noexcept
{
	return isClosing_.load();
}

void MainForm::OnShow()
{
	__super::OnShow();
	RefreshHomeRecommendations();
}

void MainForm::InitControls()
{
	windowRoot_ = FindControl("windowRoot");
	if (windowRoot_) {
		windowRoot_->Style->Opacity = 1.0f;
	}
	rootTabs_ = FindControl<TabControl>("rootTabs");
	contentTabs_ = FindControl<TabControl>("contentTabs");
	titleFrame_ = FindControl<Frame>("titleFrame");
	homeFrame_ = FindControl<Frame>("homeFrame");
	searchFrame_ = FindControl<Frame>("searchFrame");
	detailFrame_ = FindControl<Frame>("detailFrame");
	libraryFrame_ = FindControl<Frame>("libraryFrame");
	playerFrame_ = FindControl<Frame>("playerFrame");

	navLike_ = FindControl<HBox>("navLike");
	navRecent_ = FindControl<HBox>("navRecent");
	navDownload_ = FindControl<HBox>("navDownload");
	navDaily_ = FindControl<HBox>("navDaily");
	navLikeText_ = FindControl<Label>("navLikeText");
	navRecentText_ = FindControl<Label>("navRecentText");
	navDownloadText_ = FindControl<Label>("navDownloadText");
	sidebarStatus_ = FindControl<Label>("sidebarStatus");

	if (titleFrame_) {
		searchBox_ = titleFrame_->FindControl<TextBox>("searchBox");
		titleBackBtn_ = titleFrame_->FindControl<Label>("titleBackBtn");
		titleHomeBtn_ = titleFrame_->FindControl<Label>("titleHomeBtn");
	}

	if (homeFrame_) {
		homeLikeCount_ = homeFrame_->FindControl<Label>("homeLikeCount");
		homeRecentCount_ = homeFrame_->FindControl<Label>("homeRecentCount");
		homeDownloadCount_ = homeFrame_->FindControl<Label>("homeDownloadCount");
		homeCurrentMeta_ = homeFrame_->FindControl<Label>("homeCurrentMeta");
		homeHeroTitle_ = homeFrame_->FindControl<Label>("homeHeroTitle");
		homeHeroDesc_ = homeFrame_->FindControl<Label>("homeHeroDesc");
		homeHeroCover_ = homeFrame_->FindControl<Label>("homeHeroCover");
		homeHeroCard_ = homeFrame_->FindControl("homeHeroCard");
		homeCardSearch_ = homeFrame_->FindControl("homeCardSearch");
		homeCardLike_ = homeFrame_->FindControl("homeCardLike");
		homeCardDaily_ = homeFrame_->FindControl("homeCardDaily");
		homeCardRecent_ = homeFrame_->FindControl("homeCardRecent");
		homeCardDownload_ = homeFrame_->FindControl("homeCardDownload");
		homeCardLyrics_ = homeFrame_->FindControl("homeCardLyrics");
		homeCardSearchMore_ = homeFrame_->FindControl("homeCardSearchMore");
	}

	if (searchFrame_) {
		searchTitle_ = searchFrame_->FindControl<Label>("searchTitle");
		searchMeta_ = searchFrame_->FindControl<Label>("searchMeta");
		searchResultCount_ = searchFrame_->FindControl<Label>("searchResultCount");
		searchPlayBtn_ = searchFrame_->FindControl<Label>("searchPlayBtn");
		searchDownloadBtn_ = searchFrame_->FindControl<Label>("searchDownloadBtn");
		searchBatchBtn_ = searchFrame_->FindControl<Label>("searchBatchBtn");
		searchSongList_ = searchFrame_->FindControl<VListView>("searchSongList");
	}

	if (libraryFrame_) {
		libraryTitle_ = libraryFrame_->FindControl<Label>("libraryTitle");
		libraryMeta_ = libraryFrame_->FindControl<Label>("libraryMeta");
		libraryPlayBtn_ = libraryFrame_->FindControl<Label>("libraryPlayBtn");
		libraryDownloadBtn_ = libraryFrame_->FindControl<Label>("libraryDownloadBtn");
		libraryClearBtn_ = libraryFrame_->FindControl<Label>("libraryClearBtn");
		librarySongList_ = libraryFrame_->FindControl<VListView>("librarySongList");
	}

	if (detailFrame_) {
		detailRoot_ = detailFrame_->FindControl("detailRoot");
		detailBackBtn_ = detailFrame_->FindControl("detailBackBtn");
		detailCover_ = detailFrame_->FindControl<RecordCover>("detailCover");
		detailSingerAvatar_ = detailFrame_->FindControl<Label>("detailSingerAvatar");
		detailTitle_ = detailFrame_->FindControl<Label>("detailTitle");
		detailSinger_ = detailFrame_->FindControl<Label>("detailSinger");
		detailMeta_ = detailFrame_->FindControl<Label>("detailMeta");
		detailLikeBtn_ = detailFrame_->FindControl<Label>("detailLikeBtn");
		detailDownloadBtn_ = detailFrame_->FindControl<Label>("detailDownloadBtn");
		detailStatus_ = detailFrame_->FindControl<Label>("detailStatus");
		detailPrevBtn_ = detailFrame_->FindControl<Label>("detailPrevBtn");
		detailPlayPauseBtn_ = detailFrame_->FindControl<Label>("detailPlayPauseBtn");
		detailNextBtn_ = detailFrame_->FindControl<Label>("detailNextBtn");
		detailElapsedLabel_ = detailFrame_->FindControl<Label>("detailElapsedLabel");
		detailTotalLabel_ = detailFrame_->FindControl<Label>("detailTotalLabel");
		detailProgressSlider_ = detailFrame_->FindControl<MusicSeekBar>("detailProgressSlider");
		detailLrc_ = detailFrame_->FindControl<LrcPanel>("detailLrc");
	}

	if (playerFrame_) {
		dockCover_ = playerFrame_->FindControl<Label>("dockCover");
		dockSingerAvatar_ = playerFrame_->FindControl<Label>("dockSingerAvatar");
		dockTitle_ = playerFrame_->FindControl<Label>("dockTitle");
		dockSinger_ = playerFrame_->FindControl<Label>("dockSinger");
		dockLikeBtn_ = playerFrame_->FindControl<Label>("dockLikeBtn");
		dockDownloadBtn_ = playerFrame_->FindControl<Label>("dockDownloadBtn");
		dockDetailBtn_ = playerFrame_->FindControl<Label>("dockDetailBtn");
		prevBtn_ = playerFrame_->FindControl<Label>("prevBtn");
		playPauseBtn_ = playerFrame_->FindControl<Label>("playPauseBtn");
		nextBtn_ = playerFrame_->FindControl<Label>("nextBtn");
		elapsedLabel_ = playerFrame_->FindControl<Label>("elapsedLabel");
		totalLabel_ = playerFrame_->FindControl<Label>("totalLabel");
		progressSlider_ = playerFrame_->FindControl<MusicSeekBar>("progressSlider");
	}

	if (searchSongList_ && searchSongList_->GetScrollBar()) {
		searchSongList_->GetScrollBar()->SetAutoHide(600);
	}
	if (librarySongList_ && librarySongList_->GetScrollBar()) {
		librarySongList_->GetScrollBar()->SetAutoHide(600);
	}

	if (progressTimer_) {
		auto weakThis = GetWeakPtr<MainForm>();
		progressTimer_->SetInterval(200);
		progressTimer_->SetTickHandler([weakThis](Timer*) {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}
			BeginInvoke([weakThis] {
				if (weakThis.IsAlive() && !weakThis->IsClosingRequested()) {
					weakThis->UpdateProgress();
				}
				});
			});
	}
	if (artworkRotationTimer_) {
		auto weakThis = GetWeakPtr<MainForm>();
		artworkRotationTimer_->SetInterval(33);
		artworkRotationTimer_->SetTickHandler([weakThis](Timer*) {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}
			BeginInvoke([weakThis] {
				if (!weakThis.IsAlive() || weakThis->IsClosingRequested() || !weakThis->detailCover_ || !weakThis->player_) {
					return;
				}

				const auto state = weakThis->player_->GetState();
				const bool isPlaybackActive = (state == libvlc_Playing || state == libvlc_Buffering || state == libvlc_Opening);
				if (!isPlaybackActive) {
					return;
				}

				weakThis->artworkRotationAngle_ += 0.45f;
				if (weakThis->artworkRotationAngle_ >= 360.0f) {
					weakThis->artworkRotationAngle_ -= 360.0f;
				}

				weakThis->detailCover_->Style->Angle = weakThis->artworkRotationAngle_;
				weakThis->detailCover_->Invalidate();
				});
			});
	}
}

void MainForm::InitEvents()
{
	auto bindClick = [](Control* control, const std::function<void()>& callback) {
		if (!control) {
			return;
		}
		control->AddEventHandler(Event::MouseDown, [callback](Control*, EventArgs* args) {
			auto* mouseArgs = args->As<MouseEventArgs>();
			if (mouseArgs && mouseArgs->Button() == MouseButton::Left) {
				callback();
			}
			});
		};

	if (searchBox_) {
		searchBox_->TextChanged = [this](TextBox*, const UIString& text) {
			const auto query = text.trim();
			if (query.empty()) {
				if (searchDebounce_) {
					searchDebounce_->Cancel();
				}
				return;
			}

			searchDebounce_->Trigger(420, [this, query]() {
				StartSearch(query);
				});
			};

		searchBox_->AddEventHandler(Event::KeyDown, [this](Control*, EventArgs* args) {
			auto* keyArgs = args->As<KeyEventArgs>();
			if (keyArgs && keyArgs->KeyCode() == VK_RETURN) {
				StartSearch(searchBox_->GetText());
			}
			});
	}

	bindClick(titleBackBtn_, [this]() { NavigateBack(); });
	bindClick(titleHomeBtn_, [this]() { ShowPage(ContentPage::Home); });

	bindClick(navLike_, [this]() { OpenLibrary(LibraryView::Likes); });
	bindClick(navRecent_, [this]() { OpenLibrary(LibraryView::Recent); });
	bindClick(navDownload_, [this]() { OpenLibrary(LibraryView::Download); });
	bindClick(navDaily_, [this]() { OpenLibrary(LibraryView::Daily30); });

	bindClick(homeHeroCard_, [this]() { OpenRecommendedList(); });
	bindClick(homeCardSearch_, [this]() {
		if (searchBox_) {
			searchBox_->SetText(L"post to be");
		}
		if (searchDebounce_) {
			searchDebounce_->Cancel();
		}
		StartSearch(L"post to be");
		});
	bindClick(homeCardLike_, [this]() { OpenLibrary(LibraryView::Likes); });
	bindClick(homeCardDaily_, [this]() { OpenLibrary(LibraryView::Daily30); });
	bindClick(homeCardRecent_, [this]() { OpenLibrary(LibraryView::Recent); });
	bindClick(homeCardDownload_, [this]() { OpenLibrary(LibraryView::Download); });
	bindClick(homeCardLyrics_, [this]() {
		if (!currentSongHash_.empty()) {
			ShowPage(ContentPage::Detail);
		}
		else {
			if (searchBox_) {
				searchBox_->SetText(L"post to be");
			}
			if (searchDebounce_) {
				searchDebounce_->Cancel();
			}
			StartSearch(L"post to be", true);
		}
		});
	bindClick(homeCardSearchMore_, [this]() {
		if (searchBox_) {
			searchBox_->SetText(L"周杰伦");
		}
		if (searchDebounce_) {
			searchDebounce_->Cancel();
		}
		StartSearch(L"周杰伦");
		});

	bindClick(searchPlayBtn_, [this]() {
		const auto* song = GetSelectedSearchSong();
		if (song) {
			PlaySongFromSearch(FindSongIndexByHash(searchResults_, song->hash));
		}
		else if (!searchResults_.empty()) {
			PlaySongFromSearch(0);
		}
		});
	bindClick(searchDownloadBtn_, [this]() {
		const auto* song = GetSelectedSearchSong();
		if (song) {
			DownloadSong(*song);
		}
		else if (!searchResults_.empty()) {
			DownloadSong(searchResults_.front());
		}
		});
	bindClick(searchBatchBtn_, [this]() {
		DownloadSongs(searchResults_, 3);
		});

	bindClick(libraryPlayBtn_, [this]() {
		const auto* song = GetSelectedLibrarySong();
		if (song) {
			PlaySongFromLibrary(FindSongIndexByHash(libraryViewSongs_, song->hash));
		}
		else if (!libraryViewSongs_.empty()) {
			PlaySongFromLibrary(0);
		}
		});
	bindClick(libraryDownloadBtn_, [this]() {
		const auto* song = GetSelectedLibrarySong();
		if (song) {
			DownloadSong(*song);
		}
		else if (!libraryViewSongs_.empty()) {
			DownloadSong(libraryViewSongs_.front());
		}
		});
	bindClick(libraryClearBtn_, [this]() { ClearCurrentLibrary(); });

	bindClick(detailBackBtn_, [this]() { NavigateBack(); });
	bindClick(detailSinger_, [this]() {
		if (!currentSongHash_.empty() && detailSinger_) {
			SearchSinger(detailSinger_->GetText());
		}
		});
	bindClick(detailLikeBtn_, [this]() {
		if (!currentSongHash_.empty() && currentSongIndex_ >= 0 && currentSongIndex_ < static_cast<int>(playQueue_.size())) {
			ToggleFavorite(playQueue_[currentSongIndex_]);
		}
		});
	bindClick(detailDownloadBtn_, [this]() {
		if (!currentSongHash_.empty() && currentSongIndex_ >= 0 && currentSongIndex_ < static_cast<int>(playQueue_.size())) {
			DownloadSong(playQueue_[currentSongIndex_]);
		}
		});
	bindClick(detailPrevBtn_, [this]() { PlayPreviousSong(); });
	bindClick(detailPlayPauseBtn_, [this]() { TogglePlayPause(); });
	bindClick(detailNextBtn_, [this]() { PlayNextSong(); });

	bindClick(prevBtn_, [this]() { PlayPreviousSong(); });
	bindClick(playPauseBtn_, [this]() { TogglePlayPause(); });
	bindClick(nextBtn_, [this]() { PlayNextSong(); });
	bindClick(dockLikeBtn_, [this]() {
		if (!currentSongHash_.empty() && currentSongIndex_ >= 0 && currentSongIndex_ < static_cast<int>(playQueue_.size())) {
			ToggleFavorite(playQueue_[currentSongIndex_]);
		}
		});
	bindClick(dockDownloadBtn_, [this]() {
		if (!currentSongHash_.empty() && currentSongIndex_ >= 0 && currentSongIndex_ < static_cast<int>(playQueue_.size())) {
			DownloadSong(playQueue_[currentSongIndex_]);
		}
		});
	bindClick(dockDetailBtn_, [this]() {
		if (!currentSongHash_.empty()) {
			ShowPage(ContentPage::Detail);
		}
		});
	bindClick(dockCover_, [this]() {
		if (!currentSongHash_.empty()) {
			ShowPage(ContentPage::Detail);
		}
		});
	bindClick(dockSinger_, [this]() {
		if (!currentSongHash_.empty() && dockSinger_) {
			SearchSinger(dockSinger_->GetText());
		}
		});
	bindClick(dockSingerAvatar_, [this]() {
		if (!currentSongHash_.empty()) {
			ShowPage(ContentPage::Detail);
		}
		});

	if (searchSongList_ && searchSongList_->GetScrollBar()) {
		searchSongList_->GetScrollBar()->ValueChanged = [this](ScrollBar* scrollBar, int value) {
			if (currentQuery_.empty() || !hasMoreResults_ || isLoadingResults_) {
				return;
			}

			if (value >= scrollBar->GetMaxValue() - 2) {
				RequestSearchPage(currentSearchPage_ + 1, true);
			}
			};
	}

	if (progressSlider_) {
		progressSlider_->AddEventHandler(Event::MouseUp, [this](Control*, EventArgs*) {
			SeekFromSlider(progressSlider_);
			});
	}
	if (detailProgressSlider_) {
		detailProgressSlider_->AddEventHandler(Event::MouseUp, [this](Control*, EventArgs*) {
			SeekFromSlider(detailProgressSlider_);
			});
	}
}

void MainForm::AnimateControlOpacity(std::unique_ptr<Animation>& animation,
	const std::vector<Control*>& controls,
	float from,
	float to,
	int durationMs,
	EasingCurve easing)
{
	if (controls.empty()) {
		return;
	}

	if (!animation) {
		animation = std::make_unique<Animation>();
	}

	animation->Stop();
	animation->SetEasingCurve(easing);
	animation->ValueChanged = [controls](float value) {
		for (auto* control : controls) {
			if (!control) {
				continue;
			}
			control->Style->Opacity = value;
			control->Invalidate();
		}
		};
	animation->SetStartValue(from);
	animation->SetEndValue(to);
	animation->Start(durationMs);
}

void MainForm::AnimatePageReveal(Control* control, float from, int durationMs)
{
	if (!control) {
		return;
	}
	AnimateControlOpacity(pageRevealAnimation_, { control }, from, 1.0f, durationMs, EasingCurve::Damping);
}

void MainForm::LoadLibraryData()
{
	favoritesIni_ = std::make_unique<IniConfig>(Path::StartPath() + "\\favorites.ini");
	recentIni_ = std::make_unique<IniConfig>(Path::StartPath() + "\\recent.ini");
	downloadsIni_ = std::make_unique<IniConfig>(Path::StartPath() + "\\downloads.ini");
	daily30Ini_ = std::make_unique<IniConfig>(Path::StartPath() + "\\daily30.ini");
	daily30MetaIni_ = std::make_unique<IniConfig>(Path::StartPath() + "\\daily30_meta.ini");

	favorites_ = LoadSongsFromIni(favoritesIni_.get());
	recentPlays_ = LoadSongsFromIni(recentIni_.get());
	downloadedSongs_ = LoadSongsFromIni(downloadsIni_.get());
	dailySongs_ = LoadSongsFromIni(daily30Ini_.get());
}

std::vector<kugou::SongSummary> MainForm::LoadSongsFromIni(IniConfig* ini) const
{
	std::vector<kugou::SongSummary> songs;
	if (!ini) {
		return songs;
	}

	for (const auto& section : ini->GetSections()) {
		kugou::SongSummary song;
		song.hash = section;
		song.songName = ini->ReadString(section, "name");
		song.singerName = ini->ReadString(section, "singer");
		song.albumId = ini->ReadString(section, "album_id");
		song.albumName = ini->ReadString(section, "album_name");
		song.mvHash = ini->ReadString(section, "mv_hash");
		song.localPath = ini->ReadString(section, "local_path");
		song.durationSeconds = ini->ReadInt(section, "duration");
		song.qualityLevel = ini->ReadInt(section, "quality");

		const auto updatedAt = ini->ReadString(section, "updated_at");
		if (!updatedAt.empty()) {
			song.updatedAt = _wtoll(updatedAt.unicode().c_str());
		}

		if (song.hash.empty()) {
			continue;
		}
		songs.push_back(song);
	}

	std::sort(songs.begin(), songs.end(), [](const auto& left, const auto& right) {
		return left.updatedAt > right.updatedAt;
		});

	return songs;
}

void MainForm::SaveSongToIni(IniConfig* ini, const kugou::SongSummary& song) const
{
	if (!ini || song.hash.empty()) {
		return;
	}

	ini->WriteString(song.hash, "name", song.songName);
	ini->WriteString(song.hash, "singer", song.singerName);
	ini->WriteString(song.hash, "album_id", song.albumId);
	ini->WriteString(song.hash, "album_name", song.albumName);
	ini->WriteString(song.hash, "mv_hash", song.mvHash);
	ini->WriteString(song.hash, "local_path", song.localPath);
	ini->WriteInt(song.hash, "duration", song.durationSeconds);
	ini->WriteInt(song.hash, "quality", song.qualityLevel);
	ini->WriteString(song.hash, "updated_at", ToUiString(song.updatedAt));
}

void MainForm::DeleteSongFromIni(IniConfig* ini, const UIString& hash) const
{
	if (ini && !hash.empty()) {
		ini->DeleteSection(hash);
	}
}

void MainForm::ClearIniSections(IniConfig* ini) const
{
	if (!ini) {
		return;
	}

	for (const auto& section : ini->GetSections()) {
		ini->DeleteSection(section);
	}
}

void MainForm::ShowPage(ContentPage page, bool rememberPrevious)
{
	if (!rootTabs_ || !contentTabs_) {
		return;
	}

	const auto oldPage = currentPage_;
	if (rememberPrevious && page == ContentPage::Detail && currentPage_ != ContentPage::Detail) {
		previousPage_ = currentPage_;
	}

	currentPage_ = page;

	if (page == ContentPage::Detail) {
		if (oldPage != ContentPage::Detail) {
			rootTabs_->SlideToPage(1, SlideDirection::Vertical, 280);
			AnimatePageReveal(detailFrame_, 0.78f, 280);
		}
		else {
			rootTabs_->SetPageIndex(1);
		}
		if (detailFrame_) {
			detailFrame_->InvalidateLayout();
			detailFrame_->RefreshLayout();
			detailFrame_->Invalidate();
		}
	}
	else {
		int contentIndex = 0;
		switch (page) {
		case ContentPage::Home:
			contentIndex = 0;
			break;
		case ContentPage::Search:
			contentIndex = 1;
			break;
		case ContentPage::Library:
			contentIndex = 2;
			break;
		case ContentPage::Detail:
			contentIndex = 0;
			break;
		}

		if (oldPage == ContentPage::Detail) {
			rootTabs_->SlideToPage(0, SlideDirection::Vertical, 260);
			contentTabs_->SetPageIndex(contentIndex);
			AnimatePageReveal(contentTabs_->GetPage(contentIndex), 0.76f, 220);
		}
		else {
			rootTabs_->SetPageIndex(0);
			if (oldPage != page) {
				contentTabs_->SetPageIndex(contentIndex);
			}
			else {
				contentTabs_->SetPageIndex(contentIndex);
			}
		}
		contentTabs_->InvalidateLayout();
		contentTabs_->RefreshLayout();
		contentTabs_->Invalidate();
	}

	rootTabs_->InvalidateLayout();
	rootTabs_->RefreshLayout();
	rootTabs_->Invalidate();
	UpdateSidebarSelection();
}

void MainForm::NavigateBack()
{
	if (currentPage_ == ContentPage::Detail) {
		ShowPage(previousPage_, false);
		return;
	}

	if (currentPage_ != ContentPage::Home) {
		ShowPage(ContentPage::Home, false);
	}
}

#if 0
void MainForm::OpenLibrary(LibraryView view)
{
	currentLibraryView_ = view;
	if (view == LibraryView::Daily30) {
		RefreshDailySongs();
	}
	else {
		PopulateLibraryList();
	}
	ShowPage(ContentPage::Library, false);

	if (false) {
		SetSidebarStatus(L"每日30首功能预留中");
	}
}

#endif

void MainForm::OpenLibrary(LibraryView view)
{
	currentLibraryView_ = view;
	if (view == LibraryView::Daily30) {
		RefreshDailySongs();
	}
	else {
		PopulateLibraryList();
	}

	ShowPage(ContentPage::Library, false);
}

void MainForm::StartSearch(const UIString& keyword, bool autoPlayFirst)
{
	const auto trimmed = keyword.trim();
	if (trimmed.empty()) {
		return;
	}

	currentQuery_ = trimmed;
	currentSearchPage_ = 0;
	hasMoreResults_ = false;
	isLoadingResults_ = false;
	pendingAutoPlayFirstResult_ = autoPlayFirst;
	++searchToken_;

	selectedSearchHash_.clear();
	searchResults_.clear();
	ShowPage(ContentPage::Search, false);

	if (searchTitle_) {
		searchTitle_->SetText(UIString(L"搜索 “") + trimmed + UIString(L"”"));
		searchTitle_->Invalidate();
	}
	if (searchResultCount_) {
		searchResultCount_->SetText(L"搜索中");
		searchResultCount_->Invalidate();
	}

	SetSearchMeta(UIString(L"正在搜索 “") + trimmed + UIString(L"” ..."));
	SetSidebarStatus(UIString(L"正在搜索：") + trimmed);
	RequestSearchPage(1, false);
}

void MainForm::SearchSinger(const UIString& singerName)
{
	const auto primarySinger = FirstSingerName(singerName);
	if (primarySinger.empty()) {
		return;
	}

	if (searchBox_) {
		searchBox_->SetText(primarySinger);
	}
	if (searchDebounce_) {
		searchDebounce_->Cancel();
	}

	StartSearch(primarySinger);
}

void MainForm::RequestSearchPage(int page, bool append)
{
	if (isLoadingResults_ || currentQuery_.empty()) {
		return;
	}

	isLoadingResults_ = true;
	const auto token = searchToken_;
	const auto query = currentQuery_;
	auto weakThis = GetWeakPtr<MainForm>();

	QQMusicTaskPool().Add([weakThis, token, page, append, query]() {
		UIString error;
		bool hasMore = false;
		auto songs = kugou::SearchSongs(query, page, 20, &hasMore, &error);
		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			return;
		}

		BeginInvoke([weakThis, token, page, append, hasMore, error, songs = std::move(songs)]() mutable {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested() || token != weakThis->searchToken_) {
				return;
			}

			weakThis->isLoadingResults_ = false;
			weakThis->hasMoreResults_ = hasMore;
			weakThis->currentSearchPage_ = page;

			if (!error.empty() && songs.empty()) {
				weakThis->SetSearchMeta(error);
				if (weakThis->searchResultCount_) {
					weakThis->searchResultCount_->SetText(L"搜索失败");
					weakThis->searchResultCount_->Invalidate();
				}
				return;
			}

			weakThis->RenderSearchResults(songs, append);
			});
		});
}

void MainForm::RenderSearchResults(const std::vector<kugou::SongSummary>& songs, bool append)
{
	if (!searchSongList_) {
		return;
	}

	if (!append) {
		searchSongList_->RemoveAll(true);
		searchResults_.clear();
	}

	const int startIndex = static_cast<int>(searchResults_.size());
	searchResults_.insert(searchResults_.end(), songs.begin(), songs.end());

	if (searchResults_.empty()) {
		RenderEmptyState(searchSongList_, L"没有找到匹配的歌曲，换个关键词试试。");
	}
	else {
		AppendSongRows(searchSongList_, searchResults_, startIndex, false);
		UpdateSearchRowStates();
	}

	searchSongList_->RefreshLayout();
	searchSongList_->Invalidate();

	if (searchResultCount_) {
		searchResultCount_->SetText(UIString(L"已加载 ") + ToUiString(static_cast<int>(searchResults_.size())) + UIString(L" 首"));
		searchResultCount_->Invalidate();
	}

	if (searchResults_.empty()) {
		SetSearchMeta(UIString(L"“") + currentQuery_ + UIString(L"” 没有搜索到结果。"));
	}
	else if (hasMoreResults_) {
		SetSearchMeta(UIString(L"“") + currentQuery_ + UIString(L"” 已加载 ") + ToUiString(static_cast<int>(searchResults_.size())) + UIString(L" 首，继续下滑可加载更多。"));
	}
	else {
		SetSearchMeta(UIString(L"“") + currentQuery_ + UIString(L"” 共加载 ") + ToUiString(static_cast<int>(searchResults_.size())) + UIString(L" 首歌曲。"));
	}

	if (pendingAutoPlayFirstResult_ && !searchResults_.empty()) {
		pendingAutoPlayFirstResult_ = false;
		PlaySongFromSearch(0);
	}
}

void MainForm::RefreshDailySongs(bool forceRefresh)
{
	const auto todayKey = CurrentDayKey();
	const auto cachedDay = daily30MetaIni_ ? daily30MetaIni_->ReadString("daily30", "date") : UIString();
	const bool hasCache = !dailySongs_.empty();
	const bool needsRefresh = forceRefresh || !hasCache || cachedDay != todayKey;

	if (!needsRefresh) {
		if (currentLibraryView_ == LibraryView::Daily30) {
			PopulateLibraryList();
		}
		SetSidebarStatus(L"今日每日30首已准备好。");
		return;
	}

	if (isRefreshingDaily30_) {
		if (currentLibraryView_ == LibraryView::Daily30) {
			PopulateLibraryList();
		}
		return;
	}

	isRefreshingDaily30_ = true;
	if (currentLibraryView_ == LibraryView::Daily30) {
		PopulateLibraryList();
	}
	SetSidebarStatus(hasCache && !forceRefresh ? L"正在更新今日每日30首..." : L"正在获取今日每日30首...");

	auto weakThis = GetWeakPtr<MainForm>();
	QQMusicTaskPool().Add([weakThis, todayKey]() {
		UIString error;
		bool hasMore = false;
		auto songs = kugou::SearchSongs(UIString(), 1, 60, &hasMore, &error);

		std::vector<kugou::SongSummary> dailySongs;
		std::unordered_set<std::wstring> seenHashes;
		dailySongs.reserve(30);
		for (auto& song : songs) {
			if (song.hash.empty() || song.mvHash.empty()) {
				continue;
			}

			const auto hash = song.hash.unicode();
			if (!seenHashes.insert(hash).second) {
				continue;
			}

			dailySongs.push_back(song);
			if (dailySongs.size() >= 30) {
				break;
			}
		}

		if (dailySongs.empty() && error.empty()) {
			error = L"今日推荐暂时获取失败，请稍后再试。";
		}
		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			return;
		}

		BeginInvoke([weakThis, todayKey, error, dailySongs = std::move(dailySongs)]() mutable {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}

			weakThis->isRefreshingDaily30_ = false;

			if (dailySongs.empty()) {
				if (!error.empty()) {
					weakThis->SetSidebarStatus(error);
					if (weakThis->currentLibraryView_ == LibraryView::Daily30) {
						weakThis->PopulateLibraryList();
						if (weakThis->dailySongs_.empty()) {
							weakThis->SetLibraryMeta(error);
						}
					}
				}
				return;
			}

			weakThis->dailySongs_.clear();
			weakThis->ClearIniSections(weakThis->daily30Ini_.get());

			const auto baseTimestamp = CurrentUnixTimestamp();
			for (size_t i = 0; i < dailySongs.size(); ++i) {
				auto storedSong = dailySongs[i];
				storedSong.updatedAt = baseTimestamp - static_cast<long long>(i);
				weakThis->dailySongs_.push_back(storedSong);
				weakThis->SaveSongToIni(weakThis->daily30Ini_.get(), storedSong);
			}

			if (weakThis->daily30MetaIni_) {
				weakThis->daily30MetaIni_->WriteString("daily30", "date", todayKey);
				weakThis->daily30MetaIni_->WriteInt("daily30", "count", static_cast<int>(weakThis->dailySongs_.size()));
			}

			if (weakThis->currentLibraryView_ == LibraryView::Daily30) {
				weakThis->PopulateLibraryList();
			}
			weakThis->SetSidebarStatus(UIString(L"今日每日30首已更新，共 ") + ToUiString(static_cast<int>(weakThis->dailySongs_.size())) + UIString(L" 首。"));
		});
	});
}

void MainForm::RefreshHomeRecommendations()
{
	if (isRefreshingRecommendations_) {
		return;
	}

	isRefreshingRecommendations_ = true;
	UpdateHomeSummary();
	SetSidebarStatus(L"正在刷新“为你推荐”...");

	auto weakThis = GetWeakPtr<MainForm>();
	QQMusicTaskPool().Add([weakThis]() {
		UIString error;
		bool hasMore = false;
		auto songs = kugou::SearchSongs(UIString(), 1, 100, &hasMore, &error);

		std::vector<kugou::SongSummary> recommendedSongs;
		std::unordered_set<std::wstring> seenHashes;
		recommendedSongs.reserve(50);
		for (auto& song : songs) {
			if (song.hash.empty() || song.mvHash.empty()) {
				continue;
			}

			const auto hash = song.hash.unicode();
			if (!seenHashes.insert(hash).second) {
				continue;
			}

			recommendedSongs.push_back(song);
			if (recommendedSongs.size() >= 50) {
				break;
			}
		}

		if (recommendedSongs.empty() && error.empty()) {
			error = L"“为你推荐”暂时获取失败，请稍后再试。";
		}
		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			return;
		}

		BeginInvoke([weakThis, error, recommendedSongs = std::move(recommendedSongs)]() mutable {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}

			weakThis->isRefreshingRecommendations_ = false;
			if (!recommendedSongs.empty()) {
				weakThis->recommendedSongs_ = std::move(recommendedSongs);
				weakThis->SetSidebarStatus(
					UIString(L"“为你推荐”已刷新，共 ")
					+ ToUiString(static_cast<int>(weakThis->recommendedSongs_.size()))
					+ UIString(L" 首。"));
			}
			else if (!error.empty()) {
				weakThis->SetSidebarStatus(error);
			}

			weakThis->UpdateHomeSummary();
			if (weakThis->currentPage_ == ContentPage::Search && weakThis->currentQuery_.empty()) {
				weakThis->OpenRecommendedList();
			}
		});
	});
}

void MainForm::OpenRecommendedList()
{
	++searchToken_;
	currentQuery_.clear();
	currentSearchPage_ = 0;
	hasMoreResults_ = false;
	isLoadingResults_ = false;
	pendingAutoPlayFirstResult_ = false;
	selectedSearchHash_.clear();

	ShowPage(ContentPage::Search, false);

	if (searchTitle_) {
		searchTitle_->SetText(L"\u4e3a\u4f60\u63a8\u8350");
		searchTitle_->Invalidate();
	}

	if (searchSongList_) {
		searchSongList_->RemoveAll(true);
	}
	searchResults_ = recommendedSongs_;

	if (searchResultCount_) {
		searchResultCount_->SetText(
			UIString(L"\u5df2\u51c6\u5907 ")
			+ ToUiString(static_cast<int>(searchResults_.size()))
			+ UIString(L" \u9996"));
		searchResultCount_->Invalidate();
	}

	if (!searchSongList_) {
		SetSearchMeta(L"\u63a8\u8350\u6b4c\u5355\u5df2\u6253\u5f00\u3002");
		return;
	}

	if (searchResults_.empty()) {
		const UIString meta = isRefreshingRecommendations_
			? UIString(L"\u6b63\u5728\u5237\u65b0\u63a8\u8350\u5217\u8868...")
			: UIString(L"\u6682\u65f6\u6ca1\u6709\u53ef\u64ad\u653e\u7684\u63a8\u8350\u6b4c\u66f2\u3002");
		RenderEmptyState(searchSongList_, meta);
		SetSearchMeta(meta);
		return;
	}

	AppendSongRows(searchSongList_, searchResults_, 0, false);
	UpdateSearchRowStates();
	searchSongList_->RefreshLayout();
	searchSongList_->Invalidate();
	SetSearchMeta(
		UIString(L"\u542f\u52a8\u5df2\u81ea\u52a8\u5237\u65b0\uff0c\u5171 ")
		+ ToUiString(static_cast<int>(searchResults_.size()))
		+ UIString(L" \u9996\u53ef\u76f4\u63a5\u64ad\u653e\u63a8\u8350\u3002"));
}

void MainForm::AppendSongRows(VListView* list, const std::vector<kugou::SongSummary>& songs, int startIndex, bool libraryMode)
{
	if (!list) {
		return;
	}

	for (int i = startIndex; i < static_cast<int>(songs.size()); ++i) {
		auto* row = new SongRowItem(songs[i]);
		row->SetLiked(IsFavorite(songs[i].hash));
		row->SetPlaying(songs[i].hash == currentSongHash_);
		row->SetSelected((libraryMode ? selectedLibraryHash_ : selectedSearchHash_) == songs[i].hash);
		row->SetDownloadProgress(libraryMode && currentLibraryView_ == LibraryView::Download ? GetActiveDownloadProgress(songs[i].hash) : -1.0f);

		row->Clicked = [this, libraryMode](SongRowItem* item) {
			if (libraryMode) {
				selectedLibraryHash_ = item->GetSong().hash;
				UpdateLibraryRowStates();
				SetLibraryMeta(UIString(L"已选中：") + item->GetSong().songName);
			}
			else {
				selectedSearchHash_ = item->GetSong().hash;
				UpdateSearchRowStates();
				SetSearchMeta(UIString(L"已选中：") + item->GetSong().songName);
			}
			};
		row->DoubleClicked = [this, libraryMode, i](SongRowItem* item) {
			if (libraryMode) {
				selectedLibraryHash_ = item->GetSong().hash;
				UpdateLibraryRowStates();
				PlaySongFromLibrary(i);
			}
			else {
				selectedSearchHash_ = item->GetSong().hash;
				UpdateSearchRowStates();
				PlaySongFromSearch(i);
			}
			};
		row->FavoriteClicked = [this](SongRowItem* item) {
			ToggleFavorite(item->GetSong());
			};
		row->SingerClicked = [this](SongRowItem* item) {
			SearchSinger(item->GetSong().singerName);
			};

		list->AddChild(row, true);
	}
}

void MainForm::RenderEmptyState(VListView* list, const UIString& text)
{
	if (!list) {
		return;
	}

	list->RemoveAll(true);
	list->AddChild(new SongListMessageItem(text), true);
}

#if 0
void MainForm::PopulateLibraryList()
{
	if (!librarySongList_) {
		return;
	}

	librarySongList_->RemoveAll(true);
	libraryViewSongs_.clear();

	if (libraryClearBtn_) {
		libraryClearBtn_->SetText(currentLibraryView_ == LibraryView::Daily30 ? L"刷新" : L"清空");
		libraryClearBtn_->Invalidate();
	}

	UIString emptyText;
	switch (currentLibraryView_) {
	case LibraryView::Likes:
		libraryViewSongs_ = favorites_;
		if (libraryTitle_) {
			libraryTitle_->SetText(L"喜欢");
			libraryTitle_->Invalidate();
		}
		SetLibraryMeta(UIString(L"共收藏 ") + ToUiString(static_cast<int>(favorites_.size())) + UIString(L" 首歌曲。"));
		emptyText = L"还没有喜欢的歌曲，点点心形试试看。";
		break;
	case LibraryView::Recent:
		libraryViewSongs_ = recentPlays_;
		if (libraryTitle_) {
			libraryTitle_->SetText(L"最近播放");
			libraryTitle_->Invalidate();
		}
		SetLibraryMeta(UIString(L"共记录 ") + ToUiString(static_cast<int>(recentPlays_.size())) + UIString(L" 首最近播放。"));
		emptyText = L"还没有最近播放记录。";
		break;
	case LibraryView::Download:
		libraryViewSongs_ = BuildDownloadLibrarySongs();
		if (libraryTitle_) {
			libraryTitle_->SetText(L"本地下载");
			libraryTitle_->Invalidate();
		}
		SetLibraryMeta(UIString(L"共保存 ") + ToUiString(static_cast<int>(downloadedSongs_.size())) + UIString(L" 首本地歌曲。"));
		emptyText = L"还没有下载歌曲。";
		break;
	case LibraryView::Daily30:
		dailySongs_.clear();
		ClearIniSections(daily30Ini_.get());
		ClearIniSections(daily30MetaIni_.get());
		SetSidebarStatus(L"正在刷新每日30首...");
		RefreshDailySongs(true);
		return;
		libraryViewSongs_ = dailySongs_;
		if (libraryTitle_) {
			libraryTitle_->SetText(L"每日30首");
			libraryTitle_->Invalidate();
		}
		SetLibraryMeta(L"入口保留，功能暂未实现。");
		emptyText = L"每日30首功能预留中。";
		{
			const auto cachedDay = daily30MetaIni_ ? daily30MetaIni_->ReadString("daily30", "date") : UIString();
			if (isRefreshingDaily30_ && libraryViewSongs_.empty()) {
				SetLibraryMeta(L"正在为你获取今日随机歌单...");
				emptyText = L"今日推荐加载中...";
			}
			else if (isRefreshingDaily30_) {
				SetLibraryMeta(UIString(L"已缓存 ") + ToUiString(static_cast<int>(libraryViewSongs_.size())) + UIString(L" 首歌曲，正在刷新今日推荐..."));
				emptyText = L"正在刷新今日推荐...";
			}
			else if (!libraryViewSongs_.empty()) {
				const auto metaSuffix = cachedDay.empty() ? UIString() : UIString(L"（") + cachedDay + UIString(L"）");
				SetLibraryMeta(UIString(L"今日随机推荐 ") + ToUiString(static_cast<int>(libraryViewSongs_.size())) + UIString(L" 首歌曲") + metaSuffix + UIString(L"。"));
				emptyText = L"今日推荐已经准备好了。";
			}
			else {
				SetLibraryMeta(L"今日随机歌单暂时为空，可以点刷新重新获取。");
				emptyText = L"今日推荐暂时为空。";
			}
		}
		break;
	}

	if (FindSongIndexByHash(libraryViewSongs_, selectedLibraryHash_) < 0) {
		selectedLibraryHash_.clear();
	}

	if (libraryViewSongs_.empty()) {
		RenderEmptyState(librarySongList_, emptyText);
	}
	else {
		AppendSongRows(librarySongList_, libraryViewSongs_, 0, true);
		UpdateLibraryRowStates();
	}

	librarySongList_->RefreshLayout();
	librarySongList_->Invalidate();
}
#endif

void MainForm::PopulateLibraryList()
{
	if (!librarySongList_) {
		return;
	}

	librarySongList_->RemoveAll(true);
	libraryViewSongs_.clear();

	if (libraryClearBtn_) {
		libraryClearBtn_->SetText(currentLibraryView_ == LibraryView::Daily30 ? L"刷新" : L"清空");
		libraryClearBtn_->Invalidate();
	}

	UIString emptyText;
	switch (currentLibraryView_) {
	case LibraryView::Likes:
		libraryViewSongs_ = favorites_;
		if (libraryTitle_) {
			libraryTitle_->SetText(L"喜欢");
			libraryTitle_->Invalidate();
		}
		SetLibraryMeta(UIString(L"共收藏 ") + ToUiString(static_cast<int>(favorites_.size())) + UIString(L" 首歌曲。"));
		emptyText = L"还没有喜欢的歌曲，点点心形试试看。";
		break;
	case LibraryView::Recent:
		libraryViewSongs_ = recentPlays_;
		if (libraryTitle_) {
			libraryTitle_->SetText(L"最近播放");
			libraryTitle_->Invalidate();
		}
		SetLibraryMeta(UIString(L"共记录 ") + ToUiString(static_cast<int>(recentPlays_.size())) + UIString(L" 首最近播放。"));
		emptyText = L"还没有最近播放记录。";
		break;
	case LibraryView::Download:
		libraryViewSongs_ = BuildDownloadLibrarySongs();
		if (libraryTitle_) {
			libraryTitle_->SetText(L"本地下载");
			libraryTitle_->Invalidate();
		}
		if (!activeDownloads_.empty()) {
			SetLibraryMeta(
				UIString(L"共保存 ")
				+ ToUiString(static_cast<int>(downloadedSongs_.size()))
				+ UIString(L" 首本地歌曲，正在下载 ")
				+ ToUiString(static_cast<int>(activeDownloads_.size()))
				+ UIString(L" 首。"));
			emptyText = L"当前正在下载，完成后会自动归档到本地下载。";
		}
		else {
			SetLibraryMeta(UIString(L"共保存 ") + ToUiString(static_cast<int>(downloadedSongs_.size())) + UIString(L" 首本地歌曲。"));
			emptyText = L"还没有下载歌曲。";
		}
		break;
	case LibraryView::Daily30: {
		libraryViewSongs_ = dailySongs_;
		if (libraryTitle_) {
			libraryTitle_->SetText(L"每日30首");
			libraryTitle_->Invalidate();
		}

		const auto cachedDay = daily30MetaIni_ ? daily30MetaIni_->ReadString("daily30", "date") : UIString();
		if (isRefreshingDaily30_ && libraryViewSongs_.empty()) {
			SetLibraryMeta(L"正在为你获取今日随机歌单...");
			emptyText = L"今日推荐加载中...";
		}
		else if (isRefreshingDaily30_) {
			SetLibraryMeta(UIString(L"已缓存 ") + ToUiString(static_cast<int>(libraryViewSongs_.size())) + UIString(L" 首歌曲，正在刷新今日推荐..."));
			emptyText = L"正在刷新今日推荐...";
		}
		else if (!libraryViewSongs_.empty()) {
			const auto metaSuffix = cachedDay.empty() ? UIString() : UIString(L"（") + cachedDay + UIString(L"）");
			SetLibraryMeta(UIString(L"今日随机推荐 ") + ToUiString(static_cast<int>(libraryViewSongs_.size())) + UIString(L" 首歌曲") + metaSuffix + UIString(L"。"));
			emptyText = L"今日推荐已经准备好了。";
		}
		else {
			SetLibraryMeta(L"今日随机歌单暂时为空，可以点刷新重新获取。");
			emptyText = L"今日推荐暂时为空。";
		}
		break;
	}
	}

	if (FindSongIndexByHash(libraryViewSongs_, selectedLibraryHash_) < 0) {
		selectedLibraryHash_.clear();
	}

	if (libraryViewSongs_.empty()) {
		RenderEmptyState(librarySongList_, emptyText);
	}
	else {
		AppendSongRows(librarySongList_, libraryViewSongs_, 0, true);
		UpdateLibraryRowStates();
	}

	librarySongList_->RefreshLayout();
	librarySongList_->Invalidate();
	if (currentLibraryView_ == LibraryView::Download) {
		RefreshVisibleDownloadProgress();
	}
}

void MainForm::UpdateSearchRowStates()
{
	UpdateRowStates(searchSongList_, selectedSearchHash_);
}

void MainForm::UpdateLibraryRowStates()
{
	UpdateRowStates(librarySongList_, selectedLibraryHash_);
}

void MainForm::UpdateRowStates(VListView* list, const UIString& selectedHash)
{
	if (!list) {
		return;
	}

	for (auto* child : list->GetChildren()) {
		if (auto* row = dynamic_cast<SongRowItem*>(child)) {
			row->SetLiked(IsFavorite(row->GetSong().hash));
			row->SetSelected(row->GetSong().hash == selectedHash);
			row->SetPlaying(row->GetSong().hash == currentSongHash_);
			row->SetDownloadProgress(list == librarySongList_ && currentLibraryView_ == LibraryView::Download
				? GetActiveDownloadProgress(row->GetSong().hash)
				: -1.0f);
		}
	}
}

void MainForm::PlaySongFromSearch(int index)
{
	if (index < 0 || index >= static_cast<int>(searchResults_.size())) {
		return;
	}

	playQueue_ = searchResults_;
	currentSongIndex_ = index;
	currentSongHash_ = playQueue_[index].hash;
	UpdateSearchRowStates();
	UpdateLibraryRowStates();

	if (searchSongList_ && index < static_cast<int>(searchSongList_->GetChildren().size()) && searchSongList_->GetScrollBar()) {
		searchSongList_->GetScrollBar()->ScrollTo(searchSongList_->GetChildren()[index]);
	}

	PlaySongAt(index);
}

void MainForm::PlaySongFromLibrary(int index)
{
	if (index < 0 || index >= static_cast<int>(libraryViewSongs_.size())) {
		return;
	}

	playQueue_ = libraryViewSongs_;
	currentSongIndex_ = index;
	currentSongHash_ = playQueue_[index].hash;
	UpdateSearchRowStates();
	UpdateLibraryRowStates();

	if (librarySongList_ && index < static_cast<int>(librarySongList_->GetChildren().size()) && librarySongList_->GetScrollBar()) {
		librarySongList_->GetScrollBar()->ScrollTo(librarySongList_->GetChildren()[index]);
	}

	PlaySongAt(index);
}

void MainForm::PlayRecommendedSong()
{
	const auto* song = GetPrimaryRecommendation();
	if (!song) {
		if (!isRefreshingRecommendations_) {
			RefreshHomeRecommendations();
		}
		return;
	}

	playQueue_ = recommendedSongs_;
	currentSongIndex_ = 0;
	currentSongHash_ = song->hash;
	UpdateSearchRowStates();
	UpdateLibraryRowStates();
	PlaySongAt(0);
}

void MainForm::PlaySongAt(int index)
{
	if (!player_ || index < 0 || index >= static_cast<int>(playQueue_.size())) {
		return;
	}

	currentSongIndex_ = index;
	currentSongHash_ = playQueue_[index].hash;
	UpdateSearchRowStates();
	UpdateLibraryRowStates();
	UpdateFavoriteButtons();
	UpdatePlayButtonState();

	const auto song = playQueue_[index];
	if (detailLrc_) {
		detailLrc_->ClearLrc();
	}
	UpdateDetailStatus(L"歌词加载中...");
	SetSidebarStatus(UIString(L"正在准备播放：") + (song.songName.empty() ? UIString(L"当前歌曲") : song.songName));
	if (progressSlider_) {
		progressSlider_->SetValue(0);
	}
	if (detailProgressSlider_) {
		detailProgressSlider_->SetValue(0);
	}

	const int token = ++playToken_;
	auto weakThis = GetWeakPtr<MainForm>();

	QQMusicTaskPool().Add([weakThis, token, song]() {
		kugou::SongPlaybackInfo info;
		UIString error;
		bool useLocalFile = !song.localPath.empty() && File::Exists(song.localPath);
		bool ok = true;

		if (useLocalFile) {
			info.hash = song.hash;
			info.songName = song.songName;
			info.singerName = song.singerName;
			info.displayName = song.songName.empty() ? UIString(L"本地歌曲") : song.songName;
			info.streamUrl = song.localPath;
			info.durationSeconds = song.durationSeconds;

			kugou::SongPlaybackInfo remoteInfo;
			if (kugou::GetSongPlaybackInfo(song.hash, song.mvHash, remoteInfo, nullptr)) {
				if (info.songName.empty()) {
					info.songName = remoteInfo.songName;
				}
				if (info.singerName.empty()) {
					info.singerName = remoteInfo.singerName;
				}
				if (info.durationSeconds <= 0) {
					info.durationSeconds = remoteInfo.durationSeconds;
				}
				info.imageUrl = remoteInfo.imageUrl;
			}
		}
		else {
			ok = kugou::GetSongPlaybackInfo(song.hash, song.mvHash, info, &error);
		}
		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			return;
		}

		BeginInvoke([weakThis, token, song, ok, info, error, useLocalFile]() mutable {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested() || token != weakThis->playToken_) {
				return;
			}

			if (!ok) {
				weakThis->UpdatePlayButtonState();
				weakThis->UpdateDetailStatus(error.empty() ? UIString(L"歌曲暂时无法播放。") : error);
				weakThis->SetSidebarStatus(error.empty() ? UIString(L"歌曲暂时无法播放。") : error);
				::MessageBoxW(weakThis->GetWindowHandle(),
					(error.empty() ? UIString(L"歌曲暂时无法播放。") : error).unicode().c_str(),
					L"提示", MB_OK);
				return;
			}

			auto mergedSong = MergeSongSummary(song, info);
			weakThis->playQueue_[weakThis->currentSongIndex_] = mergedSong;
			weakThis->currentSongHash_ = mergedSong.hash;
			weakThis->currentPlaybackInfo_ = info;

			if (useLocalFile) {
				weakThis->player_->OpenPath(info.streamUrl);
			}
			else {
				weakThis->player_->OpenUrl(info.streamUrl);
			}
			if (info.durationSeconds > 0) {
				weakThis->player_->SetDuration(info.durationSeconds);
			}
			weakThis->player_->Play();

			weakThis->UpdateNowPlaying(mergedSong, info);
			weakThis->PushRecent(mergedSong);
			weakThis->UpdatePlayButtonState();

			if (weakThis->progressTimer_) {
				weakThis->progressTimer_->Start();
			}

			weakThis->LoadLyricsAsync(mergedSong, token);
			weakThis->UpdateArtworkAsync(mergedSong, info, token);
			});
		});
}

void MainForm::TogglePlayPause()
{
	if (!player_) {
		return;
	}

	const auto state = player_->GetState();
	if (state == libvlc_Playing || state == libvlc_Buffering || state == libvlc_Opening) {
		player_->Pause();
		UpdatePlayButtonState();
		UpdateDetailStatus(L"播放已暂停。");
		return;
	}

	if (currentPlaybackInfo_.streamUrl.empty()) {
		if (!playQueue_.empty() && currentSongIndex_ >= 0) {
			PlaySongAt(currentSongIndex_);
			return;
		}
		if (!recentPlays_.empty()) {
			playQueue_ = recentPlays_;
			currentSongIndex_ = 0;
			PlaySongAt(0);
			return;
		}
		PlayRecommendedSong();
		return;
	}

	player_->Play();
	UpdatePlayButtonState();
	UpdateDetailStatus(L"播放继续。");
}

void MainForm::PlayPreviousSong()
{
	if (playQueue_.empty()) {
		if (!recentPlays_.empty()) {
			playQueue_ = recentPlays_;
			currentSongIndex_ = 0;
			PlaySongAt(0);
		}
		return;
	}

	if (currentSongIndex_ < 0) {
		currentSongIndex_ = 0;
	}
	else {
		currentSongIndex_ = (currentSongIndex_ - 1 + static_cast<int>(playQueue_.size())) % static_cast<int>(playQueue_.size());
	}

	PlaySongAt(currentSongIndex_);
}

void MainForm::PlayNextSong()
{
	if (playQueue_.empty()) {
		if (!recentPlays_.empty()) {
			playQueue_ = recentPlays_;
			currentSongIndex_ = 0;
			PlaySongAt(0);
		}
		return;
	}

	if (currentSongIndex_ < 0) {
		currentSongIndex_ = 0;
	}
	else {
		currentSongIndex_ = (currentSongIndex_ + 1) % static_cast<int>(playQueue_.size());
	}

	PlaySongAt(currentSongIndex_);
}

void MainForm::SeekFromSlider(Slider* slider)
{
	if (!player_ || !slider) {
		return;
	}

	player_->SetPosition(slider->GetRatio());
	if (player_->GetState() != libvlc_Playing) {
		player_->Play();
	}
	UpdateProgress();
	UpdatePlayButtonState();
}

void MainForm::UpdatePlayButtonState()
{
	if (!player_) {
		return;
	}

	const auto state = player_->GetState();
	const bool isPlaybackActive = (state == libvlc_Playing || state == libvlc_Buffering || state == libvlc_Opening);
	const UIString buttonText = isPlaybackActive
		? UIString(L"❚❚")
		: UIString(L"▶");

	if (playPauseBtn_) {
		playPauseBtn_->SetText(buttonText);
		playPauseBtn_->Invalidate();
	}
	if (detailPlayPauseBtn_) {
		detailPlayPauseBtn_->SetText(buttonText);
		detailPlayPauseBtn_->Invalidate();
	}

	if (artworkRotationTimer_) {
		if (isPlaybackActive) {
			artworkRotationTimer_->Start();
		}
		else {
			artworkRotationTimer_->Stop();
		}
	}
}

void MainForm::UpdateNowPlaying(const kugou::SongSummary& song, const kugou::SongPlaybackInfo& info)
{
	const UIString title = song.songName.empty() ? (info.songName.empty() ? info.displayName : info.songName) : song.songName;
	const UIString singer = song.singerName.empty() ? (info.singerName.empty() ? UIString(L"未知歌手") : info.singerName) : song.singerName;
	const UIString album = song.albumName.empty() ? UIString(L"专辑信息同步中") : song.albumName;

	SetTitle(title.empty() ? UIString(L"QQ音乐") : title);

	if (dockTitle_) {
		dockTitle_->SetText(title);
		dockTitle_->Invalidate();
	}
	if (dockSinger_) {
		dockSinger_->SetText(singer);
		dockSinger_->Invalidate();
	}
	if (detailTitle_) {
		detailTitle_->SetText(title);
		detailTitle_->Invalidate();
	}
	if (detailSinger_) {
		detailSinger_->SetText(singer);
		detailSinger_->Invalidate();
	}
	if (detailMeta_) {
		detailMeta_->SetText(singer + UIString(L" / ") + album);
		detailMeta_->Invalidate();
	}
	if (elapsedLabel_) {
		elapsedLabel_->SetText(L"00:00");
		elapsedLabel_->Invalidate();
	}
	if (detailElapsedLabel_) {
		detailElapsedLabel_->SetText(L"00:00");
		detailElapsedLabel_->Invalidate();
	}
	if (totalLabel_) {
		totalLabel_->SetText(kugou::ToTimeString(info.durationSeconds));
		totalLabel_->Invalidate();
	}
	if (detailTotalLabel_) {
		detailTotalLabel_->SetText(kugou::ToTimeString(info.durationSeconds));
		detailTotalLabel_->Invalidate();
	}
	if (progressSlider_) {
		progressSlider_->SetMinValue(0);
		progressSlider_->SetMaxValue((std::max)(1, info.durationSeconds * 1000));
		progressSlider_->SetValue(0);
		progressSlider_->Invalidate();
	}
	if (detailProgressSlider_) {
		detailProgressSlider_->SetMinValue(0);
		detailProgressSlider_->SetMaxValue((std::max)(1, info.durationSeconds * 1000));
		detailProgressSlider_->SetValue(0);
		detailProgressSlider_->Invalidate();
	}

	SetSidebarStatus(UIString(L"正在播放：") + title + UIString(L" - ") + singer);
	UpdateDetailStatus(L"歌词加载中...");
	UpdateFavoriteButtons();
	UpdateHomeSummary();
}

void MainForm::UpdateProgress()
{
	if (!player_) {
		return;
	}

	const auto state = player_->GetState();
	if (state == libvlc_Ended) {
		if (progressTimer_) {
			progressTimer_->Stop();
		}
		PlayNextSong();
		return;
	}

	const auto positionMs = player_->Position();
	const auto durationMs = std::max<long long>(player_->Duration(), static_cast<long long>(currentPlaybackInfo_.durationSeconds) * 1000);

	if (positionMs >= 0 && elapsedLabel_) {
		elapsedLabel_->SetText(kugou::ToTimeString(static_cast<int>(positionMs / 1000)));
		elapsedLabel_->Invalidate();
	}
	if (positionMs >= 0 && detailElapsedLabel_) {
		detailElapsedLabel_->SetText(kugou::ToTimeString(static_cast<int>(positionMs / 1000)));
		detailElapsedLabel_->Invalidate();
	}
	if (durationMs > 0 && totalLabel_) {
		totalLabel_->SetText(kugou::ToTimeString(static_cast<int>(durationMs / 1000)));
		totalLabel_->Invalidate();
	}
	if (durationMs > 0 && detailTotalLabel_) {
		detailTotalLabel_->SetText(kugou::ToTimeString(static_cast<int>(durationMs / 1000)));
		detailTotalLabel_->Invalidate();
	}
	if (progressSlider_ && progressSlider_->PressedButton() != MouseButton::Left) {
		progressSlider_->SetMinValue(0);
		progressSlider_->SetMaxValue(static_cast<int>(std::max<long long>(1, durationMs)));
		progressSlider_->SetValue(static_cast<int>(std::max<long long>(0, positionMs)));
		progressSlider_->Invalidate();
	}
	if (detailProgressSlider_ && detailProgressSlider_->PressedButton() != MouseButton::Left) {
		detailProgressSlider_->SetMinValue(0);
		detailProgressSlider_->SetMaxValue(static_cast<int>(std::max<long long>(1, durationMs)));
		detailProgressSlider_->SetValue(static_cast<int>(std::max<long long>(0, positionMs)));
		detailProgressSlider_->Invalidate();
	}
	if (detailLrc_ && positionMs >= 0) {
		detailLrc_->ChangePostion(static_cast<int>(positionMs));
	}

	UpdatePlayButtonState();
}

void MainForm::LoadLyricsAsync(const kugou::SongSummary& song, int token)
{
	auto weakThis = GetWeakPtr<MainForm>();

	QQMusicTaskPool().Add([weakThis, token, song]() {
		UIString error;
		const auto lyric = kugou::GetSongLyrics(song.hash, song.albumId, &error);
		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			return;
		}

		BeginInvoke([weakThis, token, lyric, error]() {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested() || token != weakThis->playToken_) {
				return;
			}

			if (weakThis->detailLrc_) {
				weakThis->detailLrc_->LoadLrc(lyric);
			}
			weakThis->UpdateDetailStatus(error.empty() ? UIString(L"歌词已同步到当前播放进度。") : error);
			});
		});
}

void MainForm::UpdateArtworkAsync(const kugou::SongSummary& song, const kugou::SongPlaybackInfo& info, int token)
{
	auto weakThis = GetWeakPtr<MainForm>();
	const auto singerName = FirstSingerName(song.singerName.empty() ? info.singerName : song.singerName);

	QQMusicTaskPool().Add([weakThis, token, info, singerName]() {
		Image* cover = kugou::DownloadImage(info.imageUrl);
		Image* singerAvatar = nullptr;
		Image* singerBackground = nullptr;

		const auto singerAvatarUrl = kugou::GetSingerAvatar(singerName);
		if (!singerAvatarUrl.empty()) {
			singerAvatar = kugou::DownloadImage(singerAvatarUrl);
		}

		const auto singerBackgroundUrl = kugou::GetSingerBackground(singerName);
		if (!singerBackgroundUrl.empty()) {
			singerBackground = kugou::DownloadImage(singerBackgroundUrl);
		}

		if (!singerAvatar) {
			singerAvatar = singerBackground ? CloneImage(singerBackground) : CloneImage(cover);
		}
		if (!singerBackground) {
			singerBackground = singerAvatar ? CloneImage(singerAvatar) : CloneImage(cover);
		}

		Image* blurredBackground = CreateBlurredBackgroundImage(singerBackground);
		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			delete cover;
			delete singerAvatar;
			delete singerBackground;
			delete blurredBackground;
			return;
		}

		if (!BeginInvoke([weakThis, token, cover, singerAvatar, singerBackground, blurredBackground]() {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested() || token != weakThis->playToken_) {
				delete cover;
				delete singerAvatar;
				delete singerBackground;
				delete blurredBackground;
				return;
			}

			Image* avatarSource = singerAvatar ? singerAvatar : cover;
			Image* backgroundSource = singerBackground ? singerBackground : avatarSource;

			Image* dockImage = CloneImage(avatarSource);
			if (!dockImage) {
				dockImage = Image::Make(L"res/assets/headImg.jpg");
			}

			Image* recordImage = CloneImage(avatarSource);
			if (!recordImage) {
				recordImage = Image::Make(L"res/assets/headImg.jpg");
			}

			Image* backgroundImage = blurredBackground ? blurredBackground : CloneImage(backgroundSource);
			if (!backgroundImage) {
				backgroundImage = Image::Make(L"res/assets/headImg.jpg");
			}

			weakThis->SetControlBackImage(weakThis->dockCover_, dockImage);
			weakThis->artworkRotationAngle_ = 0.0f;
			if (weakThis->detailCover_) {
				weakThis->detailCover_->Style->Angle = weakThis->artworkRotationAngle_;
				weakThis->detailCover_->SetArtwork(recordImage);
			}
			else {
				delete recordImage;
			}
			weakThis->SetControlBackImage(weakThis->detailRoot_, backgroundImage);
			delete cover;
			delete singerAvatar;
			delete singerBackground;
			})) {
			delete cover;
			delete singerAvatar;
			delete singerBackground;
			delete blurredBackground;
		}
		});
}

void MainForm::SetControlBackImage(Control* control, Image* image, ImageSizeMode sizeMode)
{
	if (!control) {
		delete image;
		return;
	}

	if (control->Style->BackImage) {
		control->Free(control->Style->BackImage);
		control->Style->BackImage = nullptr;
	}

	if (image) {
		image->SizeMode = sizeMode;
		control->Style->BackImage = control->Attach(image);
	}

	control->Invalidate();
}

void MainForm::ToggleFavorite(const kugou::SongSummary& song)
{
	const int index = FindSongIndexByHash(favorites_, song.hash);
	if (index >= 0) {
		favorites_.erase(favorites_.begin() + index);
		DeleteSongFromIni(favoritesIni_.get(), song.hash);
		SetSidebarStatus(UIString(L"已从喜欢移除：") + song.songName);
	}
	else {
		auto stored = song;
		stored.updatedAt = CurrentUnixTimestamp();
		favorites_.insert(favorites_.begin(), stored);
		SaveSongToIni(favoritesIni_.get(), stored);
		SetSidebarStatus(UIString(L"已加入喜欢：") + stored.songName);
	}

	UpdateSidebarCounts();
	UpdateHomeSummary();
	UpdateFavoriteButtons();
	UpdateSearchRowStates();
	if (currentLibraryView_ == LibraryView::Likes && currentPage_ == ContentPage::Library) {
		PopulateLibraryList();
	}
	else {
		UpdateLibraryRowStates();
	}
}

void MainForm::PushRecent(const kugou::SongSummary& song)
{
	auto stored = song;
	stored.updatedAt = CurrentUnixTimestamp();

	const int oldIndex = FindSongIndexByHash(recentPlays_, song.hash);
	if (oldIndex >= 0) {
		recentPlays_.erase(recentPlays_.begin() + oldIndex);
	}
	recentPlays_.insert(recentPlays_.begin(), stored);
	SaveSongToIni(recentIni_.get(), stored);

	if (recentPlays_.size() > 200) {
		const auto removedHash = recentPlays_.back().hash;
		recentPlays_.resize(200);
		DeleteSongFromIni(recentIni_.get(), removedHash);
	}

	UpdateSidebarCounts();
	UpdateHomeSummary();
	if (currentLibraryView_ == LibraryView::Recent && currentPage_ == ContentPage::Library) {
		PopulateLibraryList();
	}
}

void MainForm::MarkDownloaded(const kugou::SongSummary& song, const UIString& localPath)
{
	auto stored = song;
	stored.localPath = localPath;
	stored.updatedAt = CurrentUnixTimestamp();

	const int oldIndex = FindSongIndexByHash(downloadedSongs_, song.hash);
	if (oldIndex >= 0) {
		downloadedSongs_.erase(downloadedSongs_.begin() + oldIndex);
	}
	downloadedSongs_.insert(downloadedSongs_.begin(), stored);
	SaveSongToIni(downloadsIni_.get(), stored);

	UpdateSidebarCounts();
	UpdateHomeSummary();
	if (currentLibraryView_ == LibraryView::Download && currentPage_ == ContentPage::Library) {
		PopulateLibraryList();
	}
}

void MainForm::DownloadSong(const kugou::SongSummary& song)
{
	if (song.hash.empty()) {
		return;
	}

	if (FindActiveDownloadIndex(song.hash) >= 0) {
		const auto songName = song.songName.empty() ? UIString(L"当前歌曲") : song.songName;
		SetSidebarStatus(UIString(L"正在下载：") + songName);
		UpdateDetailStatus(UIString(L"正在下载：") + songName);
		return;
	}

	const auto cachedHash = currentSongHash_;
	const auto cachedInfo = currentPlaybackInfo_;
	auto weakThis = GetWeakPtr<MainForm>();

	SetSidebarStatus(UIString(L"开始下载：") + (song.songName.empty() ? UIString(L"当前歌曲") : song.songName));
	UpdateDetailStatus(UIString(L"开始下载：") + (song.songName.empty() ? UIString(L"当前歌曲") : song.songName));

	QQMusicTaskPool().Add([weakThis, song, cachedHash, cachedInfo]() {
		kugou::SongPlaybackInfo info = cachedInfo;
		UIString error;
		bool hasPlayableInfo = cachedHash == song.hash && !cachedInfo.streamUrl.empty();

		if (!hasPlayableInfo) {
			if (!song.localPath.empty() && File::Exists(song.localPath)) {
				info.hash = song.hash;
				info.songName = song.songName;
				info.singerName = song.singerName;
				info.streamUrl = song.localPath;
				info.durationSeconds = song.durationSeconds;
				hasPlayableInfo = true;
			}
			else {
				hasPlayableInfo = kugou::GetSongPlaybackInfo(song.hash, song.mvHash, info, &error);
			}
		}

		if (!hasPlayableInfo) {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}
			BeginInvoke([weakThis, error]() {
				if (weakThis.IsAlive() && !weakThis->IsClosingRequested()) {
					weakThis->SetSidebarStatus(error.empty() ? UIString(L"下载失败。") : error);
					weakThis->UpdateDetailStatus(error.empty() ? UIString(L"下载失败。") : error);
				}
				});
			return;
		}

		auto storedSong = MergeSongSummary(song, info);
		UIString filePath = song.localPath;

		if (!filePath.empty() && File::Exists(filePath)) {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}
			BeginInvoke([weakThis, storedSong, filePath]() {
				if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
					return;
				}
				weakThis->MarkDownloaded(storedSong, filePath);
				weakThis->SetSidebarStatus(UIString(L"已加入本地下载：") + storedSong.songName);
				weakThis->UpdateDetailStatus(UIString(L"已加入本地下载：") + storedSong.songName);
				});
			return;
		}

		if (File::Exists(info.streamUrl)) {
			filePath = info.streamUrl;
		}
		else {
			const UIString downloadDir = Path::StartPath() + "\\downloads";
			if (!Directory::Exists(downloadDir)) {
				Directory::Create(downloadDir);
			}

			UIString fileName = storedSong.songName;
			if (!storedSong.singerName.empty()) {
				fileName += UIString(L" - ") + storedSong.singerName;
			}
			fileName = SanitizeFileName(fileName.empty() ? UIString(L"qqmusic_demo_track") : fileName);

			const UIString extension = GuessDownloadExtension(info.streamUrl, song.mvHash.empty() ? UIString(".mp3") : UIString(".mp4"));
			filePath = downloadDir + "\\" + fileName + extension;

			if (!File::Exists(filePath)) {
				if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
					return;
				}
				BeginInvoke([weakThis, storedSong]() {
					if (weakThis.IsAlive() && !weakThis->IsClosingRequested()) {
						weakThis->BeginDownloadTracking(storedSong);
					}
					});

				long long lastNotifyTick = 0;
				float lastProgress = -1.0f;
				const auto hash = storedSong.hash;
				const auto progressCallback = [weakThis, hash, &lastNotifyTick, &lastProgress](long long dltotal, long long dlnow) {
					if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
						return;
					}

					float progress = 0.0f;
					if (dltotal > 0) {
						progress = static_cast<float>(dlnow) / static_cast<float>(dltotal);
					}
					progress = (std::max)(0.0f, (std::min)(progress, 1.0f));

					const auto nowTick = GetTickCountMs();
					if (progress < 1.0f && std::fabs(progress - lastProgress) < 0.01f && nowTick - lastNotifyTick < 70) {
						return;
					}

					lastNotifyTick = nowTick;
					lastProgress = progress;
					BeginInvoke([weakThis, hash, progress]() {
						if (weakThis.IsAlive() && !weakThis->IsClosingRequested()) {
							weakThis->UpdateDownloadTracking(hash, progress);
						}
						});
				};

				if (!kugou::DownloadFile(info.streamUrl, filePath, &error, 120, progressCallback)) {
					if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
						return;
					}
					BeginInvoke([weakThis, error, hash = storedSong.hash]() {
						if (weakThis.IsAlive() && !weakThis->IsClosingRequested()) {
							weakThis->FinishDownloadTracking(hash);
							if (weakThis->currentPage_ == ContentPage::Library && weakThis->currentLibraryView_ == LibraryView::Download) {
								weakThis->PopulateLibraryList();
							}
							weakThis->SetSidebarStatus(error.empty() ? UIString(L"下载失败。") : error);
							weakThis->UpdateDetailStatus(error.empty() ? UIString(L"下载失败。") : error);
						}
						});
					return;
				}

				if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
					return;
				}
				BeginInvoke([weakThis, hash]() {
					if (weakThis.IsAlive() && !weakThis->IsClosingRequested()) {
						weakThis->UpdateDownloadTracking(hash, 1.0f);
					}
					});
			}
		}

		if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
			return;
		}
		BeginInvoke([weakThis, storedSong, filePath]() {
			if (!weakThis.IsAlive() || weakThis->IsClosingRequested()) {
				return;
			}

			weakThis->FinishDownloadTracking(storedSong.hash);
			weakThis->MarkDownloaded(storedSong, filePath);
			weakThis->SetSidebarStatus(UIString(L"下载完成：") + storedSong.songName);
			weakThis->UpdateDetailStatus(UIString(L"下载完成：") + storedSong.songName);
			});
		});
}

void MainForm::DownloadSongs(const std::vector<kugou::SongSummary>& songs, size_t maxCount)
{
	if (songs.empty()) {
		SetSidebarStatus(L"当前列表没有可下载歌曲。");
		return;
	}

	const auto count = (std::min)(maxCount, songs.size());
	for (size_t i = 0; i < count; ++i) {
		DownloadSong(songs[i]);
	}

	SetSidebarStatus(UIString(L"已开始批量下载前 ") + ToUiString(static_cast<int>(count)) + UIString(L" 首。"));
}

#if 0
void MainForm::ClearCurrentLibrary()
{
	switch (currentLibraryView_) {
	case LibraryView::Likes:
		favorites_.clear();
		ClearIniSections(favoritesIni_.get());
		SetSidebarStatus(L"已清空喜欢列表。");
		UpdateFavoriteButtons();
		UpdateSearchRowStates();
		break;
	case LibraryView::Recent:
		recentPlays_.clear();
		ClearIniSections(recentIni_.get());
		SetSidebarStatus(L"已清空最近播放。");
		break;
	case LibraryView::Download:
		downloadedSongs_.clear();
		ClearIniSections(downloadsIni_.get());
		SetSidebarStatus(L"已清空本地下载记录。");
		break;
	case LibraryView::Daily30:
		SetSidebarStatus(L"每日30首功能预留中。");
		return;
	}

	PopulateLibraryList();
	UpdateSidebarCounts();
	UpdateHomeSummary();
}
#endif

void MainForm::ClearCurrentLibrary()
{
	switch (currentLibraryView_) {
	case LibraryView::Likes:
		favorites_.clear();
		ClearIniSections(favoritesIni_.get());
		SetSidebarStatus(L"已清空喜欢列表。");
		UpdateFavoriteButtons();
		UpdateSearchRowStates();
		break;
	case LibraryView::Recent:
		recentPlays_.clear();
		ClearIniSections(recentIni_.get());
		SetSidebarStatus(L"已清空最近播放。");
		break;
	case LibraryView::Download:
		downloadedSongs_.clear();
		ClearIniSections(downloadsIni_.get());
		SetSidebarStatus(L"已清空本地下载记录。");
		break;
	case LibraryView::Daily30:
		dailySongs_.clear();
		ClearIniSections(daily30Ini_.get());
		ClearIniSections(daily30MetaIni_.get());
		SetSidebarStatus(L"正在刷新每日30首...");
		RefreshDailySongs(true);
		return;
	}

	PopulateLibraryList();
	UpdateSidebarCounts();
	UpdateHomeSummary();
}

void MainForm::UpdateSidebarCounts()
{
	if (navLikeText_) {
		navLikeText_->SetText(UIString(L"喜欢 ") + ToUiString(static_cast<int>(favorites_.size())));
		navLikeText_->Invalidate();
	}
	if (navRecentText_) {
		navRecentText_->SetText(UIString(L"最近播放 ") + ToUiString(static_cast<int>(recentPlays_.size())));
		navRecentText_->Invalidate();
	}
	if (navDownloadText_) {
		navDownloadText_->SetText(UIString(L"本地下载 ") + ToUiString(static_cast<int>(downloadedSongs_.size())));
		navDownloadText_->Invalidate();
	}

	UpdateSidebarSelection();
}

void MainForm::UpdateSidebarSelection()
{
	ApplyNavStyle(navLike_, currentPage_ == ContentPage::Library && currentLibraryView_ == LibraryView::Likes);
	ApplyNavStyle(navRecent_, currentPage_ == ContentPage::Library && currentLibraryView_ == LibraryView::Recent);
	ApplyNavStyle(navDownload_, currentPage_ == ContentPage::Library && currentLibraryView_ == LibraryView::Download);
	ApplyNavStyle(navDaily_, currentPage_ == ContentPage::Library && currentLibraryView_ == LibraryView::Daily30);
}

void MainForm::UpdateHomeSummary()
{
	const auto* recommendedSong = GetPrimaryRecommendation();

	if (homeLikeCount_) {
		homeLikeCount_->SetText(UIString(L"\u559c\u6b22 ") + ToUiString(static_cast<int>(favorites_.size())));
		homeLikeCount_->Invalidate();
	}
	if (homeRecentCount_) {
		homeRecentCount_->SetText(UIString(L"\u6700\u8fd1\u64ad\u653e ") + ToUiString(static_cast<int>(recentPlays_.size())));
		homeRecentCount_->Invalidate();
	}
	if (homeDownloadCount_) {
		homeDownloadCount_->SetText(UIString(L"\u672c\u5730\u4e0b\u8f7d ") + ToUiString(static_cast<int>(downloadedSongs_.size())));
		homeDownloadCount_->Invalidate();
	}

	if (isRefreshingRecommendations_ && !recommendedSong) {
		if (homeHeroTitle_) {
			homeHeroTitle_->SetText(L"\u6b63\u5728\u5237\u65b0\u63a8\u8350");
			homeHeroTitle_->Invalidate();
		}
		if (homeHeroDesc_) {
			homeHeroDesc_->SetText(L"\u6bcf\u6b21\u542f\u52a8\u90fd\u4f1a\u81ea\u52a8\u5237\u65b0 50 \u9996\u53ef\u64ad\u653e\u63a8\u8350\u3002");
			homeHeroDesc_->Invalidate();
		}
		if (homeHeroCover_) {
			homeHeroCover_->SetText(L"\u63a8\u8350");
			homeHeroCover_->Invalidate();
		}
	}
	else if (recommendedSong) {
		const UIString recommendedTitle = recommendedSong->songName.empty() ? UIString(L"\u968f\u673a\u63a8\u8350") : recommendedSong->songName;
		const UIString recommendedSinger = recommendedSong->singerName.empty() ? UIString(L"\u672a\u77e5\u6b4c\u624b") : recommendedSong->singerName;

		if (homeHeroTitle_) {
			homeHeroTitle_->SetText(recommendedTitle);
			homeHeroTitle_->Invalidate();
		}
		if (homeHeroDesc_) {
			homeHeroDesc_->SetText(
				recommendedSinger
				+ UIString(L" | \u542f\u52a8\u5df2\u5237\u65b0 ")
				+ ToUiString(static_cast<int>(recommendedSongs_.size()))
				+ UIString(L" \u9996\u63a8\u8350"));
			homeHeroDesc_->Invalidate();
		}
		if (homeHeroCover_) {
			homeHeroCover_->SetText(recommendedTitle);
			homeHeroCover_->Invalidate();
		}
	}
	else {
		if (homeHeroTitle_) {
			homeHeroTitle_->SetText(L"\u4e3a\u4f60\u63a8\u8350");
			homeHeroTitle_->Invalidate();
		}
		if (homeHeroDesc_) {
			homeHeroDesc_->SetText(L"\u6682\u65f6\u6ca1\u6709\u53ef\u64ad\u653e\u7684\u63a8\u8350\u6b4c\u66f2\u3002");
			homeHeroDesc_->Invalidate();
		}
		if (homeHeroCover_) {
			homeHeroCover_->SetText(L"\u63a8\u8350");
			homeHeroCover_->Invalidate();
		}
	}

	if (currentSongHash_.empty()) {
		if (homeCurrentMeta_) {
			if (recommendedSong) {
				homeCurrentMeta_->SetText(
					UIString(L"\u5df2\u4e3a\u4f60\u51c6\u5907 ")
					+ ToUiString(static_cast<int>(recommendedSongs_.size()))
					+ UIString(L" \u9996\u542f\u52a8\u63a8\u8350\u3002"));
			}
			else {
				homeCurrentMeta_->SetText(L"\u559c\u6b22\u3001\u6700\u8fd1\u64ad\u653e\u548c\u672c\u5730\u4e0b\u8f7d\u4f1a\u5728\u8fd9\u91cc\u6c47\u603b\u3002");
			}
			homeCurrentMeta_->Invalidate();
		}
		return;
	}

	const UIString title = currentPlaybackInfo_.songName.empty() ? UIString(L"\u5f53\u524d\u64ad\u653e") : currentPlaybackInfo_.songName;
	const UIString singer = currentPlaybackInfo_.singerName.empty() ? UIString(L"\u672a\u77e5\u6b4c\u624b") : currentPlaybackInfo_.singerName;

	if (homeCurrentMeta_) {
		homeCurrentMeta_->SetText(UIString(L"\u6b63\u5728\u64ad\u653e\uff1a") + title + UIString(L" - ") + singer);
		homeCurrentMeta_->Invalidate();
	}
}

void MainForm::UpdateFavoriteButtons()
{
	const bool liked = !currentSongHash_.empty() && IsFavorite(currentSongHash_);

	if (dockLikeBtn_) {
		dockLikeBtn_->SetText(liked ? UIString(L"♥") : UIString(L"♡"));
		dockLikeBtn_->Style->ForeColor = liked ? Color(255, 109, 119) : Color(208, 208, 208);
		dockLikeBtn_->Invalidate();
	}
	if (detailLikeBtn_) {
		detailLikeBtn_->SetText(liked ? UIString(L"♥ 已喜欢") : UIString(L"♡ 喜欢"));
		detailLikeBtn_->Invalidate();
	}
}

void MainForm::UpdateDetailStatus(const UIString& text)
{
	if (detailStatus_) {
		detailStatus_->SetText(text);
		detailStatus_->Invalidate();
	}
}

void MainForm::SetSearchMeta(const UIString& text)
{
	if (searchMeta_) {
		searchMeta_->SetText(text);
		searchMeta_->Invalidate();
	}
}

void MainForm::SetLibraryMeta(const UIString& text)
{
	if (libraryMeta_) {
		libraryMeta_->SetText(text);
		libraryMeta_->Invalidate();
	}
}

void MainForm::SetSidebarStatus(const UIString& text)
{
	if (sidebarStatus_) {
		sidebarStatus_->SetText(text);
		sidebarStatus_->Invalidate();
	}
}

const kugou::SongSummary* MainForm::GetSelectedSearchSong() const
{
	const int index = FindSongIndexByHash(searchResults_, selectedSearchHash_);
	return index >= 0 ? &searchResults_[index] : nullptr;
}

const kugou::SongSummary* MainForm::GetSelectedLibrarySong() const
{
	const int index = FindSongIndexByHash(libraryViewSongs_, selectedLibraryHash_);
	return index >= 0 ? &libraryViewSongs_[index] : nullptr;
}

int MainForm::FindSongIndexByHash(const std::vector<kugou::SongSummary>& songs, const UIString& hash) const
{
	for (int i = 0; i < static_cast<int>(songs.size()); ++i) {
		if (songs[i].hash == hash) {
			return i;
		}
	}
	return -1;
}

const kugou::SongSummary* MainForm::GetPrimaryRecommendation() const
{
	return recommendedSongs_.empty() ? nullptr : &recommendedSongs_.front();
}

int MainForm::FindActiveDownloadIndex(const UIString& hash) const
{
	for (int i = 0; i < static_cast<int>(activeDownloads_.size()); ++i) {
		if (activeDownloads_[i].song.hash == hash) {
			return i;
		}
	}
	return -1;
}

bool MainForm::IsFavorite(const UIString& hash) const
{
	return FindSongIndexByHash(favorites_, hash) >= 0;
}

float MainForm::GetActiveDownloadProgress(const UIString& hash) const
{
	const int index = FindActiveDownloadIndex(hash);
	return index >= 0 ? activeDownloads_[index].progress : -1.0f;
}

void MainForm::BeginDownloadTracking(const kugou::SongSummary& song)
{
	if (song.hash.empty()) {
		return;
	}

	DownloadTaskState state;
	state.song = song;
	state.song.updatedAt = CurrentUnixTimestamp();
	state.updatedAt = state.song.updatedAt;
	state.progress = 0.0f;

	const int existingIndex = FindActiveDownloadIndex(song.hash);
	if (existingIndex >= 0) {
		activeDownloads_.erase(activeDownloads_.begin() + existingIndex);
	}
	activeDownloads_.insert(activeDownloads_.begin(), state);

	if (currentPage_ == ContentPage::Library && currentLibraryView_ == LibraryView::Download) {
		PopulateLibraryList();
	}
}

void MainForm::UpdateDownloadTracking(const UIString& hash, float progress)
{
	const int index = FindActiveDownloadIndex(hash);
	if (index < 0) {
		return;
	}

	const float normalized = (std::max)(0.0f, (std::min)(progress, 1.0f));
	if (std::fabs(activeDownloads_[index].progress - normalized) <= 0.001f) {
		return;
	}

	activeDownloads_[index].progress = normalized;
	RefreshVisibleDownloadProgress();
}

void MainForm::FinishDownloadTracking(const UIString& hash)
{
	const int index = FindActiveDownloadIndex(hash);
	if (index >= 0) {
		activeDownloads_.erase(activeDownloads_.begin() + index);
	}
}

std::vector<kugou::SongSummary> MainForm::BuildDownloadLibrarySongs() const
{
	std::vector<kugou::SongSummary> songs;
	songs.reserve(activeDownloads_.size() + downloadedSongs_.size());

	std::unordered_set<std::wstring> activeHashes;
	activeHashes.reserve(activeDownloads_.size());

	for (const auto& task : activeDownloads_) {
		songs.push_back(task.song);
		activeHashes.insert(task.song.hash.unicode());
	}

	for (const auto& song : downloadedSongs_) {
		if (activeHashes.find(song.hash.unicode()) == activeHashes.end()) {
			songs.push_back(song);
		}
	}

	return songs;
}

void MainForm::RefreshVisibleDownloadProgress()
{
	if (currentPage_ != ContentPage::Library || currentLibraryView_ != LibraryView::Download || !librarySongList_) {
		return;
	}

	for (auto* child : librarySongList_->GetChildren()) {
		if (auto* row = dynamic_cast<SongRowItem*>(child)) {
			row->SetDownloadProgress(GetActiveDownloadProgress(row->GetSong().hash));
		}
	}
}
