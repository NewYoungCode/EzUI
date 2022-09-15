#pragma once
#include "CPURender.h"
namespace EzUI {
	class UI_EXPORT GdI
	{
	private:
		HDC _hdc = NULL;
		HRGN _rgn = NULL;
	public:
		GdI(HDC dc);
		virtual ~GdI();
		void SetClip(const Rect& rect);
		void DrawRect(const Rect& rect, const COLORREF& rgb, int radius = 0, int width = 1);
		void FillRect(const Rect& rect, const COLORREF& rgb, int radius = 0);
	};
};