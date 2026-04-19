#include "MusicSeekBar.h"

#include <algorithm>

using namespace ezui;

namespace {

float TrackHeight(float scale)
{
    return 3.0f * scale;
}

float ThumbDiameter(float scale, VisualState state)
{
    return (state == VisualState::Active ? 11.0f : 9.0f) * scale;
}

} // namespace

MusicSeekBar::MusicSeekBar(Object* owner) : Slider(owner)
{
    Style->BackColor = Color(255, 255, 255, 72);
    Style->ForeColor = Color(25, 223, 127);
}

void MusicSeekBar::OnTrackPaint(PaintEventArgs* args)
{
    const auto trackRect = GetTrackRect();
    const float radius = trackRect.Height / 2.0f;
    const float activeRatio = GetRatio();
    const float activeWidth = (std::max)(trackRect.Height, trackRect.Width * activeRatio);

    args->Graphics()->SetColor(GetBackColor());
    args->Graphics()->FillRectangle(trackRect, radius);

    RectF activeRect(trackRect.X, trackRect.Y, (std::min)(trackRect.Width, activeWidth), trackRect.Height);
    args->Graphics()->SetColor(GetForeColor());
    args->Graphics()->FillRectangle(activeRect, radius);
}

void MusicSeekBar::OnThumbPaint(PaintEventArgs* args)
{
    const auto thumbRect = GetThumbRect();
    args->Graphics()->SetColor(Color(255, 255, 255, 58));
    args->Graphics()->FillEllipse(RectF(thumbRect.X - (1.5f * GetScale()), thumbRect.Y - (1.5f * GetScale()),
        thumbRect.Width + (3.0f * GetScale()), thumbRect.Height + (3.0f * GetScale())));

    args->Graphics()->SetColor(Color(255, 255, 255));
    args->Graphics()->FillEllipse(thumbRect);
    args->Graphics()->SetColor(Color(0, 0, 0, 32));
    args->Graphics()->DrawEllipse(thumbRect, 1.0f * GetScale());
}

SizeF MusicSeekBar::GetThumbSize()
{
    const float diameter = ThumbDiameter(GetScale(), GetVisualState());
    return SizeF(diameter, diameter);
}

RectF MusicSeekBar::GetTrackRect()
{
    const auto thumbSize = GetThumbSize();
    const float trackHeight = TrackHeight(GetScale());
    const float padding = thumbSize.Width / 2.0f;
    return RectF(padding, (Height() - trackHeight) / 2.0f, (std::max)(0.0f, Width() - thumbSize.Width), trackHeight);
}

RectF MusicSeekBar::GetThumbRect()
{
    const auto thumbSize = GetThumbSize();
    const auto trackRect = GetTrackRect();
    const float ratio = GetRatio();
    const float centerX = trackRect.X + trackRect.Width * ratio;
    const float x = centerX - thumbSize.Width / 2.0f;
    const float y = (Height() - thumbSize.Height) / 2.0f;
    return RectF(x, y, thumbSize.Width, thumbSize.Height);
}
