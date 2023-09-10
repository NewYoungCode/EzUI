#pragma once
#include "global.h"
#include "VlcPlayer.h"
#include "LrcControl.h"
#include "TabLayout.h"
#include "NotifyIcon.h"
#include "BorderlessWindow.h"
#include "LayeredWindow.h"
#include "UIManager.h"
class MainFrm :
	public BorderlessWindow
{
private:
	std::vector<Song> songs;
	LayeredWindow* deskTopWnd;
	//no new 不需要释放
	NotifyIcon ntfi;
	UIManager umg;
	VlcPlayer player;
	VList* localList = NULL, * searchList = NULL;
	TextBox* searchEdit;
	LrcControl lrcCtl;
	LrcControl* deskTopLrc;
	EString lastFen;
	Label* time, * singer;
	TabLayout* tabCtrl, * control;
	Control* main, * center, * centerLeft, * tools, * playerBar, * playerBar2;
	//need new 需要释放
	Thread::Timer* timer = NULL;
	ConfigIni* cfg = NULL;
	Image* bkImg = NULL, * headImg = NULL;
	std::future<void>* downloadTask = NULL;
	int lastWidth = -1;
	EString nowSong;
protected:
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	bool OnNotify(Control* sender, EventArgs& args)override;
	LRESULT WndProc(UINT msg, WPARAM W, LPARAM L) override;
	void NextPage(float scrollPos);
	void OpenSongView();
	void Task();
	void OpenLrcView();
	void InitForm();
	void DownLoadImage(EString SingerName, EString headImageUrl);
public:
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;
	void OnPaint(PaintEventArgs& arg)override;
	size_t FindLocalSong(const EString& hash);
};

