#pragma once
#include "global.h"
#include "VlcPlayer.h"
#include "LrcControl.h"
#include "Timer.h"
#include "TabLayout.h"
class MainFrm :
	public BorderlessWindow
{
private:
	UIManager umg;
	VlcPlayer player;
	Windows::Timer* timer = NULL;
	void InitForm();
	VList* localList = NULL;
	VList* searchList = NULL;
	TextBox* searchEdit = NULL;
	LrcControl lrcCtl;
	Control* playerBar = NULL, * playerBar2 = NULL;
	int lastWidth = -1;
	EString lastFen;
	Label* time = NULL, * singer = NULL;
	ConfigIni* cfg = new ConfigIni;
	Image* bkImage = NULL, * headImg = NULL;
	TabLayout* tabCtrl;
	TabLayout* control;
	Image* playingImage = NULL, * pauseImage = NULL;
	Control* main;
	Control* center;
	Control* centerLeft;
	Control* tools;
	std::future<void>* downloadTask = NULL;
	void DownLoadImage(EString SingerName, EString headImageUrl);
protected:
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	bool OnNotify(Control* sender, EventArgs& args)override;
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

