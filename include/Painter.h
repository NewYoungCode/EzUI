#pragma once
#include "EzUI.h"
#include "GdiplusRender.h"
#include "Direct2DRender.h"

namespace EzUI {

#ifdef USED_GDIPLUS
	using Painter = GdiplusRender;
	class Image :public GdiplusImage {
	public:
		Image(const std::wstring& file) :GdiplusImage(file) {};
		Image(HBITMAP hBitmap) :GdiplusImage(hBitmap) {}
		virtual ~Image() {};
	};
#endif

#ifdef USED_Direct2D
	using Painter = Direct2DRender;
	class Image :public D2DImage {
	public:
		//Image(const EString& file) :D2DImage(file.utf16()) {};
		Image(const std::wstring& file) :D2DImage(file) {};
		Image(HBITMAP hBitmap) :D2DImage(hBitmap) {}
		virtual ~Image() {};
	};
#endif

#define HImage Tuple<Image*>
	// ժҪ: 
	// Ϊ OnPaint �¼��ṩ���ݡ�
	struct PaintEventArgs {
		Painter& Painter;//����
		Rect InvalidRectangle;//WM_PAINT�������Ч����
		HWND HWnd;//�����ھ��
		PaintEventArgs(EzUI::Painter& painter) :Painter(painter) {}
		virtual ~PaintEventArgs() {}
	};
};