#pragma once

#include <functional>

#include "EzUI/EzUI.h"

#include "KugouApi.h"

class SongListMessageItem : public ezui::Label
{
public:
	explicit SongListMessageItem(const ezui::UIString& message);

private:
	ezui::UIString message_;
};

class SongRowItem : public ezui::Frame
{
public:
	explicit SongRowItem(const kugou::SongSummary& song);
	~SongRowItem() override = default;

	const kugou::SongSummary& GetSong() const { return song_; }

	void SetPlaying(bool playing);
	void SetSelected(bool selected);
	void SetLiked(bool liked);
	void SetDownloadProgress(float progress);

	std::function<void(SongRowItem*)> Clicked;
	std::function<void(SongRowItem*)> DoubleClicked;
	std::function<void(SongRowItem*)> SingerClicked;
	std::function<void(SongRowItem*)> FavoriteClicked;

protected:
	void OnBackgroundPaint(ezui::PaintEventArgs* args) override;
	void OnMouseDown(ezui::MouseEventArgs* args) override;
	void OnMouseDoubleClick(ezui::MouseEventArgs* args) override;
	ezui::PropertyValue<HCURSOR> OnQueryCursor(const ezui::Point& point) override;
private:
	void ApplyState();
	bool IsHeartHot(const ezui::Point& pt) const;
	bool IsSingerHot(const ezui::Point& pt) const;
	ezui::UIString BuildCoverText() const;

	kugou::SongSummary song_;
	ezui::Label* titleLabel_ = nullptr;
	ezui::Label* singerLabel_ = nullptr;
	ezui::Control* singerHitArea_ = nullptr;
	ezui::Label* heartLabel_ = nullptr;
	bool playing_ = false;
	bool selected_ = false;
	bool liked_ = false;
	float downloadProgress_ = -1.0f;
};
