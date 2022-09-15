#include "GDIRender.h"
#include <Windowsx.h>
namespace EzUI {
	GdI::GdI(HDC dc) {
		_hdc = dc;
	}
	GdI::~GdI() {
		if (_rgn) {
			DeleteRgn(_rgn);
		}
	}
	void GdI::SetClip(const Rect& rect) {
		if (_rgn) {
			DeleteRgn(_rgn);
		}
		_rgn = ::CreateRectRgn(rect.X, rect.Y, rect.GetRight(), rect.GetBottom());
		::SelectClipRgn(_hdc, _rgn);
	}
	void GdI::DrawRect(const Rect& rect, const COLORREF& rgb, int radius , int width ) {
		HPEN pen = ::CreatePen(PS_SOLID, width, rgb);
		auto old = SelectPen(_hdc, pen);
		::RoundRect(_hdc, rect.X, rect.Y, rect.GetRight(), rect.GetBottom(), radius, radius);
		SelectPen(_hdc, old);
		::DeleteObject(pen);
	}
	void GdI::FillRect(const Rect& rect, const COLORREF& rgb, int radius ) {
		HBRUSH brush = ::CreateSolidBrush(rgb);
		auto old = SelectBrush(_hdc, brush);
		RECT r{ rect.X, rect.Y, rect.GetRight(), rect.GetBottom() };

		auto rgn = ::CreateRoundRectRgn(rect.X, rect.Y, rect.GetRight(), rect.GetBottom(), radius, radius);
		::FillRgn(_hdc, rgn, brush);
		DeleteRgn(rgn);

		SelectBrush(_hdc, old);
		::DeleteObject(brush);
	}
}