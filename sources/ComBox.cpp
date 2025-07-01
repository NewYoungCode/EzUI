#include "ComBox.h"

namespace EzUI {
	void ComBox::Init()
	{
		this->_textBox.ReadOnly = true;
		this->Add(&_textBox);
		this->Add(&_UpDown);

		_UpDown.EventHandler = [&](Control* sd, EventArgs& arg)->void {
			if (arg.EventType == Event::OnPaint) {
				//绘制
				auto& args = (PaintEventArgs&)arg;

				auto fSzie = sd->GetFontSize() * 0.5f;
				int_t width = fSzie * 1.5f;

				Rect rect(0, 0, width, fSzie);
				rect.Y = (sd->Height() - fSzie) / 2.0f;
				rect.X += (sd->Height() - width) / 2.0f;

				//args.Graphics.SetColor(Color::Red);
				//args.Graphics.DrawRectangle(rect);

				PointF p1(rect.GetLeft(), rect.Y);
				PointF p2(rect.GetRight(), rect.Y);
				PointF p3(rect.GetLeft() + width / 2.0f, rect.GetBottom());

				args.Graphics.SetColor(this->GetForeColor());
				args.Graphics.DrawLine(p1, p3);
				args.Graphics.DrawLine(p2, p3);
			}
			else if (arg.EventType == Event::OnMouseClick/*&& args.Button == MouseButton::Left*/) {
				//单击
				if (_menuWnd == NULL) {
					_menuWnd = new MenuContent(this, &_UpDown);
					_list.Style.BackColor = Color::White;
					_menuWnd->SetLayout(&_list);
				}
				for (auto& it : _list.GetControls()) {
					it->SetFixedHeight(Height());
				}
				int_t height = this->Height() * _list.GetControls().size();
				if (height == 0) {
					height = Height();
				}
				if (!_menuWnd->IsVisible()) {
					::SetWindowPos(_menuWnd->Hwnd(), NULL, 0, 0, Width(), height, SWP_NOZORDER | SWP_NOACTIVATE);
					_menuWnd->Show();
				}
				else {
					_menuWnd->Hide();
				}
			}
			};
	}
	ComBox::ComBox()
	{
		Init();
	}
	EString ComBox::GetText()
	{
		return this->_textBox.GetText();
	}
	int_t ComBox::GetCheck()
	{
		return this->_index;
	}
	bool ComBox::SetCheck(int_t pos)
	{
		auto item = _list.GetControl(pos);
		if (item) {
			_textBox.SetText(((Label*)item)->GetText());
			_index = pos;
			return true;
		}
		return false;
	}

	ComBox::~ComBox()
	{
		_list.Clear(true);
		//涉及到由于"_list"是成员变量 释放比较靠后 所以释放窗口的时候 必须把 "_list"的公共数据也置零
		_list.PublicData = NULL;
		if (_list.GetScrollBar()) {
			//滚动条同理 也需要置零公共数据
			_list.GetScrollBar()->PublicData = NULL;
		}
		if (_menuWnd) {
			delete _menuWnd;
		}
	}
	int_t ComBox::AddItem(const EString& text)
	{
		Label* lb = new Label;
		lb->SetDockStyle(DockStyle::Horizontal);
		lb->SetText(text);
		_list.Add(lb);
		lb->HoverStyle.BackColor = Color::Gray;
		lb->HoverStyle.ForeColor = Color::White;

		lb->EventHandler = [&](Control* sd, const EventArgs& args) ->void {
			if (args.EventType == Event::OnMouseClick) {
				_index = sd->Parent->IndexOf(sd);
				_textBox.SetText(((Label*)sd)->GetText());
				_textBox.Invalidate();
				_menuWnd->Hide();
			}
			};

		return _list.GetControls().size() - 1;
	}
	void ComBox::RemoveItem(int_t index)
	{
		Control* lb = _list.GetControl(index);
		_list.Remove(lb,true);
	}
	void ComBox::OnLayout() {
		this->_UpDown.SetFixedSize(Size(Height(), Height()));
		__super::OnLayout();
	}

	ComBox::MenuContent::MenuContent(Control* ownerCtl, Control* hittestCtl) :PopupWindow(0, 0, ownerCtl), _hittestCtl(hittestCtl)
	{
	}
	void ComBox::MenuContent::OnKillFocus(HWND wnd)
	{
		if (::GetWindowOwner(Hwnd()) == wnd) {
			POINT pt;
			::GetCursorPos(&pt);
			// 将鼠标屏幕坐标转换为客户端坐标
			::ScreenToClient(::GetWindowOwner(Hwnd()), &pt);
			Rect _hittestRect = _hittestCtl->GetClientRect();
			if (_hittestRect.Contains(pt.x, pt.y)) {
				return;
			}
			else {
				this->Hide();
			}
		}
		else {
			this->Hide();
		}
	}
	ComBox::MenuContent::~MenuContent()
	{
	}
}

