#include "Edit.h"
//#include "EzUI.h"
namespace EzUI {
	Point HitTestPoint(const Point& pt, IDWriteTextLayout* textLayout) {
		DWRITE_HIT_TEST_METRICS hitTestMetrics;
		BOOL isTrailingHit;
		BOOL isInside;
		{
			FLOAT x = (FLOAT)pt.X, y = (FLOAT)pt.Y;
			textLayout->HitTestPoint(
				(FLOAT)x,
				(FLOAT)y,
				&isTrailingHit,
				&isInside,
				&hitTestMetrics
			);
		}
		////绘制光标
		int posX = (int)(hitTestMetrics.left + 0.5);
		if (isTrailingHit) {//判断前侧还是尾侧
			posX += (int)(hitTestMetrics.width + 0.5);
		}
		return Point{ posX,(int)(hitTestMetrics.top + 0.5) };
	}
	/*Edit::~Edit() {
		if (textFormat) delete textFormat;
		if (textLayout) delete textLayout;
	}*/
	void Edit::OnChar(WPARAM wParam, LPARAM lParam)
	{
		if (wParam == VK_BACK) { //退格键
			if (buf.size() > 0) {
				buf.erase(buf.size() - 1, 1);
				Analysis();
				Invalidate();
			}
		}
		if (wParam < 32)return;//控制字符
		WCHAR&& _char = (WCHAR)wParam;
		buf += _char;
		Analysis();
		Invalidate();
	}
	void Edit::OnKeyDown(WPARAM wParam)
	{
		__super::OnKeyDown(wParam);
		//Debug::Log(utf8("按下了%d"), wParam);
	}
	void Edit::Analysis()
	{
		selectRect = Rect();
		careRect = Rect();
		if (buf.empty() || GetRect().IsEmptyArea()) return;
		if (textFormat) delete textFormat;
		textFormat = new DxTextFormat(GetFontFamily().utf16(), GetFontSize(), TextAlign::MiddleLeft);
		if (textLayout) delete textLayout;
		textLayout = new DxTextLayout(buf, { 16777216, Height() }, textFormat->value);
		FontHeight = textLayout->GetFontSize().Height;
	}
	void Edit::OnMouseDown(MouseButton mbtn, const Point& point) {
		__super::OnMouseDown(mbtn, point);
		if (mbtn == MouseButton::Left) {
			_focus = true;
			Point pointF{ point.X,point.Y };
			point_Start = pointF;

			if (textLayout) {
				selectRect = Rect();

				Point a = HitTestPoint(point_Start, textLayout->value);
				careRect.X = a.X;
				careRect.Y = a.Y;
				careRect.Width = 1;
				careRect.Height = FontHeight;
			}
			Invalidate();
		}
	}
	void Edit::OnMouseMove(const Point& point)
	{
		if (_focus) {
			point_End = point;
			//Debug::Log("%d %d", point_End.X, point_End.Y);
			if (textLayout) {
				selectRect = Rect();
				
				Point a = HitTestPoint(point_Start, textLayout->value);
				Point b = HitTestPoint(point_End, textLayout->value);
				Rect& rect = selectRect;
				rect.X = a.X;
				rect.Y = a.Y;
				rect.Height = FontHeight;
				rect.Width = b.X - a.X;
				if (rect.Width < 0) {
					rect.X = b.X;
					rect.Y = b.Y;
					rect.Width = -rect.Width;
				}
				Invalidate();
			}
		}
	}
	void Edit::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		_focus = false;
		Invalidate();
		this;
	}
	void Edit::OnKillFocus()
	{
		__super::OnKillFocus();
		_focus = false;
	}
	EString Edit::GetText()
	{
		return EString(buf);
	}
	void Edit::SetText(const EString& text)
	{
		buf = text.utf16();
		Analysis();
	}
	void Edit::OnSize(const Size& sz) {
		__super::OnSize(sz);
		Analysis();
	}

	void Edit::OnForePaint(PaintEventArgs& e) {
		if (textLayout) {
			e.Painter.DrawTextLayout({ 0,0 }, textLayout->value, GetForeColor());
		}
		if (!selectRect.IsEmptyArea()) {
			e.Painter.FillRectangle(selectRect, Color(100, 255, 0, 0));
		}
		if (!careRect.IsEmptyArea()) {
			e.Painter.FillRectangle(careRect, Color(255, 0, 0, 0));
		}

	}
}