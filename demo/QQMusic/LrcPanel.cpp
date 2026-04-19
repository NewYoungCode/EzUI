#include "LrcPanel.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

using namespace ezui;

namespace {

float ClampFloat(float value, float minimum, float maximum)
{
	return (std::max)(minimum, (std::min)(value, maximum));
}

int ClampInt(int value, int minimum, int maximum)
{
	return (std::max)(minimum, (std::min)(value, maximum));
}

uint8_t BlendChannel(uint8_t from, uint8_t to, float progress)
{
	progress = ClampFloat(progress, 0.0f, 1.0f);
	return static_cast<uint8_t>(from + (to - from) * progress + 0.5f);
}

Color BlendColor(const Color& from, const Color& to, float progress)
{
	progress = ClampFloat(progress, 0.0f, 1.0f);
	return Color(
		BlendChannel(from.GetR(), to.GetR(), progress),
		BlendChannel(from.GetG(), to.GetG(), progress),
		BlendChannel(from.GetB(), to.GetB(), progress),
		BlendChannel(from.GetA(), to.GetA(), progress));
}

}

LrcPanel::LrcPanel(Object* owner) : Control(owner)
{
	scrollAnimation_ = std::make_unique<Animation>();
	scrollAnimation_->SetEasingCurve(EasingCurve::Damping);
	scrollAnimation_->ValueChanged = [this](float value) {
		offsetY_ = value;
		Invalidate();
	};

	focusAnimation_ = std::make_unique<Animation>();
	focusAnimation_->SetEasingCurve(EasingCurve::EaseOut);
	focusAnimation_->ValueChanged = [this](float value) {
		focusBlend_ = value;
		if (focusBlend_ >= 0.999f) {
			focusBlend_ = 1.0f;
			previousLine_ = nullptr;
		}
		Invalidate();
	};
}

LrcPanel::~LrcPanel()
{
	StopAnimations();
	for (auto* item : lines_) {
		delete item;
	}
}

void LrcPanel::ChangePostion(int positionMs)
{
	if (lines_.empty() || GetRect().IsEmptyArea()) {
		return;
	}

	LrcLine* nextLine = nullptr;
	for (auto it = lines_.rbegin(); it != lines_.rend(); ++it) {
		if ((*it)->positionMs <= positionMs) {
			nextLine = *it;
			break;
		}
	}

	if (!nextLine) {
		nextLine = lines_.front();
	}

	if (!currentLine_) {
		currentLine_ = nextLine;
		focusBlend_ = 1.0f;
		previousLine_ = nullptr;
		SyncScrollTarget(true);
		Invalidate();
		return;
	}

	if (nextLine == currentLine_) {
		return;
	}

	previousLine_ = currentLine_;
	currentLine_ = nextLine;
	StartFocusAnimation();
	SyncScrollTarget();
}

void LrcPanel::OnBackgroundPaint(PaintEventArgs* arg)
{
	__super::OnBackgroundPaint(arg);

	if (!arg || lines_.empty() || Width() <= 0 || Height() <= 0) {
		return;
	}

	const Rect viewport(0, 0, Width(), Height());
	const float scale = GetScale();
	const float baseFont = 15.0f * scale;
	const float activeFontBoost = 7.0f * scale;
	const Color baseColor = GetForeColor();
	const Color activeColor(25, 223, 127, 255);

	for (auto* item : lines_) {
		float emphasis = 0.0f;
		if (item == currentLine_) {
			emphasis = focusBlend_;
		}
		else if (item == previousLine_) {
			emphasis = 1.0f - focusBlend_;
		}

		const float fontSize = baseFont + activeFontBoost * emphasis;
		const int extraHeight = static_cast<int>(std::round(activeFontBoost * emphasis * 2.2f));
		const int drawY = static_cast<int>(std::round(item->baseY - offsetY_ - extraHeight * 0.5f));
		Rect drawRect(0, drawY, Width(), lineHeight_ + extraHeight);
		if (!drawRect.IntersectsWith(viewport)) {
			continue;
		}

		arg->Graphics()->SetColor(BlendColor(baseColor, activeColor, emphasis));
		arg->Graphics()->SetFont(GetFontFamily(), fontSize);
		arg->Graphics()->DrawString(item->text.unicode(), drawRect, TextAlign::MiddleLeft);
	}
}

void LrcPanel::OnSize(SizeEventArgs* args)
{
	__super::OnSize(args);
	ReflowLines(true);
}

