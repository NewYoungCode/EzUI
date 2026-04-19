#pragma once

#include "EzUI/EzUI.h"

class RecordCover : public ezui::Control {
public:
    explicit RecordCover(ezui::Object* owner = nullptr);
    ~RecordCover() override = default;

    void SetArtwork(ezui::Image* image);

protected:
    void OnPaint(ezui::PaintEventArgs* args) override;

private:
    ezui::Image* artwork_ = nullptr;
};
