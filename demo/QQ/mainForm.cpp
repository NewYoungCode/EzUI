#pragma once
#include "mainForm.h"

MainForm::MainForm(int width, int height) :Form(width, height)
{
	this->SetResizable(true);
	this->LoadXml("res/mainForm.htm");

	//Frame* mySessionFrame = (Frame*)this->FindControl("mySessionFrame");
	//mySessionFrame->NotifyHandler = [this](Control* sd, EventArgs& args) {
	//	//...处理事件
	//	//ezui::DefaultNotify(sd, args);
	//	this->OnNotify(sd, args);
	//	};


	for (size_t i = 0; i < 100; i++)
	{
		Invoke([=]() {
			auto* sessionList = (Control*)this->FindControl("sessionFrame")->FindControl("charList");
			UIString xml = LR"xml(<hbox class="item"> sessionId="101"
					<hbox margin="15" event="none">
						<label class="headImg" event="none"></label>
						<vbox margin-left="10px" event="none">
							<label event="none" text="EzUI交流群(%d)群" align="left" style="font-size:15px"></label>
							<label event="none" text="根网页一样的ui库一样" align="left"></label>
						</vbox>
						<label event="none" text="23:16" width="36" align="top"></label>
					</hbox>
				</hbox>)xml";

			xml = xml.format(i + 1);
			Control* ctrl = sessionList->Append(xml);
			this->Attach(ctrl);
			sessionList->Invalidate();
			});
	}

	/*new Task([this]() {

		for (size_t i = 0; i < 500; i++)
		{
			BeginInvoke([=]() {
				auto* sessionList = (Control*)this->FindControl("sessionFrame")->FindControl("charList");
				UIString xml = LR"xml(<hbox class="item"> sessionId="101"
					<hbox margin="15" event="none">
						<label class="headImg" event="none"></label>
						<vbox margin-left="10px" event="none">
							<label event="none" text="EzUI交流群(%d)群" align="left" style="font-size:15px"></label>
							<label event="none" text="根网页一样的ui库一样" align="left"></label>
						</vbox>
						<label event="none" text="23:16" width="36" align="top"></label>
					</hbox>
				</hbox>)xml";

				xml = xml.format(i + 1);
				Control* ctrl = sessionList->Append(xml);
				sessionList->Invalidate();
				});
			Sleep(50);
		}
		});

	Timer::Timeout(1000, [this]() {

		});*/
}

void MainForm::OnNotify(Control* sd, EventArgs& args)
{
	if (args.EventType == Event::OnMouseDown) {
		if (sd->GetAttribute("sessionId") == "101") {
			//do...
		}
		/*if (sd->GetAttribute("class") == "btnMenu") {
			int pos = sd->GetParent()->IndexOf(sd);
			TabLayout* tab = (TabLayout*)this->FindControl("tab");
			tab->SlideToPage(pos, SlideDirection::Horizontal, 200, 120);
			tab->Invalidate();
			return;
		}*/
	}
	__super::OnNotify(sd, args);
}

void MainForm::OnClose(bool& bClose)
{
	Application::Exit(0);//退出循环
	//bClose = false;
	//Animation* amt = new Animation(this);
	//amt->ValueChanged = [this](float value) {
	//	BeginInvoke([this, value]() {
	//		if (value <= 0.001) {
	//			Application::Exit(0);
	//		}
	//		this->Opacity = value;
	//		this->Invalidate();
	//		});
	//	};
	//this->Opacity = 1;
	//amt->SetStartValue(1);
	//amt->SetEndValue(0);
	//amt->Start(300);
}

MainForm::~MainForm()
{
}
