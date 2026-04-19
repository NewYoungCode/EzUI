#include "RecordCover.h"

#include <algorithm>

using namespace ezui;

namespace {

RectF DeflateRect(const RectF& rect, float amount)
{
    return RectF(
        rect.X + amount,
        rect.Y + amount,
        (std::max)(0.0f, rect.Width - amount * 2.0f),
        (std::max)(0.0f, rect.Height - amount * 2.0f));
}

}

RecordCover::RecordCover(Object* owner) : Control(owner)
{
}

void RecordCover::SetArtwork(Image* image)
{
    if (artwork_) {
        Free(artwork_);
        artwork_ = nullptr;
    }

    if (image) {
        image->SizeMode = ImageSizeMode::Cover;
        artwork_ = Attach(image);
    }

    Invalidate();
}

void RecordCover::OnPaint(PaintEventArgs* args)
{
    const RectF bounds(0.0f, 0.0f, static_cast<float>(Width()), static_cast<float>(Height()));
    if (bounds.Width <= 0.0f || bounds.Height <= 0.0f) {
        return;
    }

    auto* graphics = args->Graphics();
    const float scale = GetScale();
    const float imageInset = 18.0f * scale;
    const float grooveInset = 12.0f * scale;
    const float centerCap = 56.0f * scale;
    const float centerHole = 12.0f * scale;

    graphics->SetColor(Color(16, 16, 18, 235));
    graphics->FillEllipse(bounds);

    graphics->SetColor(Color(255, 255, 255, 34));
    graphics->DrawEllipse(bounds, 2.0f * scale);

    for (int i = 0; i < 3; ++i) {
        const float inset = grooveInset + i * (10.0f * scale);
        const RectF grooveRect = DeflateRect(bounds, inset);
        graphics->SetColor(Color(255, 255, 255, 18));
        graphics->DrawEllipse(grooveRect, 1.0f * scale);
    }

    const RectF imageRect = DeflateRect(bounds, imageInset);
    EllipseGeometry clip(imageRect);
    args->PushLayer(clip, 1.0f);
    if (artwork_) {
        graphics->DrawImage(artwork_, imageRect);
    }
    else {
        graphics->SetColor(Color(52, 52, 56, 220));
        graphics->FillEllipse(imageRect);
    }
    args->PopLayer();

    graphics->SetColor(Color(255, 255, 255, 26));
    graphics->DrawEllipse(imageRect, 1.2f * scale);

    const RectF centerRect(
        bounds.X + bounds.Width / 2.0f - centerCap / 2.0f,
        bounds.Y + bounds.Height / 2.0f - centerCap / 2.0f,
        centerCap,
        centerCap);
    graphics->SetColor(Color(245, 245, 245, 228));
    graphics->FillEllipse(centerRect);
    graphics->SetColor(Color(0, 0, 0, 42));
    graphics->DrawEllipse(centerRect, 1.0f * scale);

    const RectF holeRect(
        bounds.X + bounds.Width / 2.0f - centerHole / 2.0f,
        bounds.Y + bounds.Height / 2.0f - centerHole / 2.0f,
        centerHole,
        centerHole);
    graphics->SetColor(Color(34, 34, 36, 240));
    graphics->FillEllipse(holeRect);
}
