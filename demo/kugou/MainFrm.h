#pragma once
#include "global.h"
#include "VlcPlayer.h"
#include "LrcControl.h"

#include "DesktopLrcFrm.h"

using Form = BorderlessWindow;

class MainFrm :
	public Form
{
private:
	std::vector<Song> songs;
	//no new 不需要释放
	NotifyIcon ntfi;
	UIManager umg;
	VlcPlayer player;
	VList* localList = NULL, * searchList = NULL;
	TextBox* searchEdit;
	LrcControl lrcCtl;
	EString lastFen;
	Label* time, * singer;
	TabLayout* tabCtrl, * control;
	Control* main, * center, * centerLeft, * tools, * playerBar, * playerBar2;
	CheckBox* labelDeskLrc;
	//need new 需要释放
	ThreadTimer* timer = NULL;
	ConfigIni* cfg = NULL;
	Image* bkImg = NULL, * headImg = NULL;
	Task* downloadTask = NULL;
	int lastWidth = -1;
	DesktopLrcFrm* deskTopWnd;
	EString nowSong;
protected:
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	bool OnNotify(Control* sender, EventArgs& args)override;
	void OpenSongView();
	void TimerTick();
	void OpenLrcView();
	void InitForm();
	void DownLoadImage(EString SingerName, EString headImageUrl);
	bool PlayForHash(const EString& hash, Song& info);
public:
	void NextPage(float scrollPos);
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;
	size_t FindLocalSong(const EString& hash);
};

