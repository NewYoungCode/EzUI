#include "stdafx.h"
#include "MainFrm.h"

MainFrm::MainFrm(int width, int height) :Window(width, height)
{
	SetLayout(&mainLayout);
	mainLayout.Style.BackgroundColor = Color::White;

	ui = new UIWindow(750, 500);

	UIManager* umg = new UIManager;
	umg->LoadFile("xml/main.htm");
	umg->SetupUI(ui);

	left.SetFixedWidth(150);
	left.Style.BorderRight = 1;
	left.Style.BorderColor = Color::Gray;
	//left.Style.BackgroundColor = Color::Pink;

	std::list<EString> controlList;

	controlList.push_back("VList");
	controlList.push_back("Label");
	controlList.push_back("Button");
	controlList.push_back("HList");
	controlList.push_back("TextBox");

	for (auto& it : controlList) {
		Label* ctl = new Label;
		ctl->SetFixedHeight(30);
		ctl->Dock = DockStyle::Horizontal;
		ctl->HoverStyle.BackgroundColor = Color(20, 10, 10, 10);
		ctl->ActiveStyle.BackgroundColor = Color(20, 10, 10, 10);
		ctl->ActiveStyle.FontSize = 11;
		ctl->SetAttribute("ctlName", it);
		ctl->Name = "ctl";
		ctl->SetText(it);
		left.AddControl(ctl);
	}


	right.SetFixedWidth(200);
	right.Style.BorderLeft = 1;
	right.Style.BorderColor = Color::Gray;

	mainLayout.AddControl(&left);
	mainLayout.AddControl(new Spacer);
	mainLayout.AddControl(&right);

}
void MainFrm::OnLoad() {
	::SetParent(ui->Hwnd(), this->Hwnd());
	auto rect = ui->GetClientRect();

	int x = (GetClientRect().Width - rect.Width) / 2;
	int y = (GetClientRect().Height - rect.Height) / 2;
	::MoveWindow(ui->Hwnd(), x, y, rect.Width, rect.Height, FALSE);
	ui->Show();
}

bool MainFrm::OnNotify(Control* sender, EventArgs& arg) {

	if (sender->Name == "ctl" && arg.EventType == Event::OnMouseUp) {
		EString ctlName = sender->GetAttribute("ctlName");
		if (ctlName == "Label") {
			POINT p1{ 0 };
			::GetCursorPos(&p1);
			::ScreenToClient(ui->Hwnd(), &p1);
			Point point{ p1.x,p1.y };
			Control* test = ui->FindControl(point, &point);
			Label* lb = new Label;
			lb->SetText(ctlName);
			lb->SetSize({ 100,30 });
			lb->SetLocation({ point.X - lb->Width() / 2 ,point.Y - lb->Height() / 2 });
			test->AddControl(lb);
			ui->Invalidate();
		}
	}
	return __super::OnNotify(sender, arg);
}

void MainFrm::OnClose(bool& cal) {
	Application::exit(0);
}