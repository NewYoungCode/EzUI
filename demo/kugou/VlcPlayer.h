#pragma once
#include "EzUI/Control.h"
#include "EzUI/BorderlessWindow.h"
#include "EzUI/Label.h"
#include <mutex>
#include "vlc/vlc.h"
#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")
namespace EzUI {
	//封装用于播放视频的控件
	class VlcPlayer :public Control
	{
	private:
		libvlc_instance_t* vlc_inst = NULL;
		libvlc_media_player_t* vlc_player = NULL;
		libvlc_media_t* vlc_media = NULL;
		libvlc_time_t _Duration = 0;
	public:
		std::mutex mtx;
		unsigned int IMG_WIDTH = 0;
		unsigned int IMG_HEIGHT = 0;
		Bitmap* BuffBitmap = NULL;
	protected:
		void SetConfig();
		virtual void OnBackgroundPaint(PaintEventArgs& pArg) override;
	public:
		VlcPlayer();
		virtual ~VlcPlayer();
		void OpenPath(const EString& file);
		void OpenUrl(const EString& url);
		void Play();
		void Pause();
		void Stop();
		long long Duration();
		void SetDuration(int dur);
		long long Position();
		void SetPosition(float f_pos);
		libvlc_state_t GetState();
	};
}
