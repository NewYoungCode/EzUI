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
	// ժҪ: 
	// Ϊ OnPaint �¼��ṩ���ݡ�
	struct PaintEventArgs {
		HDC DC = NULL;
		Painter& Painter;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		HWND HWnd;//�����ھ��
		PaintEventArgs(EzUI::Painter& painter) :Painter(painter) {}
		virtual ~PaintEventArgs() {}
	};
};