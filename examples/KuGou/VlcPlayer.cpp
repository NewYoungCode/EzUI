#include "VlcPlayer.h"
namespace EzUI {
	void* lock(void* opaque, void** planes)
	{
		VlcPlayer* vp = (VlcPlayer*)opaque;
		vp->mtx.lock();
		*planes = vp->BuffBitmap->point;           /*tell VLC to put decoded data to this buffer*/
		return NULL;
	}
	/*##get the argb picture AND save to file*/
	void unlock(void* opaque, void* picture, void* const* planes)
	{
		VlcPlayer* vp = (VlcPlayer*)opaque;
		vp->mtx.unlock();
	}

	void display(void* opaque, void* picture)
	{
		VlcPlayer* vp = (VlcPlayer*)opaque;
		vp->Refresh();
		//::UpdateWindow(vp->_hWnd);//应该立即绘制
	}
	unsigned setup(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
	{
		int w = *width;
		int h = *height;
		VlcPlayer* vp = (VlcPlayer*)*opaque;
		if (vp->BuffBitmap == NULL) {
			vp->BuffBitmap = new EBitmap(w, h, 32);
		}
		memcpy(chroma, "RV32", 4);
		*pitches = w * 4;
		*lines = h;
		return 1;
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
		if (vlc_media) {
			libvlc_media_release(vlc_media);
		}
		if (BuffBitmap) {
			delete BuffBitmap;
		}
	}
	void VlcPlayer::OnBackgroundPaint(PaintEventArgs& pArg) {
		__super::OnBackgroundPaint(pArg);
		if (BuffBitmap) {
			auto bitMap = Gdiplus::Bitmap::FromBITMAPINFO(&BuffBitmap->bmi, BuffBitmap->point);
			pArg.Painter.DrawImage(bitMap, GetRect());
			delete bitMap;
		}
	}
	void VlcPlayer::SetConfig()
	{

	}
	void VlcPlayer::OpenPath(const EString& file)
	{
		if (vlc_player) {
			libvlc_media_player_stop(vlc_player);//停止播放
			libvlc_media_player_release(vlc_player);//释放之前的播放媒体器
		}
		if (vlc_media) {
			libvlc_media_release(vlc_media);
		}
		vlc_media = libvlc_media_new_path(vlc_inst, file.c_str());
		libvlc_media_parse(vlc_media);//解析
		duration = libvlc_media_get_duration(vlc_media);//获取媒体时长
		vlc_player = libvlc_media_player_new_from_media(vlc_media);//设置媒体
		libvlc_video_set_format_callbacks(vlc_player, setup, (libvlc_video_cleanup_cb)this);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
	}
	void VlcPlayer::OpenUrl(const EString& url)
	{
		if (vlc_player) {

			libvlc_media_player_stop(vlc_player);//停止播放
			libvlc_media_player_release(vlc_player);//释放之前的播放媒体器
		}
		if (vlc_media) {
			libvlc_media_release(vlc_media);
		}
		vlc_media = libvlc_media_new_location(vlc_inst, url.c_str());
		libvlc_media_parse(vlc_media);//解析
		duration = libvlc_media_get_duration(vlc_media);//获取媒体时长
		vlc_player = libvlc_media_player_new_from_media(vlc_media);//设置媒体
		libvlc_video_set_format_callbacks(vlc_player, setup, (libvlc_video_cleanup_cb)this);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);

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
