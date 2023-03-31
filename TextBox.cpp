#include "TextBox.h"
namespace EzUI {
#define FontHeight  FontBox.Height
	TextBox::~TextBox() {
		timer.Stop();
		if (textFormat) delete textFormat;
		if (textLayout) delete textLayout;
	}

	void TextBox::OnRemove() {
		timer.Stop();
		__super::OnRemove();
	}

	TextBox::TextBox() {
		Cursor = Cursor::IBEAM;
		timer.Interval = 500;
		timer.Tick = [&](Windows::Timer*) {
			if (!careRect.IsEmptyArea() && _focus) {
				_careShow = !_careShow;
				this->Invalidate();
			}
		};
	}
	void TextBox::OnChar(WPARAM wParam, LPARAM lParam)
	{
		//Debug::Log(utf8("按下了%d"), wParam);
		do
		{
			if (wParam == VK_BACK) { //退格键
				OnBackspace();//退格键的操作在里面
				Analysis();//重新分析
				Invalidate();//刷新
				break;
			}
			if (wParam == 1) {
				SelectedAll();//全选
				Invalidate();//刷新
				break;
			}
			if (wParam == 3) {//复制
				Copy();
				break;
			}
			if (wParam == 24) {//ctrl+x裁剪
				if (Copy()) {
					DeleteRange();//因为是剪切 所以要删除选中的这段
					Analysis();
					Invalidate();//刷新
				}
				break;
			}
			if (wParam == 26) {//ctrl+z撤销
				break;
			}
			if (wParam == 22) {
				Paste();//粘贴
				Analysis();//分析字符串
				Invalidate();//刷新
				break;
			}


		} while (false);

		if (wParam < 32)return;//控制字符

		DeleteRange();//先删除是否有选中的区域
		TCHAR buf[2]{ (TCHAR)wParam ,0 };
#ifdef UNICODE
		Insert(buf);//插入新的字符
#else
		byte _ch = (byte)wParam;
		if (_ch > 127) {
			ansiBuf += _ch;
			if (ansiBuf.size() > 1) {//判断是否中文
				std::wstring wBuf;
				EString::ANSIToUniCode(ansiBuf, &wBuf);
				Insert(wBuf);//插入新的字符
				ansiBuf = "";
			}
			else {
				return;
			}
		}
		else {
			WCHAR buf[2]{ (WCHAR)wParam ,0 };
			Insert(buf);//插入新的字符
		}

#endif // !UINCODE
		Analysis();//分析字符串
		Invalidate();//刷新
	}

	bool TextBox::SelectedAll() {
		if (textLayout && !text.empty()) {
			A = Point{ 0,0 };
			A_isTrailingHit = FALSE;
			A_TextPos = 0;

			B = Point{ FontBox.Width ,0 };
			B_isTrailingHit = TRUE;
			B_TextPos = text.size() - 1;

			selectRect.X = 0;
			selectRect.Y = (this->Height() - FontHeight) / 2;
			selectRect.Width = FontBox.Width;
			selectRect.Height = FontBox.Height;
			return true;
		}
		return false;
	}

	bool TextBox::GetSelectedRange(int* outPos, int* outCount) {
		if (!selectRect.IsEmptyArea()) {
			int pos, count;
			if (A.X < B.X) {
				int pos1 = A_TextPos;
				if (A_isTrailingHit == 1) {
					pos1 += 1;
				}
				int pos2 = B_TextPos;
				if (B_isTrailingHit == 0) {
					pos2 -= 1;
				}
				pos = pos1;
				count = std::abs(pos2 - pos1) + 1;
			}
			else {
				int pos1 = A_TextPos;
				if (A_isTrailingHit == 0) {
					pos1 -= 1;
				}
				int pos2 = B_TextPos;
				if (B_isTrailingHit == 1) {
					pos2 += 1;
				}
				pos = pos2;
				count = std::abs(pos2 - pos1) + 1;
			}
			*outPos = pos;
			*outCount = count;
			if (outCount > 0) {
				return true;
			}
		}
		return false;
	}
	void TextBox::Insert(const std::wstring& str) {
		if (TextPos < 0)TextPos = 0;
		if (TextPos > (int)text.size()) {
			TextPos = text.size();
		}
		text.insert(TextPos, str);
		TextPos += str.size();
		if (TextChange) {
			TextChange(EString(text));
		}
	}
	bool TextBox::DeleteRange() {
		int pos, count;
		if (GetSelectedRange(&pos, &count)) {//删除选中的
			//isTrailingHit = FALSE;
			TextPos = pos;
			text.erase(pos, count);

			if (TextChange) {
				TextChange(EString(text));
			}
			return true;
		}
		return false;
	}
	bool TextBox::Copy() {
		do
		{
			int pos, count;
			if (!GetSelectedRange(&pos, &count))break;
			std::wstring wBuf(text.substr(pos, count));
			std::string str;
			EString::UnicodeToANSI(wBuf, &str);

			//打开剪贴板
			if (!OpenClipboard(PublicData->HANDLE))break;
			//清空剪贴板
			EmptyClipboard();
			//为剪切板申请内存
			HGLOBAL clip = GlobalAlloc(GMEM_DDESHARE, (str.size() + 1));
			memcpy((void*)clip, str.c_str(), (str.size() + 1));
			//解锁
			GlobalUnlock(clip);
			SetClipboardData(CF_TEXT, clip);
			CloseClipboard();
			return true;
		} while (false);
		return false;
	}

