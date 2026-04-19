#include "SongRowItem.h"

#include <algorithm>
#include <cmath>

using namespace ezui;

namespace {

	float ClampProgress(float value)
	{
		return (std::max)(0.0f, (std::min)(value, 1.0f));
	}

	uint8_t BlendChannel(uint8_t from, uint8_t to, float progress)
	{
		return static_cast<uint8_t>(from + (to - from) * ClampProgress(progress) + 0.5f);
	}

	Color BlendColor(const Color& from, const Color& to, float progress)
	{
		progress = ClampProgress(progress);
	return Color(
		BlendChannel(from.GetR(), to.GetR(), progress),
		BlendChannel(from.GetG(), to.GetG(), progress),
		BlendChannel(from.GetB(), to.GetB(), progress),
		BlendChannel(from.GetA(), to.GetA(), progress));
}

	Color PickCoverColor(const UIString& hash)
	{
		static const Color colors[] = {
			Color(243, 171, 58),
			Color(123, 182, 223),
			Color(225, 102, 134),
			Color(118, 164, 131),
			Color(146, 129, 207),
			Color(107, 126, 151),
		};

		unsigned int seed = 0;
		for (char ch : hash) {
			seed = seed * 131 + static_cast<unsigned char>(ch);
		}

		return colors[seed % (sizeof(colors) / sizeof(colors[0]))];
	}

	Label* MakeBadge(const UIString& text, const Color& backColor, const Color& foreColor, Object* ownerObject)
	{
		auto* badge = new Label(ownerObject);
		badge->SetFixedHeight(16);
		badge->SetAutoWidth(true);
		badge->SetText(text);
		badge->Style->BackColor = backColor;
		badge->Style->ForeColor = foreColor;
		badge->Style->Border.Radius = 8;
		badge->Style->FontSize = 10;
		badge->SetPadding(0, 7, 0, 7);
		badge->SetMargin(0, 0, 0, 6);
		badge->SetAttribute("event", "none");
		return badge;
	}

	int CalculateBadgeWidth(const kugou::SongSummary& song)
	{
		int width = 0;
		if (!song.mvHash.empty()) {
			width += 38;
		}
		if (!song.localPath.empty()) {
			width += 50;
		}
		if (song.qualityLevel >= 2) {
			width += 38;
		}
		return width;
	}

} // namespace

