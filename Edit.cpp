#include "Edit.h"

namespace EzUI {
	void Edit::OnChar(WPARAM wParam, LPARAM lParam)
	{
		if (wParam == VK_BACK) { //退格键
			if (buf.size() > 0) {
				buf.erase(buf.size() - 1, 1);
				Refresh();
			}
		}
		if (wParam < 32)return;//控制字符
		WCHAR&& _char = (WCHAR)wParam;
		buf += _char;
		Refresh();
	}
	void Edit::MoveCaret(int X) {
		if (_focus) {
			::DestroyCaret();
			int careHeight = _rect.Height * 0.6;//光标高度
			::CreateCaret(_hWnd, NULL, 1, careHeight);
			auto&& clientRect = GetClientRect();
			::SetCaretPos(clientRect.X + X, clientRect.Y + (Height() - careHeight) / 2);
			::ShowCaret(_hWnd);
		}
	}
	void Edit::Analysis()
	{
		//if (_Analysis) {
		//	ScriptStringFree(&_Analysis);
		//}
		//SCRIPT_STATE ScriptState;
		//SCRIPT_CONTROL ScriptControl;
		//ZeroMemory(&ScriptState, sizeof(ScriptState));
		//ZeroMemory(&ScriptControl, sizeof(ScriptControl));
		//ScriptApplyDigitSubstitution(NULL, &ScriptControl, &ScriptState);
		//HRESULT hr = ScriptStringAnalyse(e.Painter.DC,
		//	buf.c_str(),
		//	buf.length() + 1,
		//	buf.length() * 3 / 2 + 16,
		//	-1,
		//	SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		//	0,
		//	&ScriptControl,
		//	&ScriptState,
		//	NULL,
		//	NULL,
		//	NULL,
		//	&_Analysis);
		//
		//int pX = 0;
		//ScriptStringCPtoX(_Analysis,
		//	m_nCaret,		//字符在字符串中的位置
		//	false,				//false表示字符前端 true表示后端
		//	&pX);
	}
	void Edit::OnKeyDown(WPARAM wParam)
	{
		__super::OnKeyDown(wParam);
		Debug::Log(utf8("按下了%d"), wParam);
	}
	void Edit::OnMouseDown(MouseButton mbtn, const Point& point) {
		__super::OnMouseDown(mbtn, point);
		_focus = true;
		if (mbtn == MouseButton::Left) {
			Point pointF{ point.X,point.Y };
		}
	}
	void Edit::OnKillFocus()
	{
		__super::OnKillFocus();
		_focus = false;
		::DestroyCaret();
	}
	void Edit::SetText(const EString& text)
	{
		buf = text.utf16();
	}
	EString Edit::GetText()
	{
		return EString(buf);
	}
	void Edit::OnBackgroundPaint(PaintEventArgs& e) {
		__super::OnBackgroundPaint(e);
		int radius = GetRadius() / 2;
		int x = radius, width = Width() - radius * 2;
		if (buf.empty()) {
			MoveCaret(x);
			return;
		}
		//{
		//	Rect rect(x, 0, width, Height()); //文字可绘制范围内
		//	e.Painter.DrawRectangle(Color(100, 200, 200, 200), rect);
		//}
		e.Painter.DrawString(EString(buf), GetFontFamily(), GetFontSize(), GetForeColor(), Rect(x, 0, width, Height()), TextAlign::MiddleLeft);
		RectF box;
		e.Painter.MeasureString(EString(buf), GetFontFamily(), GetFontSize(), box);
		/*RectF box2;
		e.Painter.MeasureString(EString(buf), GetFontFamily(), GetFontSize(), GetForeColor(), RectF(x, 0, width, Height()), TextAlign::MiddleCenter, box2);*/
		MoveCaret(box.Width + x);
	}
}