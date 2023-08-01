#include "ComBox.h"
namespace EzUI {
	void ComBox::Init()
	{
		Style.Border.Color = Color(239, 239, 239);
		Style.BackColor = Color::White;
		Style.Border = 1;
		Style.Border.Color = Color(239, 239, 239);
		Style.Border.Radius = 2;
		vlist.Style.BackColor = Color::White;
		vlist.GetScrollBar()->Style.Border.Radius = 5;
		vlist.GetScrollBar()->SetFixedWidth(5);

		this->AddControl(&edit);
		this->AddControl(&label);

		label.SetText(L"_");
		//label.MouseClick = [&](Control* sender, const MouseButton, const Point&)->void {

		//	Rect _rect = GetClientRect();
		//	HWND Owner = ::GetWindowOwner(poupWnd->Hwnd());
		//	RECT OwnerRect;
		//	::GetWindowRect(Owner, &OwnerRect);
		//	auto rect = poupWnd->GetWindowRect();
		//	POINT mousePos{ _rect.X,_rect.GetBottom() };
		//	POINT clientPos = mousePos;
		//	ClientToScreen(Owner, &clientPos);
		//	int height = OwnerRect.bottom - OwnerRect.top;
		//	//::MoveWindow(poupWnd->Hwnd(), clientPos.x, clientPos.y, rect.Width, rect.Height, FALSE);
		//	//根据内容自动高度
		//	::MoveWindow(poupWnd->Hwnd(), clientPos.x, clientPos.y, rect.Width, vlist.GetContentHeight(), FALSE);
		//	poupWnd->Show(SW_SHOW);
		//};
	}
	ComBox::ComBox() {
		Init();
	}

	ComBox::~ComBox() {
		if (poupWnd) {
			poupWnd->Close();
			delete poupWnd;
			poupWnd = NULL;
		}
	}
	//void ComBox::OnLoad() {
	//	if (poupWnd == NULL) {
	//		//默认200高度
	//		poupWnd = new PoupWindow(Width(), 200, PublicData->HANDLE);
	//		poupWnd->SetLayout(&vlist);
	//	}
	//}

	void ComBox::OnLayout() {
		__super::OnLayout();
		edit.SetRect({ 0, 0, Width() - Height(), Height() });
		label.SetRect({ Width() - Height(),0, Height(), Height() });
	}

	size_t ComBox::AddItem(const EString& text) {
		Label* lb = new Label;
	/*	lb->MouseClick = [&](Control* sender, const MouseButton, const Point&)->void {
			poupWnd->Hide();
			edit.SetText(((Label*)sender)->GetText());
			Invalidate();
		};*/
		lb->SetText(text);
		//lb->TextAlign = TextAlign::MiddleLeft;
		lb->HoverStyle.BackColor = Color(208, 208, 208);
		lb->HoverStyle.ForeColor = Color::White;
		//lb->Margin.Left = 30;
		lb->SetFixedHeight(30);
		vlist.AddControl(lb);
		return lb->Index();
	}
	void ComBox::RemoveItem(size_t pos) {}

	void ComBox::OnMouseClick(MouseButton btn, const Point& pt) {
		__super::OnMouseClick(btn, pt);
		/*if (btn == MouseButton::Left) {
			poupWnd->Show(SW_SHOW);
		}*/
	}
};


