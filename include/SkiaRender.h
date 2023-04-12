#pragma once
#include "UIDef.h"
#if USED_Skia
#ifndef UI_EXPORT
#define UI_EXPORT 
#endif
#include "RenderType.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRegion.h"

namespace EzUI {
	class Geometry {

	};
	class IDWriteTextLayout {

	};

	class TextFormat {
	public:
		TextFormat(const std::wstring& fontFamily, int fontSize, TextAlign textAlign) {}
	};

	class TextLayout {
	public:
		TextLayout(const std::wstring& text, __Size maxSize, TextFormat* pTextFormat) {
			//g_WriteFactory->CreateTextLayout(text.c_str(), text.size(), pTextFormat->value, (FLOAT)maxSize.Width, (FLOAT)maxSize.Height, &value);
		}
		__Point HitTestPoint(const __Point& pt, int& textPos, BOOL& isTrailingHit) {
			//DWRITE_HIT_TEST_METRICS hitTestMetrics;
			////BOOL isTrailingHit;
			//BOOL isInside;
			//{
			//	FLOAT x = (FLOAT)pt.X, y = (FLOAT)pt.Y;
			//	value->HitTestPoint(
			//		(FLOAT)x,
			//		(FLOAT)y,
			//		&isTrailingHit,
			//		&isInside,
			//		&hitTestMetrics
			//	);
			//}
			//int posX = (int)(hitTestMetrics.left + 0.5);
			//if (isTrailingHit) {//判断前侧还是尾侧
			//	posX += (int)(hitTestMetrics.width + 0.5);
			//}
			//textPos = hitTestMetrics.textPosition;
			//return __Point{ posX,(int)(hitTestMetrics.top + 0.5) };//返回光标所在的位置
			return { 0,0 };

		}
		__Point HitTestTextPosition(int textPos, BOOL isTrailingHit) {
			/*	DWRITE_HIT_TEST_METRICS hitTestMetrics;
				FLOAT X, Y;
				value->HitTestTextPosition(textPos, isTrailingHit, &X, &Y, &hitTestMetrics);
				return __Point((int)(X + 0.5), (int)(Y + 0.5));*/

			return { 0,0 };
		}
		__Size GetFontSize() {
			/*DWRITE_TEXT_METRICS textMetrics;
			value->GetMetrics(&textMetrics);
			D2D1_SIZE_F size = D2D1::SizeF(ceil(textMetrics.widthIncludingTrailingWhitespace), ceil(textMetrics.height));*/
			/*return  __Size{ (int)(size.width + 0.5) ,(int)((size.height / textMetrics.lineCount) + 0.5) };*/
			return { 0,0 };

		}
	};

	class UI_EXPORT Image : public IImage {
	protected:
		UINT Width = 0;
		UINT Height = 0;
	public:
	protected:
		void CreateFormStream(IStream* istram) {}
		void CreateFromFile(const std::wstring& file) {}
		void Init() {}
	public:
		void DecodeOfRender() {}
		Image(HBITMAP hBitmap) {}
		Image(IStream* istram) {}
		Image(const std::wstring& file) {}
		Image(const std::string& utf8File) {}

		UINT GetWidth() { return 0; }
		UINT GetHeight() { return 0; }
		virtual size_t NextFrame() { return 0; }
		Image() {}
		virtual ~Image() {}
	};
	using Painter = SkCanvas;
};
namespace EzUI {
	UI_EXPORT void RenderInitialize();//全局初始化
	UI_EXPORT void RenderUnInitialize();//释放
	UI_EXPORT SkCanvas* CreateRender(void* pixels, int Width, int Height);
	UI_EXPORT void ReleaseRender(SkCanvas* canvas);
	UI_EXPORT void FillRectangle(SkCanvas* canvas, const __Rect& _rect, const __Color& color, int _radius = 0);
	UI_EXPORT void DrawRectangle(SkCanvas* canvas, const  __Rect& _rect, const  __Color& color, int width = 1, int _radius = 0);
	UI_EXPORT void SetTransform(SkCanvas* canvas, int xOffset, int yOffset);
	UI_EXPORT void DrawLine(SkCanvas* canvas, const __Color& color, const __Point& _A, const __Point& _B, int width);
	UI_EXPORT void PushAxisAlignedClip(SkCanvas* canvas, const __Rect& rectBounds);
	UI_EXPORT void PopAxisAlignedClip(SkCanvas* canvas);
	UI_EXPORT void PushLayer(SkCanvas* canvas, const Geometry& dxGeometry);
	UI_EXPORT void PopLayer(SkCanvas* canvas);//弹出最后一个裁剪
	UI_EXPORT void DrawImage(SkCanvas* canvas, IImage* _image, const __Rect& _rect, const ImageSizeMode& imageSizeMode, const EzUI::Margin& margin = 0);
	UI_EXPORT void DrawTextLayout(SkCanvas* canvas, const __Point& startLacation, IDWriteTextLayout* textLayout, const __Color& color);
	UI_EXPORT void DrawString(SkCanvas* canvas, const std::wstring& text, const std::wstring& fontFamily, int fontSize, const  __Color& color, const  __Rect& _rect, EzUI::TextAlign textAlign, bool underLine = false);
};

#endif