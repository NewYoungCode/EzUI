#pragma once
#include "EzUI/Control.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/Label.h"
#include "EzUI/Task.h"
#include <mutex>
#ifdef _WIN32
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif
#include "vlc/vlc.h"
#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")

//封装用于播放视频的控件
using namespace ezui;
class VlcPlayer :public Control
{
private:
	libvlc_instance_t* m_vlc = NULL;
	libvlc_media_player_t* m_vlcplayer = NULL;
	libvlc_time_t m_duration = 0;
	Task * m_task = NULL;
public:
	Mutex mtx;
	unsigned int IMG_WIDTH = 0;
	unsigned int IMG_HEIGHT = 0;
	Bitmap* BuffBitmap = NULL;
	std::function<void(Bitmap* bitmap)> PlayingCallback;
protected:
	void SetConfig();
	virtual void OnBackgroundPaint(PaintEventArgs& args) override;
public:
	VlcPlayer();
	virtual ~VlcPlayer();
	void OpenPath(const UIString& file);
	void OpenUrl(const UIString& url);
	void Play();
	void SetVolume(int volume);
	void Pause();
	void Stop();
	long long Duration();
	void SetDuration(int dur);
	long long Position();
	void SetPosition(float f_pos);
	libvlc_state_t GetState();
};
