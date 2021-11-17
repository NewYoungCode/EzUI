#include "Edit.h"
//
//void Edit::Analysis(const EString & str, Painter&painter)
//{
//
//}
//
//Edit::Edit()
//{
//	Cursor = IDC_IBEAM;//设置光标状态
//	this->_Type = ControlType::ControlEdit;
//}
//
//void Edit::OnBackgroundPaint(PaintEventArgs &e) {
//	__super::OnBackgroundPaint(e);
//}
//
//void Edit::OnPaint(PaintEventArgs&e)
//{
//	//__super::OnPaint(painter);
//	_chars.clear();
//	//auto fontFamily = GetFontFamily(this->State);
//	//auto fontSize = GetFontSize(this->State);
//	//int count = 0;
//	//float left = 0;
//	//size_t i = 0;
//	//for (auto &wchar : _wtext) {
//	//	WCHAR buf[2]{ wchar,0 };
//	//	RectF box;
//	//	e.Painter->MeasureString(buf, fontFamily, fontSize, box);
//	//	box.Height = _rect.Height;
//	//	box.X = left;
//	//	left += box.Width-4;
//	//	_chars.push_back({ i,buf ,box });
//	//	e.Painter->DrawString(buf, fontFamily, fontSize, GetForeColor(this->State), box, TextAlign::MiddleCenter);
//	//	//Gdiplus::DllExports::GdipDrawDriverString(e.Painter->base, buf.c_str(),)
//	//	e.Painter->DrawRectangle(Color::Red, Rect{ (int)box.X,(int)box.Y,(int)box.Width,(int)box.Height });
//	//	i++;
//	//}
//}
//void Edit::OnChar(WPARAM wParam, LPARAM lParam) {
//
//	if (wParam == VK_BACK) { //退格键
//		if (index > -1) {
//			_wtext.erase(index, 1);
//			index--;
//			Refresh();
//		}
//	}
//	if (lParam == 3) {
//		////打开剪贴板
//		//if (!OpenClipboard(m_hWnd))
//		//	break;
//		////清空剪贴板
//		//EmptyClipboard();
//		//std::string str(tmp.length() * 3, ' ');
//		//int size = WideCharToMultiByte(CP_OEMCP, 0, tmp.c_str(), -1, &str[0], str.length(), NULL, NULL);
//		//str.resize(size);
//		////分配内存	
//		//HGLOBAL clip_textf = GlobalAlloc(GMEM_DDESHARE, (str.length() + 1));
//		//char *_textfer = (char*)GlobalLock(clip_textf);
//		//memcpy(_textfer, str.c_str(), (str.length() + 1));
//		//GlobalUnlock(clip_textf);
//		//SetClipboardData(CF_TEXT, clip_textf);
//		//CloseClipboard();
//	}
//	if (lParam == 22) { //Ctrl +v复制
//		////只接收文本
//		//if (!IsClipboardFormatAvailable(CF_TEXT))
//		//	break;
//		////打开剪贴版
//		//if (!OpenClipboard(m_hWnd))
//		//	break;
//		////获取剪贴板数据
//		//HANDLE hClipboard = GetClipboardData(CF_TEXT);
//		//std::string tmp = (CHAR*)GlobalLock(hClipboard);
//		////解锁
//		//GlobalUnlock(hClipboard);
//		//CloseClipboard();
//
//	}
//	if (wParam <= 31)return;//控制字符
//	TCHAR _char = (TCHAR)wParam;
//#ifndef UNICODE
//	if (_char < 0) {
//		_buffer.push_back(_char);
//		if (_buffer.size() >= 2) {
//			char buf[3]{ 0 };
//			buf[0] = _buffer.at(0);
//			buf[1] = _buffer.at(1);
//			_buffer.clear();
//			std::wstring _char = String::ANSIToUniCode(buf);
//			if (_wtext.size() == 0) {
//				_wtext += _char;
//			}
//			else if (index <= 0 && _wtext.size() > 0) {
//				_wtext.insert(index, _char);
//			}
//			else if (index > 0) {
//				_wtext.insert(index, _char);
//			}
//			Refresh();
//		}
//	}
//	else {
//		_wtext += (WCHAR)wParam;
//		Refresh();
//	}
//#else
//	_wtext += _char;
//	InvaliDate();
//#endif
//
//}
//
//void Edit::OnKeyDown(WPARAM wParam)
//{
//	Debug::Log("按下了%d", wParam);
//}
//
//void Edit::OnMouseDown(MouseButton mbtn, const Point& point) {
//	if (mbtn == MouseButton::Left) {
//		PointF pointF{ (float)point.X,(float)point.Y };
//		size_t index = 0;//默认当没有字,索引为0
//		bool _after = false;
//		bool _find = false;
//		for (auto &it : _chars) {
//			bool _break = false;
//			if (it._rect.Contains(pointF)) {
//				float _a = it._rect.X + it._rect.Width*1.0 / 2;//检测鼠标更偏向前一个字体还是后一个字体
//				_find = true;
//				if (pointF.X < _a) {
//				}
//				else {
//				}
//				_break = true;
//			}
//			if (_break)break;
//			index++;
//		}
//		//if (_after) index++;
//
//		Debug::Log("         索引 %d", index);
//		MoveCaret(_left);
//	}
//}
//
//
//void Edit::Analyse() {
//
//}
//
//void Edit::MoveCaret(int X) {
//	::DestroyCaret();
//	int careHeight = _rect.Height*0.6;//光标高度
//	::CreateCaret(ParentWid, NULL, 1, careHeight);
//	auto &clientRect = GetClientRect();
//	::SetCaretPos(clientRect.X + X, clientRect.Y + (Height() - careHeight) / 2);
//	::ShowCaret(ParentWid);
//}
//
//void Edit::OnMouseMove(const Point&point) {
//	__super::OnMouseMove(point);
//	//SetCursor(LoadCursor(NULL, IDC_IBEAM));
//}
//void Edit::OnMouseLeave() {
//	__super::OnMouseLeave();
//	//SetCursor(LoadCursor(NULL, IDC_ARROW));
//}
//

