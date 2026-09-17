#include "SystemCheckBox.h"

namespace {
    int ScaleValue(float scale, int value)
    {
        return (int)EZUI_ROUND(value * scale);
    }
}

SystemCheckBox::SystemCheckBox()
{
    this->SetPaddingLeft(22);
}

void SystemCheckBox::OnForePaint(ezui::PaintEventArgs* args)
{
    if (!args || !args->Graphics()) {
        return;
    }

    float scale = GetScale();
    int boxSize = ScaleValue(scale, 14);
    __super::OnForePaint(args);

    ezui::Distance padding = this->GetPadding();
    int contentHeight = Height() - padding.Top - padding.Bottom;
    if (contentHeight <= 0 || Width() <= 0 || Height() <= 0) {
        return;
    }

    float x = 2.0f;
    float y = padding.Top + (contentHeight - boxSize) * 0.5f;
    if (y < 0.0f) {
        y = 0.0f;
    }

    ezui::RectF boxRect(x + 0.5f, y + 0.5f, (float)boxSize - 1.0f, (float)boxSize - 1.0f);
    bool checked = GetCheck();
    bool hover = (GetVisualState() & ezui::VisualState::Hover) == ezui::VisualState::Hover;
    bool disabled = (GetVisualState() & ezui::VisualState::Disabled) == ezui::VisualState::Disabled;
    ezui::Color accent = GetForeColor(ezui::VisualState::Checked);
    if (accent.GetValue() == 0) {
        accent = GetForeColor();
    }
    if (accent.GetValue() == 0) {
        accent = ezui::Color(22, 124, 128);
    }

    ezui::Color borderColor = disabled ? ezui::Color(176, 184, 180) : (checked ? accent : (hover ? ezui::Color(96, 139, 128) : ezui::Color(145, 160, 154)));
    ezui::Color fillColor = disabled ? ezui::Color(236, 240, 238) : (checked ? accent : ezui::Color(255, 255, 255));
    float radius = EZUI_MAX(2.0f, 3.0f * scale);

    args->Graphics()->SetColor(fillColor);
    args->Graphics()->FillRectangle(boxRect, radius);
    args->Graphics()->SetColor(borderColor);
    args->Graphics()->DrawRectangle(boxRect, radius, EZUI_MAX(1.0f, scale));

    if (checked) {
        float left = boxRect.X + boxRect.Width * 0.26f;
        float midX = boxRect.X + boxRect.Width * 0.44f;
        float right = boxRect.X + boxRect.Width * 0.76f;
        float midY = boxRect.Y + boxRect.Height * 0.62f;
        float bottom = boxRect.Y + boxRect.Height * 0.76f;
        float top = boxRect.Y + boxRect.Height * 0.30f;
        args->Graphics()->SetColor(disabled ? ezui::Color(155, 164, 160) : ezui::Color(255, 255, 255));
        args->Graphics()->DrawLine(ezui::PointF(left, midY), ezui::PointF(midX, bottom), EZUI_MAX(1.8f, 2.0f * scale));
        args->Graphics()->DrawLine(ezui::PointF(midX, bottom), ezui::PointF(right, top), EZUI_MAX(1.8f, 2.0f * scale));
    }
}
