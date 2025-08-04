#include "ComboBox.h"

namespace ezui {
	Control* ComboBox::Add(Control* childCtl)
	{
		return __super::Add(childCtl);
	}
	void ComboBox::Remove(Control* childCtl, bool freeCtrl)
	{
		__super::Remove(childCtl, freeCtrl);
	}
	void ComboBox::Init()
	{
		this->m_textBox.ReadOnly = true;
		this->Add(&m_textBox);
		this->Add(&m_UpDown);

		m_UpDown.EventHandler = [&](Control* sd, EventArgs& arg)->void {
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
			else if (arg.EventType == Event::OnMouseDown/*&& args.Button == MouseButton::Left*/) {
				//单击
				if (m_menuWnd == NULL) {
					m_menuWnd = new MenuContent(this, &m_UpDown);
					m_list.Style.BackColor = Color::White;
					m_menuWnd->SetLayout(&m_list);
				}
				for (auto& it : m_list.GetControls()) {
					it->SetFixedHeight(Height());
				}
				int_t height = this->Height() * m_list.GetControls().size();
				if (height == 0) {
					height = Height();
				}
				if (!m_menuWnd->IsVisible()) {
					::SetWindowPos(m_menuWnd->Hwnd(), NULL, 0, 0, Width(), height, SWP_NOZORDER | SWP_NOACTIVATE);
					m_menuWnd->Show();
				}
				else {
					m_menuWnd->Hide();
				}
			}
			};
	}
	ComboBox::ComboBox(Object* parentObject) :HLayout(parentObject)
	{
		Init();
	}
	UIString ComboBox::GetText()
	{
		return this->m_textBox.GetText();
	}
	int_t ComboBox::GetCheck()
	{
		return this->m_index;
	}
	bool ComboBox::SetCheck(int_t pos)
	{
		auto item = m_list.GetControl(pos);
		if (item) {
			m_textBox.SetText(((Label*)item)->GetText());
			m_index = pos;
			return true;
		}
		return false;
	}

	ComboBox::~ComboBox()
	{
		m_list.Clear(true);
		//涉及到由于"_list"是成员变量 释放比较靠后 所以释放窗口的时候 必须把 "_list"的公共数据也置零
		m_list.SetOwnerHwnd(NULL);
		if (m_list.GetScrollBar()) {
			//滚动条同理 也需要置零公共数据
			m_list.GetScrollBar()->SetOwnerHwnd(NULL);
		}
		if (m_menuWnd) {
			delete m_menuWnd;
		}
	}
	int_t ComboBox::AddItem(const UIString& text)
	{
		Label* lb = new Label;
		lb->SetDockStyle(DockStyle::Horizontal);
		lb->SetText(text);
		m_list.Add(lb);
		lb->HoverStyle.BackColor = Color::Gray;
		lb->HoverStyle.ForeColor = Color::White;

		lb->EventHandler = [&](Control* sd, const EventArgs& args) ->void {
			if (args.EventType == Event::OnMouseDown) {
				m_index = sd->Parent->IndexOf(sd);
				m_textBox.SetText(((Label*)sd)->GetText());
				m_textBox.Invalidate();
				m_menuWnd->Hide();
			}
			};

		return m_list.GetControls().size() - 1;
	}
	void ComboBox::RemoveItem(int_t index)
	{
		Control* lb = m_list.GetControl(index);
		m_list.Remove(lb, true);
	}
	void ComboBox::OnLayout() {
		this->m_UpDown.SetFixedSize(Size(Height(), Height()));
		__super::OnLayout();
	}

	ComboBox::MenuContent::MenuContent(Control* ownerCtl, Control* hittestCtl) :PopupWindow(0, 0, ownerCtl), m_hittestCtl(hittestCtl)
	{
	}
	void ComboBox::MenuContent::OnKillFocus(HWND wnd)
	{
		if (::GetWindow(Hwnd(),GW_OWNER) == wnd) {
			POINT pt;
			::GetCursorPos(&pt);
			// 将鼠标屏幕坐标转换为客户端坐标
			::ScreenToClient(::GetWindow(Hwnd(), GW_OWNER), &pt);
			Rect _hittestRect = m_hittestCtl->GetClientRect();
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
	ComboBox::MenuContent::~MenuContent()
	{
	}
}