	bool TextBox::Paste() {

		do
		{
			//只接收文本
			if (!IsClipboardFormatAvailable(CF_TEXT))break;
			//打开剪贴版
			if (!OpenClipboard(PublicData->HANDLE))break;
			//获取剪贴板数据
			HANDLE hClipboard = GetClipboardData(CF_TEXT);
			EString buf((CHAR*)GlobalLock(hClipboard));
			EString::Replace(&buf, "\r", "");//行编辑框不允许有换行符
			EString::Replace(&buf, "\n", "");//行编辑框不允许有换行符
			std::wstring wBuf;

			EString::ANSIToUniCode(buf, &wBuf);
			//解锁
			GlobalUnlock(hClipboard);
			CloseClipboard();

			DeleteRange();//先删除是否有选中的区域
			Insert(wBuf);//插入新的字符
			return true;

		} while (false);
		return false;
	}
	void TextBox::OnBackspace() {
		if (text.size() <= 0)return;

		if (!DeleteRange()) {//先看看有没有有选中的需要删除
			//否则删除单个字符
			TextPos--;
			if (TextPos > -1) {
				text.erase(TextPos, 1);
				if (TextChange) {
					TextChange(EString(text));
				}
			}
		}
	}

	void TextBox::OnKeyDown(WPARAM wParam, LPARAM lParam)
	{
		__super::OnKeyDown(wParam, lParam);
		/*	if (wParam == 16) {
				_down = true;
			}*/
		if (wParam == VK_LEFT) {
			TextPos--;
			_careShow = true;
			selectRect = Rect();
			BuildCare();
			Invalidate();
			return;
		}
		if (wParam == VK_RIGHT) {
			TextPos++;
			_careShow = true;
			selectRect = Rect();
			BuildCare();
			Invalidate();
			return;
		}
		//Debug::Log(utf8("按下了%d"), wParam);
	}

	void TextBox::OnKeyUp(WPARAM wParam, LPARAM lParam) {
		__super::OnKeyUp(wParam, lParam);
	}
	void TextBox::Analysis()
	{
		A = Point();
		A_isTrailingHit = 0;
		A_TextPos = 0;
		B = Point();
		B_isTrailingHit = 0;
		B_TextPos = 0;
		selectRect = Rect();
		careRect = Rect();
		if (GetFontFamily().empty() || GetFontSize() == 0 || GetRect().IsEmptyArea()) return;
		if (textFormat) delete textFormat;
		textFormat = new TextFormat(GetFontFamily().utf16(), GetFontSize(), TextAlign::MiddleLeft);
		if (textLayout) delete textLayout;
		textLayout = new TextLayout(text, { 16777216, Height() }, textFormat);

		FontBox = textLayout->GetFontSize();

		if (FontBox.Width < this->Width()) {
			x = 0;
		}

		//支持编辑 所以暂且注释掉
		/*if (FontBox.Width > this->Width()) {
			x = this->Width() - FontBox.Width;
		}
		else {
			x = 0;
		}*/
		BuildCare();
	}

	void TextBox::BuildCare() {
		if (!textLayout) return;

		if (TextPos < 0) {
			TextPos = 0;
		}
		if (TextPos > (int)text.size()) {
			TextPos = text.size();
		}
		/*char buf[256]{ 0 };
		sprintf_s(buf, "%d TextSize %d \n", TextPos,text.size());
		OutputDebugStringA(buf);*/
		Point pt = textLayout->HitTestTextPosition(TextPos, FALSE);
		careRect.X = pt.X;
		careRect.Y = pt.Y;
		careRect.Height = FontHeight;
		careRect.Width = 1;
	}

