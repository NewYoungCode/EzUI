#pragma once
#include "global.h"
#include "widgets.h"
#include "vlcPlayer.h"
#include "lrcControl.h"
#include "desktopLrcFrm.h"

//using Form = Window; //经典win32窗口样式
//using Form = BorderlessWindow; //无边框窗口 带windows吸附效果
using Form = LayeredWindow;  //分层窗口 支持异形 圆角

class MainFrm :
	public Form
{
private:
	std::vector<Song> songs;
	//no new 不需要释放
	NotifyIcon ntfi;
	UIManager umg;
	VlcPlayer player;
	VListView* localList = NULL, * searchList = NULL;
	TextBox* searchEdit;
	LrcControl lrcCtl;
	UIString lastFen;
	Label* time, * singer;
	TabLayout* tabCtrl, * control;
	Control* main, * center, * centerLeft, * tools, * playerBar, * playerBar2;
	CheckBox* labelDeskLrc;
	//need new 需要释放
	Timer* timer = NULL;
	ConfigIni* cfg = NULL;
	Image* bkImg = NULL, * headImg = NULL;
	Task* downloadTask = NULL;
	int lastWidth = -1;
	DesktopLrcFrm* deskTopWnd;
	UIString nowSong;
protected:
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	bool OnNotify(Control* sender, EventArgs& args)override;
	void OpenSongView();
	void TimerTick();
	void OpenLrcView();
	void InitForm();
	void DownLoadImage(UIString SingerName, UIString headImageUrl);
	bool PlayForHash(const UIString& hash, Song& info);
public:
	void NextPage(float scrollPos);
	MainFrm();
	virtual ~MainFrm();
	void OnClose(bool& b) override;
	size_t FindLocalSong(const UIString& hash);
};

