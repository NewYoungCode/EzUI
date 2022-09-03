#pragma once
#include "global.h"
#include "VlcPlayer.h"
#include"LrcControl.h"
class MainFrm :
	public Form
{
private:
	VlcPlayer player;
	void InitForm();
	VList* localList;
	VList* searchList;
	Edit* searchEdit;
	LrcControl lrcCtl;
protected:
	Image* bkImage = NULL;
	virtual void OnKeyDown(WPARAM wparam)override;
	bool OnNotify(Control* sender, const EventArgs& args)override;
	void OnPaint(HDC hdc, const Rect& rect) override;
	void NextPage(int a, int b);
	void SongView();
	void OnTimer() override;
	void LrcView();
public:
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;

};

