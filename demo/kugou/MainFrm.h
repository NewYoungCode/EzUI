#pragma once
#include "global.h"
#include "widgets.h"
#include "vlcPlayer.h"
#include "lrcPanel.h"
#include "desktopLrcFrm.h"

//using Form = Window; //经典win32窗口样式
//using Form = BorderlessWindow; //无边框窗口 带windows吸附效果
using Form = LayeredWindow;  //分层窗口 支持异形 圆角

class MainFrm :
	public Form
{
private:
	std::vector<Song> songLsit;
	//no new 不需要释放
	NotifyIcon ntfi;
	UIManager umg;
	VlcPlayer player;
	VListView* vlistLocal = NULL, * vlistSearch = NULL;
	TextBox* editSearch;
	LrcPanel lrcPanel;
	Label* labelTime, * labelSinger;
	TabLayout* tabCtrl, * mediaCtl;
	Control* mainLayout, * centerLayout, * centerLeft, * tools, * playerBar, * playerBar2;
	CheckBox* labelDeskLrc;
	//need new 需要释放
	Timer* timer = NULL;
	ConfigIni* listFile = NULL;
	Image* bkImg = NULL, * headImg = NULL;
	Task* downloadTask = NULL;
	DesktopLrcFrm* deskTopWnd;
	UIString nowSong;
	//1:歌曲 2:视频
	int playType = 0;
protected:
	virtual void OnClose(bool& bClose) override;
	virtual void OnKeyDown(WPARAM wparam, LPARAM lParam)override;
	virtual bool OnNotify(Control* sender, EventArgs& args)override;
	virtual void OnPaint(PaintEventArgs& args)override;
private:
	//打开/关闭桌面歌词
	void OpenDesktopLrc();
	//打开登录窗口
	void OpenLoginFrm(ezui::Control* sender);
	//上一首
	void UpSong();
	//下一首
	void NextSong();
	//播放mv
	void PlayMv(const UIString& mvhash, const UIString& songHash);
	//请求新的头像和写真
	void RequestNewImage(Song& info);
	//打开歌曲界面
	void OpenSongView();
	//计时器处理的函数
	void TimerTick();
	//打开歌词滚动界面
	void OpenLrcView();
	//初始化窗口
	void InitForm();
	//下载歌手头像和写真
	void DownLoadImage(UIString SingerName, UIString headImageUrl);
	//播放歌曲
	bool PlaySong(const UIString& hash, Song& info);
	//寻找左侧列表中的歌曲
	size_t FindLocalSong(const UIString& hash);
	//下一页
	void NextPage(float scrollPos);
public:
	MainFrm();
	virtual ~MainFrm();
};

