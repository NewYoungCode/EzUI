#include "ComBox.h"

namespace EzUI {
	void ComBox::Init()
	{
		this->_textBox.ReadOnly = true;
		this->Add(&_textBox);
	}
	ComBox::ComBox()
	{
		Init();
	}
	EString ComBox::GetText()
	{
		return this->_textBox.GetText();
	}
	int ComBox::SelectedIndex()
	{
		return this->_index;
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
		if (_wnd) {
			delete _wnd;
		}
	}
	int ComBox::AddItem(const EString& text)
	{
		Label* lb = new Label;
		lb->SetDockStyle(DockStyle::Horizontal);
		lb->SetText(text);
		_list.Add(lb);
		return _list.GetControls().size() - 1;
	}
	void ComBox::RemoveItem(int index)
	{
		Control* lb = _list.GetControl(index);
		_list.Remove(lb);
		delete lb;
	}
	void ComBox::OnLayout() {
		_hittestRect = Rect(Width() - Height(), 0, Height(), Height());
		Rect rect(Point(1 * this->GetScale() + 0.5, 0), Size(Width() - Height(), Height()));
		this->_textBox.SetRect(rect);
		__super::OnLayout();
	}
	void ComBox::OnMouseClick(const MouseEventArgs& args)
	{
		__super::OnMouseClick(args);
		if (args.Button == MouseButton::Left && _hittestRect.Contains(args.Location)) {
			if (_wnd == NULL) {
				_wnd = new MenuContent(this);
				_list.Style.BackColor = Color::White;
				_wnd->SetLayout(&_list);
			}

			for (auto& it : _list.GetControls()) {
				it->SetFixedHeight(Height());
			}

			int height = this->Height() * _list.GetControls().size();
			if (height == 0) {
				height = Height();
			}

			::SetWindowPos(_wnd->Hwnd(), NULL, 0, 0, Width(), height, SWP_NOZORDER | SWP_NOACTIVATE);

			if (!_wnd->IsVisible() ) {
				_wnd->Show();
			}
			else {
				_wnd->Hide();
			}
		}
	}
	void ComBox::OnPaint(PaintEventArgs& args)
	{
		auto fSzie = this->_textBox.GetFontSize() * 0.5f;
		int width = fSzie * 1.5f;

		Rect rect(Width() - Height(), 0, width, fSzie);
		rect.Y = (Height() - fSzie) / 2.0f;
		rect.X += (Height() - width) / 2.0f;

		args.Graphics.SetColor(Color::Red);
		//args.Graphics.DrawRectangle(rect);

		PointF p1(rect.GetLeft(), rect.Y);
		PointF p2(rect.GetRight(), rect.Y);

		PointF p3(rect.GetLeft() + width / 2.0f, rect.GetBottom());

		args.Graphics.SetColor(this->GetForeColor());
		args.Graphics.DrawLine(p1, p3);
		args.Graphics.DrawLine(p2, p3);

		__super::OnPaint(args);
	}
	ComBox::MenuContent::MenuContent(Control* ownerCtl) :PopupWindow(0, 0, ownerCtl)
	{
	}
	void ComBox::MenuContent::OnKillFocus(HWND wnd)
	{
		if (::GetWindowOwner(Hwnd()) == wnd) {
			//return;
		}
		this->Hide();
	}
	ComBox::MenuContent::~MenuContent()
	{
	}
}

