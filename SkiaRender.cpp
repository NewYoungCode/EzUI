#include "SkiaRender.h"
#include "UIDef.h"
#if USED_Skia
#if NDEBUG
#pragma comment(lib,"skia/skia_MD.lib")
#else
#pragma comment(lib,"skia/skia_MDd.lib")
#endif
#pragma comment(lib,"Opengl32.lib")
namespace EzUI {

	void RenderInitialize() {
	}
	void RenderUnInitialize() {
	}

	SkCanvas* CreateRender(void* pixels, int width, int height) {
		SkBitmap _skBitmap;
		_skBitmap.setInfo(SkImageInfo::Make(width, height, kN32_SkColorType, kPremul_SkAlphaType));
		_skBitmap.setPixels(pixels);
		SkCanvas* skc = new SkCanvas(_skBitmap);
		return skc;
	}
	void ReleaseRender(SkCanvas* canvas) {
		delete canvas;
	}
	void FillRectangle(SkCanvas* canvas, const __Rect& _rect, const __Color& color, int _radius) {
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(SkColorSetARGB(color.GetA(), color.GetR(), color.GetG(), color.GetB()));
		paint.setStyle(SkPaint::kFill_Style);
		SkRect rect = SkRect::MakeXYWH(_rect.X, _rect.Y, _rect.Width, _rect.Height);

		if (_radius > 0) {
			canvas->drawRRect(SkRRect::MakeRectXY(rect, _radius, _radius), paint);
		}
		else {
			canvas->drawRect(rect, paint);
		}
	}
	void DrawRectangle(SkCanvas* canvas, const  __Rect& _rect, const  __Color& color, int width, int _radius) {
		// Create a paint object and set its color to blue
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(SkColorSetARGB(color.GetA(), color.GetR(), color.GetG(), color.GetB()));
		paint.setStyle(SkPaint::kStroke_Style);
		SkRect rect = SkRect::MakeXYWH(_rect.X, _rect.Y, _rect.Width, _rect.Height);
		if (_radius > 0) {
			canvas->drawRRect(SkRRect::MakeRectXY(rect, _radius, _radius), paint);
		}
		else {
			canvas->drawRect(rect, paint);
		}
	}
	void SetTransform(SkCanvas* canvas, int xOffset, int yOffset) {
		/*	auto mtx = canvas->getTotalMatrix();
			auto x = (int)mtx.getTranslateX();
			auto y = (int)mtx.getTranslateY();*/
		canvas->resetMatrix();
		canvas->translate((SkScalar)xOffset, (SkScalar)yOffset);
	}
	void DrawLine(SkCanvas* canvas, const __Color& color, const __Point& _A, const __Point& _B, int width) {
		SkPaint paint;
		paint.setAntiAlias(true);
		paint.setColor(SkColorSetARGB(color.GetA(), color.GetR(), color.GetG(), color.GetB()));
		paint.setStyle(SkPaint::kStroke_Style);
		canvas->drawLine(SkPoint::Make(_A.X, _A.Y), SkPoint::Make(_B.X, _B.Y), paint);
	}
	void PushAxisAlignedClip(SkCanvas* canvas, const __Rect& _rect) {
		//canvas->clipIRect(SkIRect::MakeXYWH(_rect.X, _rect.Y, _rect.Width, _rect.Height), SkClipOp::kIntersect);
	}
	void PopAxisAlignedClip(SkCanvas* canvas) {
		//canvas->clipRegion(SkRegion());
	}
	void PushLayer(SkCanvas* canvas, const Geometry& dxGeometry) {

	}
	void PopLayer(SkCanvas* canvas) {

	}
	void DrawImage(SkCanvas* canvas, IImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Margin& margin) {

	}
	void DrawTextLayout(SkCanvas* canvas, const __Point& startLacation, IDWriteTextLayout* textLayout, const __Color& color) {

	}
	void DrawString(SkCanvas* canvas, const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  __Color& color, const  __Rect& _rect, EzUI::TextAlign textAlign, bool underLine) {

	}
}
#endif