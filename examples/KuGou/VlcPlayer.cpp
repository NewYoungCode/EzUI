#include "VlcPlayer.h"
namespace EzUI {

	static void* lock(void* opaque, void** planes)
	{
		VlcPlayer* vp = (VlcPlayer*)opaque;
		vp->mtx.lock();
		*planes = vp->BuffBitmap->point;           /*tell VLC to put decoded data to this buffer*/
		return 0;                   /* picture identifier, not needed here */
	}
	/*##get the argb picture AND save to file*/
	static void unlock(void* opaque, void* picture, void* const* planes)
	{
		VlcPlayer* vp = (VlcPlayer*)opaque;
		vp->mtx.unlock();
	}

	static void display(void* opaque, void* picture)
	{
		VlcPlayer* vp = (VlcPlayer*)opaque;
		vp->Refresh();
		::UpdateWindow(vp->_hWnd);
	}

	VlcPlayer::VlcPlayer()
	{
		vlc_inst = libvlc_new(NULL, NULL);
	}
	VlcPlayer::~VlcPlayer()
	{
		if (vlc_player) {
			libvlc_media_player_stop(vlc_player);
			libvlc_media_player_release(vlc_player);
		}
		if (BuffBitmap) {
			delete BuffBitmap;
		}
	}
	void VlcPlayer::OnBackgroundPaint(PaintEventArgs& pArg) {
		__super::OnBackgroundPaint(pArg);
		if (BuffBitmap) {
			auto r2 = GetClientRect();
			::BitBlt(pArg.Painter.DC, r2.X, r2.Y, r2.Width, r2.Height, BuffBitmap->GetHDC(), 0, 0, SRCCOPY);
		}
	}
	void VlcPlayer::SetConfig()
	{
		libvlc_media_parse(vlc_media);//解析
		duration = libvlc_media_get_duration(vlc_media);//获取媒体时长
		if (vlc_player) {
			libvlc_media_player_stop(vlc_player);//停止播放
			libvlc_media_player_release(vlc_player);//设置播放媒体
		}
		vlc_player = libvlc_media_player_new_from_media(vlc_media);

		libvlc_video_get_size(vlc_player, 0,
			&IMG_WIDTH, &IMG_HEIGHT);
		/*	IMG_WIDTH = libvlc_video_get_width(vlc_player);
			IMG_HEIGHT = libvlc_video_get_height(vlc_player);*/

		BuffBitmap = new EBitmap(IMG_WIDTH, IMG_HEIGHT, 32);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
		libvlc_video_set_format(vlc_player, "RV32", IMG_WIDTH, IMG_HEIGHT, IMG_WIDTH * 4);


		libvlc_media_release(vlc_media);
	}
	void VlcPlayer::OpenPath(const EString& file)
	{
		vlc_media = libvlc_media_new_path(vlc_inst, file.c_str());
		SetConfig();
	}
	void VlcPlayer::OpenUrl(const EString& url)
	{
		vlc_media = libvlc_media_new_location(vlc_inst, url.c_str());//打开url
		SetConfig();
	}
	void VlcPlayer::Play()
	{
		libvlc_media_player_play(vlc_player);
	}
	void VlcPlayer::Pause()
	{
		libvlc_media_player_pause(vlc_player);
	}
	long long  VlcPlayer::Duration() {
		return duration;
	}
	long long  VlcPlayer::Position() {
		libvlc_time_t play_time = libvlc_media_player_get_time(vlc_player);
		return play_time;
	}
}