SongRowItem::SongRowItem(const kugou::SongSummary& song) : song_(song)
{
	SetFixedHeight(56);
	SetAttribute("SongHash", song.hash);
	Style->Border.Radius = 10;
	HoverStyle->BackColor = Color(37, 37, 37);
	ActiveStyle->BackColor = Color(38, 38, 38);
	Style->Cursor = LoadCursor(Cursor::Pointer);
	SetMarginTop(2);

	auto* root = new HBox(this);
	root->SetAttribute("event", "none");

	auto* cover = new Label(root);
	cover->SetFixedSize({ 38, 38 });
	cover->SetText(BuildCoverText());
	cover->SetTextAlign(TextAlign::MiddleCenter);
	cover->Style->BackColor = PickCoverColor(song.hash);
	cover->Style->ForeColor = Color::White;
	cover->Style->Border.Radius = 8;
	cover->Style->FontSize = 11;
	cover->Style->FontWeight = 700;
	cover->SetAttribute("event", "none");

	auto* textBox = new VBox(root);
	textBox->SetItemsAlign(Align::MiddleLeft);
	textBox->SetAttribute("event", "none");

	titleLabel_ = new Label(textBox);
	titleLabel_->SetFixedHeight(20);
	titleLabel_->SetText(song.songName.empty() ? UIString(L"未命名歌曲") : song.songName);
	titleLabel_->SetTextAlign(TextAlign::MiddleLeft);
	titleLabel_->SetElidedText("...");
	titleLabel_->Style->ForeColor = Color(242, 242, 242);
	titleLabel_->Style->FontSize = 14;
	titleLabel_->SetAttribute("event", "none");

	auto* singerRow = new HBox(textBox);
	singerRow->SetAutoWidth(true);
	singerRow->SetFixedHeight(18);
	singerRow->SetAttribute("event", "none");

	singerLabel_ = new Label(singerRow);
	singerLabel_->SetAutoWidth(true);
	singerLabel_->SetFixedHeight(18);
	singerLabel_->SetText(song.singerName.empty() ? UIString(L"未知歌手") : song.singerName);
	singerLabel_->SetTextAlign(TextAlign::MiddleLeft);
	singerLabel_->SetElidedText("...");
	singerLabel_->Style->ForeColor = Color(171, 171, 171);
	singerLabel_->Style->FontSize = 12;
	singerLabel_->SetAttribute("event", "none");

	singerRow->AddChild(singerLabel_);
	singerHitArea_ = singerRow;

	textBox->AddChild(titleLabel_);
	textBox->AddChild(singerRow);

	auto* badgeBox = new HBox(root);
	badgeBox->SetFixedHeight(18);
	badgeBox->SetFixedWidth(CalculateBadgeWidth(song));
	badgeBox->SetAttribute("event", "none");

	if (!song.mvHash.empty()) {
		badgeBox->AddChild(MakeBadge("MV", Color(60, 60, 60), Color(217, 217, 217), badgeBox));
	}
	if (!song.localPath.empty()) {
		badgeBox->AddChild(MakeBadge("本地", Color(35, 75, 61), Color(116, 245, 178), badgeBox));
	}
	if (song.qualityLevel >= 2) {
		badgeBox->AddChild(MakeBadge(song.qualityLevel >= 3 ? "SQ" : "HQ", Color(100, 82, 38), Color(242, 203, 99), badgeBox));
	}

	heartLabel_ = new Label(root);
	heartLabel_->SetFixedWidth(42);
	heartLabel_->SetTextAlign(TextAlign::MiddleCenter);
	heartLabel_->Style->FontSize = 16;
	heartLabel_->Style->Cursor = LoadCursor(Cursor::Pointer);
	heartLabel_->SetAttribute("event", "none");

	auto* albumName = new Label(root);
	albumName->SetFixedWidth(210);
	albumName->SetText(song.albumName.empty() ? UIString(L"未知专辑") : song.albumName);
	albumName->SetTextAlign(TextAlign::MiddleLeft);
	albumName->SetElidedText("...");
	albumName->Style->ForeColor = Color(171, 171, 171);
	albumName->Style->FontSize = 12;
	albumName->SetAttribute("event", "none");

	auto* duration = new Label(root);
	duration->SetFixedWidth(66);
	duration->SetText(kugou::ToTimeString(song.durationSeconds));
	duration->SetTextAlign(TextAlign::MiddleCenter);
	duration->Style->ForeColor = Color(171, 171, 171);
	duration->Style->FontSize = 12;
	duration->SetAttribute("event", "none");

	root->AddChild(new HSpacer(10));
	root->AddChild(cover);
	root->AddChild(new HSpacer(10));
	root->AddChild(textBox);
	if (CalculateBadgeWidth(song) > 0) {
		root->AddChild(new HSpacer(10));
		root->AddChild(badgeBox);
	}
	root->AddChild(heartLabel_);
	root->AddChild(albumName);
	root->AddChild(duration);
	root->AddChild(new HSpacer(10));

	SetLayout(root);
	ApplyState();
}

void SongRowItem::SetPlaying(bool playing)
{
	playing_ = playing;
	ApplyState();
}

void SongRowItem::SetSelected(bool selected)
{
	selected_ = selected;
	ApplyState();
}

void SongRowItem::SetLiked(bool liked)
{
	liked_ = liked;
	ApplyState();
}

void SongRowItem::SetDownloadProgress(float progress)
{
	const float normalized = progress < 0.0f ? -1.0f : ClampProgress(progress);
	if (std::fabs(downloadProgress_ - normalized) <= 0.001f) {
		return;
	}

	downloadProgress_ = normalized;
	Invalidate();
}