void LrcPanel::LoadLrc(const UIString& lrcData)
{
	ClearLrc();

	auto rows = lrcData.split("\n");
	for (auto& row : rows) {
		if (row.empty()) {
			continue;
		}

		const int pos1 = static_cast<int>(row.find("["));
		const int pos2 = static_cast<int>(row.find("]"));
		if (pos1 != 0 || pos2 != 9) {
			continue;
		}

		const UIString text = UIString(row.substr(pos2 + 1)).trim();
		const int minute = std::atoi(row.substr(1, 2).c_str());
		const float second = std::atof(row.substr(4, 5).c_str());
		const int positionMs = static_cast<int>(minute * 60 * 1000 + second * 1000);

		if (text.empty()) {
			continue;
		}

		lines_.push_back(new LrcLine(positionMs, text));
	}

	if (!lines_.empty()) {
		currentLine_ = lines_.front();
		focusBlend_ = 1.0f;
		previousLine_ = nullptr;
		ReflowLines();
	}
	Invalidate();
}

void LrcPanel::ClearLrc()
{
	StopAnimations();

	currentLine_ = nullptr;
	previousLine_ = nullptr;
	for (auto* item : lines_) {
		delete item;
	}
	lines_.clear();
	offsetY_ = 0.0f;
	targetOffsetY_ = 0.0f;
	focusBlend_ = 1.0f;
	focusLineTop_ = 0;
	contentHeight_ = 0;
	Invalidate();
}

void LrcPanel::ReflowLines(bool keepCurrentLine)
{
	if (lines_.empty()) {
		currentLine_ = nullptr;
		previousLine_ = nullptr;
		focusBlend_ = 1.0f;
		focusLineTop_ = 0;
		contentHeight_ = 0;
		offsetY_ = 0.0f;
		targetOffsetY_ = 0.0f;
		return;
	}

	LrcLine* activeLine = keepCurrentLine ? currentLine_ : nullptr;
	const float scale = GetScale();
	verticalMargin_ = (std::max)(16, static_cast<int>(std::round(18.0f * scale)));
	lineHeight_ = (std::max)(52, static_cast<int>(std::round(56.0f * scale)));
	topPadding_ = (std::max)(lineHeight_ * 2, Height() / 4);
	bottomPadding_ = (std::max)(lineHeight_ * 2, Height() / 3);

	int nextY = topPadding_;
	for (auto* item : lines_) {
		item->baseY = nextY;
		nextY += lineHeight_ + verticalMargin_;
	}

	contentHeight_ = nextY - verticalMargin_ + bottomPadding_;
	currentLine_ = activeLine ? activeLine : lines_.front();
	previousLine_ = nullptr;
	focusBlend_ = 1.0f;
	SyncScrollTarget(true);
}

void LrcPanel::SyncScrollTarget(bool snapToTarget)
{
	if (Height() <= 0) {
		offsetY_ = 0.0f;
		targetOffsetY_ = 0.0f;
		return;
	}

	focusLineTop_ = (std::max)(topPadding_, Height() / 2 - lineHeight_ / 2);
	targetOffsetY_ = CalcTargetOffset(currentLine_);

	const float maxOffset = static_cast<float>((std::max)(0, contentHeight_ - Height()));
	offsetY_ = ClampFloat(offsetY_, 0.0f, maxOffset);
	targetOffsetY_ = ClampFloat(targetOffsetY_, 0.0f, maxOffset);

	if (snapToTarget || std::fabs(targetOffsetY_ - offsetY_) <= 0.5f) {
		if (scrollAnimation_) {
			scrollAnimation_->Stop();
		}
		offsetY_ = targetOffsetY_;
		Invalidate();
		return;
	}

	if (scrollAnimation_) {
		scrollAnimation_->Stop();
		scrollAnimation_->SetStartValue(offsetY_);
		scrollAnimation_->SetEndValue(targetOffsetY_);
		const int duration = ClampInt(static_cast<int>(std::fabs(targetOffsetY_ - offsetY_) * 0.42f), 160, 340);
		scrollAnimation_->Start(duration);
	}
}

float LrcPanel::CalcTargetOffset(LrcLine* line) const
{
	if (!line) {
		return 0.0f;
	}

	const float maxOffset = static_cast<float>((std::max)(0, contentHeight_ - Height()));
	return ClampFloat(static_cast<float>(line->baseY - focusLineTop_), 0.0f, maxOffset);
}

void LrcPanel::StartFocusAnimation()
{
	if (!currentLine_ || !previousLine_ || currentLine_ == previousLine_) {
		previousLine_ = nullptr;
		focusBlend_ = 1.0f;
		if (focusAnimation_) {
			focusAnimation_->Stop();
		}
		Invalidate();
		return;
	}

	focusBlend_ = 0.0f;
	if (focusAnimation_) {
		focusAnimation_->Stop();
		focusAnimation_->SetStartValue(0.0f);
		focusAnimation_->SetEndValue(1.0f);
		focusAnimation_->Start(220);
	}
}

void LrcPanel::StopAnimations()
{
	if (scrollAnimation_) {
		scrollAnimation_->Stop();
	}
	if (focusAnimation_) {
		focusAnimation_->Stop();
	}
}
