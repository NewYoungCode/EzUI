#pragma once
#include "global.h"
#include "widgets.h"
#include "vlcPlayer.h"
#include "lrcPanel.h"
#include "desktopLrcForm.h"

//using Form = Window; //经典win32窗口样式
using Form = BorderlessWindow; //无边框窗口 带windows吸附效果
//using Form = LayeredWindow;  //分层窗口 支持异形 圆角

//播放器主窗口
class MainFrm :
	public Form
{
private:
	//========== UI 控件指针（由 XML 加载，无需手动释放）==========
	Frame* titleFrame = nullptr;
	Frame* centerFrame = nullptr;
	Frame* bottomFrame = nullptr;
	VlcPlayer* player = nullptr;
	VListView* vlistLocal = nullptr;
	VListView* vlistSearch = nullptr;
	TextBox* editSearch = nullptr;
	LrcPanel* lrcPanel = nullptr;
	Label* labelTime = nullptr;
	Label* labelSinger = nullptr;
	TabControl* tabCtrl = nullptr;
	TabControl* mediaCtl = nullptr;
	Control* mainLayout = nullptr;
	Control* centerLayout = nullptr;
	Control* centerLeft = nullptr;
	Control* tools = nullptr;
	Slider* playerBar = nullptr;
	CheckBox* labelDeskLrc = nullptr;

	//========== 需要手动管理的资源==========
	TrayIcon ntfi;
	Timer* timer = nullptr;
	IniConfig* listFile = nullptr;
	Task* downloadTask = nullptr;
	DesktopLrcFrm* deskTopWnd = nullptr;

	//========== 状态数据 ==========
	UIString nowSong;
	std::vector<Song> songLsit;
	int playType = 0;  // 1:歌曲 2:视频
	int searchPage_ = 0;
	int searchToken_ = 0;
	int playToken_ = 0;
	bool hasMoreSearchResults_ = false;
	bool isSearchLoading_ = false;

protected:
	void OnClose(bool& bClose) override;
	void OnShow() override;
	void OnKeyDown(WPARAM wparam, LPARAM lParam) override;
	void OnNotify(Control* sender, EventArgs* args) override;
	void OnPaint(PaintEventArgs* args) override;

private:
	//========== 初始化函数 ==========
	void InitForm();
	void InitTrayIcon();
	void InitControls();
	void InitLocalPlaylist();
	void InitEventHandlers();
	void InitTimer();

	//========== 播放控制 ==========
	void PlaySong(const UIString& hash);
	void PlayMv(const UIString& mvhash, const UIString& songHash);
	void UpSong();
	void NextSong();
	void TimerTick();

	//========== 搜索相关 ==========
	void SearchSongs(const ezui::UIString& keyword);
	void NextPage(float scrollPos);
	size_t FindLocalSong(const UIString& hash);

	//========== 界面切换 ==========
	void OpenSongView();
	void OpenLrcView();
	bool OpenDesktopLrc();
	void OpenLoginFrm(ezui::Control* sender);

	//========== 资源管理 ==========
	void RequestNewImage(const Song& info);
	void ClearImages();

public:
	MainFrm();
	~MainFrm() override;
};