EditWnd::EditWnd()
{
	this->_Type = ControlType::ControlEditWin32;
}
EditWnd::~EditWnd()
{
	if (_font) {
		DeleteFont(_font);
	}if (_brush) {
		DeleteBrush(_font);
	}
}
LRESULT EditWnd::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == uMsg && (HWND)lParam == _editWnd) {
		auto hword = HIWORD(wParam);
		Debug::Log(TEXT("%x"), hword);
		OnKeyDown(hword);
	}
	if (uMsg == WM_CTLCOLOREDIT && (HWND)lParam == _editWnd) {
		auto bkColor = GetBackgroundColor();
		auto foreColor = GetForeColor(State);
		DWORD _foreColor = RGB(foreColor.GetR(), foreColor.GetG(), foreColor.GetB());
		DWORD _bkColor = RGB(bkColor.GetR(), bkColor.GetG(), bkColor.GetB());
		SetBkMode((HDC)wParam, TRANSPARENT);
		SetTextColor((HDC)wParam, _foreColor);
		if (_brush) DeleteBrush(_brush);
		return (INT_PTR)(_brush = CreateSolidBrush(_bkColor));
	}
	return ::DefWindowProc(_hWnd, uMsg, wParam, lParam);
}
void EditWnd::SetRect(const Rect& rect, bool rePaint)
{
	__super::SetRect(rect, rePaint);

}
void EditWnd::SetText(const EString& text)
{
	_text = text;
	if (_editWnd) {
		::SetWindowTextW(_editWnd, _text.utf16().c_str());
	}
}
const EString& EditWnd::GetText()
{
	WCHAR buf[256]{ 0 };
	::GetWindowTextW(_editWnd, buf, 255);
	_text = buf;
	return _text;
}
void EditWnd::OnLoad() {
	if (_editWnd == NULL) {
		auto rect = GetClientRect();
		_editWnd = CreateWindowW(L"Edit", _text.utf16().c_str(), WS_VISIBLE | WS_CHILD, rect.X, rect.Y, rect.Width, rect.Height,
			_hWnd, NULL, GetModuleHandle(0), NULL);//因为UI框架的edit并不成熟(懒得写了),所以暂时使用WIN32的输入框代替
		UI_SetUserData(_editWnd, this);
	}
}

HFONT EditWnd::SetFont(float fontHeight, const WCHAR* faceName) {
	if (_font) {
		DeleteFont(_font);
	}
	LOGFONTW LogFont;
	memset(&LogFont, 0, sizeof(LOGFONT));
	lstrcpyW(LogFont.lfFaceName, faceName);
	LogFont.lfWeight = FW_NORMAL;//FW_NORMAL;
	LogFont.lfHeight = fontHeight; // 字体大小
	LogFont.lfCharSet = 134;
	LogFont.lfOutPrecision = 3;
	LogFont.lfClipPrecision = 2;
	LogFont.lfOrientation = 45;
	LogFont.lfQuality = 1;
	LogFont.lfPitchAndFamily = 2;
	// 创建字体
	_font = CreateFontIndirectW(&LogFont);
	// 取得控件句柄
	if (_editWnd) {
		SendMessageW(_editWnd, WM_SETFONT, (WPARAM)_font, TRUE);
	}

	return _font;
}

void EditWnd::SetVisible(bool visible) {
	if (_editWnd) {
		::ShowWindow(_editWnd, visible? SW_SHOW: SW_HIDE);
	}
}

void EditWnd::OnPaint(PaintEventArgs& e) {
	__super::OnPaint(e);

	if (_editWnd) {
		auto font_size = GetFontSize(this->State);
		auto font_faceName = GetFontFamily(this->State);
		SetFont(font_size, font_faceName.utf16().c_str());
		auto rect = GetClientRect();
		rect.Y += (rect.Height - font_size) / 2;
		rect.X += 1;
		rect.Width -= 2;
		::MoveWindow(_editWnd, rect.X, rect.Y, rect.Width, font_size, TRUE);
	}

}

