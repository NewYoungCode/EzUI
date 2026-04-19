#pragma once

#include <memory>
#include <vector>

#include "EzUI/EzUI.h"

class LrcPanel : public ezui::Control {
private:
	struct LrcLine {
		int positionMs = 0;
		ezui::UIString text;
		int baseY = 0;

		LrcLine(int ms, const ezui::UIString& lineText)
			: positionMs(ms), text(lineText) {
		}
	};

	float offsetY_ = 0.0f;
	float targetOffsetY_ = 0.0f;
	float focusBlend_ = 1.0f;
	LrcLine* currentLine_ = nullptr;
	LrcLine* previousLine_ = nullptr;
	std::unique_ptr<ezui::Animation> scrollAnimation_;
	std::unique_ptr<ezui::Animation> focusAnimation_;
	std::vector<LrcLine*> lines_;
	int focusLineTop_ = 0;
	int contentHeight_ = 0;
	int topPadding_ = 24;
	int bottomPadding_ = 48;
	int verticalMargin_ = 18;
	int lineHeight_ = 56;

protected:
	void OnBackgroundPaint(ezui::PaintEventArgs* arg) override;
	void OnSize(ezui::SizeEventArgs* args) override;

public:
	explicit LrcPanel(ezui::Object* owner = nullptr);
	~LrcPanel() override;

	void ChangePostion(int positionMs);
	void LoadLrc(const ezui::UIString& lrcData);
	void ClearLrc();

private:
	void ReflowLines(bool keepCurrentLine = false);
	void SyncScrollTarget(bool snapToTarget = false);
	float CalcTargetOffset(LrcLine* line) const;
	void StartFocusAnimation();
	void StopAnimations();
};
