#pragma once
#include "global.h"
#include "VlcPlayer.h"
#include"LrcControl.h"
#include "Timer.h"
#include "TabLayout.h"
class MainFrm :
	public Form
{
private:
	Timer* timer = NULL;
	VlcPlayer player;
	void InitForm();
	VList* localList;
	VList* searchList;
	Edit* searchEdit;
	LrcControl lrcCtl;
	Control* playerBar, * playerBar2;
	int lastWidth = -1;
	EString lastFen;
	Label* time, * singer;
	ConfigIni* cfg = new ConfigIni;
	Image* bkImage, * headImg;
	std::thread* downloadTask = NULL;
	TabLayout* tabCtrl;

	Control* main;
	Control* center;
	Control* centerLeft;
	Control* tools;

	void DownLoadImage(EString SingerName, EString headImageUrl);
protected:
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	bool OnNotify(Control* sender, const EventArgs& args)override;
	LRESULT WndProc(UINT msg, WPARAM W, LPARAM L) override;
	void NextPage(int a, int b);
	void SongView();
	void Task();
	void LrcView();
public:
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;
};

