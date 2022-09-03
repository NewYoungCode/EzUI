#pragma once
#include "Control.h"
#include  "Form.h"
#include "Label.h"
#include <mutex>

#include "vlc/vlc.h"
#pragma comment(lib,"libvlc.lib")
#pragma comment(lib,"libvlccore.lib")
namespace EzUI {
	class VlcPlayer :public Label
	{
	private:
		libvlc_instance_t* vlc_inst = NULL;
		libvlc_media_player_t* vlc_player = NULL;
		libvlc_media_t* vlc_media = NULL;
		long long duration = 0;
	public:
		std::mutex mtx;

		std::mutex mtx2;
		unsigned int IMG_WIDTH = 0;
		unsigned int IMG_HEIGHT = 0;
		EBitmap* BuffBitmap = NULL;
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
		long long Duration();
		long long Position();
		libvlc_state_t GetState();
	};
}
