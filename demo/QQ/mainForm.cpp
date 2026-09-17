#pragma once
#include "mainForm.h"

MainForm::MainForm() :Form()
{
	this->EnableAlphaBlending();
	this->SetResizable(true);
	this->LoadXml("res/mainForm.htm");
}

void MainForm::OnNotify(Control* sd, EventArgs* args)
{
	__super::OnNotify(sd, args);
}

void MainForm::OnClose(bool& allowClose)
{
	Application::Exit(0);
}

MainForm::~MainForm()
{
}
