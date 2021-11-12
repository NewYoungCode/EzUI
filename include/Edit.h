#pragma once
#include "Control.h"
//struct _edit_char_ {
//	size_t _index;
//	std::wstring _char;
//	RectF _rect;
//};
//class Edit :
//	public Control
//{
//private:
//	bool _end = false;
//	int _left =  1;
//	int index = 0;
//	std::vector<_edit_char_> _chars;//文字集合
//	std::wstring _wtext;
//	std::vector<TCHAR> _buffer;
//	WORD _caret = 0;//光标位置
//	void Analysis(const EString&str,  Painter&painter);
//protected:
//	void OnBackgroundPaint(PaintEventArgs & e);
//	virtual void OnPaint(PaintEventArgs&args) override;
//	virtual void OnChar(WPARAM wParam, LPARAM lParam)override;
//	virtual void OnKeyDown(WPARAM wParam)override;
//	virtual	void OnMouseDown(MouseButton mbtn, const Point & point)override;
//	virtual	void Analyse();
//	virtual	void MoveCaret(int X);
//	virtual void OnMouseMove(const Point & point)override;
//	virtual void OnMouseLeave()override;
//public:
//	Edit();
//};

class UI_EXPORT EditWnd :
	public Control
{
private:
	HWND _editWnd = NULL;
	EString _text;
	HFONT _font = NULL;
	HBRUSH _brush = NULL;
public:
	EditWnd();
	virtual ~EditWnd();
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void SetRect(const Rect&rect, bool rePaint=true)override;
	virtual void SetText(const EString&text);
	const EString& GetText();
	virtual void OnLoad()override;
	virtual void OnPaint(PaintEventArgs & e)override;
};