void SongRowItem::OnBackgroundPaint(PaintEventArgs* args)
{
	__super::OnBackgroundPaint(args);

	if (!args || downloadProgress_ < 0.0f || Width() <= 0 || Height() <= 0) {
		return;
	}

	Color fillColor = GetBackColor();
	if (fillColor.GetValue() == 0) {
		fillColor = Color(43, 43, 43);
	}

	const uint8_t alpha = fillColor.GetA() > 0 ? fillColor.GetA() : 255;
	Color baseColor = BlendColor(fillColor, Color(0, 0, 0, alpha), 0.18f);
	args->Graphics()->SetColor(baseColor);
	args->Graphics()->FillRectangle(RectF(0.0f, 0.0f, static_cast<float>(Width()), static_cast<float>(Height())), 10.0f);

	const float progressWidth = static_cast<float>(Width()) * ClampProgress(downloadProgress_);
	if (progressWidth > 0.5f) {
		args->Graphics()->SetColor(fillColor);
		args->Graphics()->FillRectangle(RectF(0.0f, 0.0f, progressWidth, static_cast<float>(Height())));
	}
}

void SongRowItem::OnMouseDown(MouseEventArgs* args)
{
	__super::OnMouseDown(args);
	if (!args || args->Button() != MouseButton::Left) {
		return;
	}

	if (IsHeartHot(args->Position())) {
		if (FavoriteClicked) {
			FavoriteClicked(this);
		}
		return;
	}

	if (IsSingerHot(args->Position())) {
		if (SingerClicked) {
			SingerClicked(this);
		}
		return;
	}

	if (Clicked) {
		Clicked(this);
	}
}

void SongRowItem::OnMouseDoubleClick(MouseEventArgs* args)
{
	__super::OnMouseDoubleClick(args);
	if (!args || args->Button() != MouseButton::Left) {
		return;
	}

	if (IsHeartHot(args->Position()) || IsSingerHot(args->Position())) {
		return;
	}

	if (DoubleClicked) {
		DoubleClicked(this);
	}
}

PropertyValue<HCURSOR> SongRowItem::OnQueryCursor(const Point& point)
{
	if (IsHeartHot(point)) {
		return heartLabel_ ? heartLabel_->Style->Cursor : LoadCursor(Cursor::Pointer);
	}

	if (IsSingerHot(point)) {
		return LoadCursor(Cursor::Pointer);
	}

	return PropertyValue<HCURSOR>();
}

void SongRowItem::ApplyState()
{
	if (playing_) {
		Style->BackColor = Color(33, 48, 41);
	}
	else if (selected_) {
		Style->BackColor = Color(41, 41, 41);
	}
	else {
		Style->BackColor = Color::Transparent;
	}

	if (titleLabel_) {
		titleLabel_->Style->ForeColor = playing_ ? Color(25, 223, 127) : Color(242, 242, 242);
		titleLabel_->Invalidate();
	}
	if (singerLabel_) {
		singerLabel_->Style->ForeColor = selected_ ? Color(198, 198, 198) : Color(171, 171, 171);
		singerLabel_->Invalidate();
	}
	if (heartLabel_) {
		heartLabel_->SetText(liked_ ? UIString(L"♥") : UIString(L"♡"));
		heartLabel_->Style->ForeColor = liked_ ? Color(255, 109, 119) : Color(110, 110, 110);
		heartLabel_->Invalidate();
	}

	Invalidate();
}

bool SongRowItem::IsHeartHot(const Point& pt) const
{
	return heartLabel_ && heartLabel_->GetRectInFrame().Contains(pt);
}

bool SongRowItem::IsSingerHot(const Point& pt) const
{
	return singerHitArea_ && !song_.singerName.empty() && singerHitArea_->GetRectInFrame().Contains(pt);
}

UIString SongRowItem::BuildCoverText() const
{
	if (song_.songName.empty()) {
		return L"♪";
	}

	const auto unicode = song_.songName.unicode();
	if (unicode.size() >= 2) {
		return UIString(unicode.substr(0, 2));
	}
	return song_.songName.substr(0, 1);
}
