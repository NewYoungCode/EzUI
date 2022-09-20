#pragma once
#include "EzUI.h"
#include "GdiplusRender.h"
#include "Direct2DRender.h"
#include "RenderExType.h"
namespace EzUI {

#ifdef USED_GDIPLUS
	using Painter = GdiplusRender;
	//class Image :public GdiplusImage {
	//public:
	//	Image(const std::wstring& file) :GdiplusImage(file) {};
	//	Image(HBITMAP hBitmap) :GdiplusImage(hBitmap) {}
	//	virtual ~Image() {};
	//};
	using Image = GdiplusImage;
#endif

#ifdef USED_Direct2D
	using Painter = Direct2DRender;
	//class Image :public DXImage {
	//public:
	//	//Image(const EString& file) :D2DImage(file.utf16()) {};
	//	Image(const std::wstring& file) :DXImage(file) {};
	//	Image(HBITMAP hBitmap) :DXImage(hBitmap) {}
	//	virtual ~Image() {};
	//};
	using Image = DXImage;
#endif

#define HImage Tuple<Image*>
	// 摘要: 
	// 为 OnPaint 事件提供数据。
	struct PaintEventArgs {
		HDC DC = NULL;
		Painter& Painter;//画家
		Rect InvalidRectangle;//WM_PAINT里面的无效区域
		HWND HWnd;//父窗口句柄
		PaintEventArgs(EzUI::Painter& painter) :Painter(painter) {}
		virtual ~PaintEventArgs() {}
	};
};