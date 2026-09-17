#pragma once
#include <mutex>

#include "EzUI/EzUI.h"
#ifdef _WIN32
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

#include "vlc/vlc.h"

#ifdef _WIN64

#pragma comment(lib,"x64/vlc/libvlc.lib")
#pragma comment(lib,"x64/vlc/libvlc.lib")

#else

#pragma comment(lib,"x86/vlc/libvlc.lib")
#pragma comment(lib,"x86/vlc/libvlc.lib")

#endif


//封装用于播放视频的控件
using namespace ezui;
class VlcPlayer :public Control
{
private:
	libvlc_instance_t* m_vlc = NULL;
	libvlc_media_player_t* m_vlcplayer = NULL;
	libvlc_time_t m_duration = 0;
	ezui::Thread* m_task = NULL;
public:
	std::mutex mtx;
	Image* play_img = NULL;
	std::mutex play_mtx;
	unsigned int IMG_WIDTH = 0;
	unsigned int IMG_HEIGHT = 0;
	std::function<void(Image* bitmap)> PlayingCallback;
protected:
	void SetConfig();
	virtual void OnBackgroundPaint(PaintEventArgs* args) override;
public:
	VlcPlayer(Object* ownerObj = NULL);
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
