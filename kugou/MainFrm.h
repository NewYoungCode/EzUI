#pragma once
#include "global.h"
#include "VlcPlayer.h"
#include "LrcControl.h"
#include "Timer.h"
#include "TabLayout.h"
#include "NotifyIcon.h"

class MainFrm :
	public Form
{
private:
	//no new
	NotifyIcon ntfi;
	UIManager umg;
	VlcPlayer player;
	VList* localList = NULL, * searchList=NULL;
	TextBox* searchEdit;
	LrcControl lrcCtl;
	EString lastFen;
	Label* time, * singer;
	TabLayout* tabCtrl, * control;
	Control* main, * main2, * center, * centerLeft, * tools, * playerBar, * playerBar2;
	//need new 
	Thread::Timer* timer = NULL;
	ConfigIni* cfg = NULL;
	Image* playingImage = NULL, * pauseImage = NULL;
	Image* bkImage = NULL, * headImg = NULL;
	std::future<void>* downloadTask = NULL;
	int lastWidth = -1;
	//TextArea textArea;
protected:
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	bool OnNotify(Control* sender, EventArgs& args)override;
	LRESULT WndProc(UINT msg, WPARAM W, LPARAM L) override;
	void NextPage(int a, int b);
	void SongView();
	void Task();
	void LrcView();
	void InitForm();
	void DownLoadImage(EString SingerName, EString headImageUrl);
	void OnPaint(PaintEventArgs& arg);
public:
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;
};