	void TextBox::OnMouseDown(MouseButton mbtn, const Point& point) {
		__super::OnMouseDown(mbtn, point);
		_focus = true;
		lastX = 0;
		Invalidate();
		_careShow = true;
		timer.Start();

		if (mbtn == MouseButton::Left) {
			_down = true;
			point_Start = ConvertPoint(point);

			if (textLayout == NULL) {
				Analysis();
			}
			if (textLayout) {
				selectRect = Rect();
				A = textLayout->HitTestPoint(point_Start, A_TextPos, A_isTrailingHit);
				careRect.X = A.X;
				careRect.Y = A.Y;
				careRect.Width = 1;
				careRect.Height = FontHeight;

				TextPos = A_TextPos;
				if (A_isTrailingHit) {
					TextPos++;
				}
			}
			Invalidate();
		}
	}
	void TextBox::OnMouseWheel(short zDelta, const Point& point) {
		__super::OnMouseWheel(zDelta, point);
		//if (x == 0)return;
		/*if (zDelta > 0) {
			x+=4;
		}
		else {
			x-=4;
		}
		Invalidate();*/
	}

	Point TextBox::ConvertPoint(const Point& pt) {
		int _x = -x;
		return Point{ pt.X + _x,pt.X };
	}

	void TextBox::OnMouseMove(const Point& point)
	{
		__super::OnMouseMove(point);
		if (_down) {
			point_End = ConvertPoint(point);
			if (textLayout) {
				selectRect = Rect();
				B = textLayout->HitTestPoint(point_End, B_TextPos, B_isTrailingHit);
				Rect& rect = selectRect;
				rect.X = A.X;
				rect.Y = A.Y;
				rect.Height = FontHeight;
				rect.Width = B.X - A.X;
				if (rect.Width < 0) {
					rect.X = B.X;
					rect.Y = B.Y;
					rect.Width = -rect.Width;
				}

				//当鼠标往左侧移动
				int textWidth = textLayout->GetFontSize().Width;
				if (lastX > point.X) {
					lastX = point.X;
					if (textWidth > Width() && x < 0 && point.X < 0) {
						x += 2;
						Invalidate();
						return;
					}
				}
				//当鼠标往右侧移动
				if (lastX < point.X) {
					lastX = point.X;
					if (textWidth > Width() && point.X > Width()) {
						x -= 2;
						if (-x + Width() > textWidth) {
							x = -(textWidth - Width());
						}
						Invalidate();
						return;
					}
				}
				Invalidate();
			}
		}
	}
	void TextBox::OnMouseUp(MouseButton mbtn, const Point& point)
	{
		__super::OnMouseUp(mbtn, point);
		_down = false;
		lastX = 0;
		Invalidate();
		this;
	}
	void TextBox::OnKillFocus()
	{
		__super::OnKillFocus();
		_down = false;
		_focus = false;
		_careShow = false;
		timer.Stop();
		this->Invalidate();
	}
	const EString TextBox::GetText()
	{
		return EString(text);
	}
	void TextBox::SetText(const EString& _text)
	{
		text = _text.utf16();
		Analysis();
	}
	void TextBox::SetAttribute(const EString& key, const EString& value) {
		__super::SetAttribute(key, value);
		if (key == "placeholder") {
			Placeholder = value;
		}
		if (key == "text") {
			SetText(value);
		}
	}
	void TextBox::OnSize(const Size& sz) {
		__super::OnSize(sz);
		Analysis();
	}

	void TextBox::OnForePaint(PaintEventArgs& e) {
		if (text.empty()) {
			Color c = GetForeColor();
			byte r = c.GetR() - 30;
			byte g = c.GetG() - 30;
			byte b = c.GetB() - 30;
			e.Painter.DrawString(Placeholder.utf16(), GetFontFamily().utf16(), GetFontSize(), Color(r, g, b), { 0,0,Width(),Height() }, TextAlign::MiddleLeft);
		}
		if (textLayout) {
			e.Painter.DrawTextLayout({ x,0 }, textLayout, GetForeColor());
		}
		if (!selectRect.IsEmptyArea()) {
			Rect rect(selectRect);
			rect.X += x;//偏移
			e.Painter.FillRectangle(rect, SelectColor);
		}
		if (!careRect.IsEmptyArea() && _focus) {
			if (_careShow) {
				Rect rect(careRect);
				rect.X += x;//偏移
				if (rect.X == this->Width()) {//如果刚好处于边界
					rect.X = this->Width() - 1;
				}
				e.Painter.FillRectangle(rect, CareColor);
			}
		}
	}
}