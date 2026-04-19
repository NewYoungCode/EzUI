#pragma once

#include "EzUI/EzUI.h"

class MusicSeekBar : public ezui::Slider {
public:
    explicit MusicSeekBar(ezui::Object* owner = nullptr);
    ~MusicSeekBar() override = default;

protected:
    void OnTrackPaint(ezui::PaintEventArgs* args) override;
    void OnThumbPaint(ezui::PaintEventArgs* args) override;
    ezui::SizeF GetThumbSize() override;
    ezui::RectF GetTrackRect() override;
    ezui::RectF GetThumbRect() override;
};
