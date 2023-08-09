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
		if ((Control*)vp->Tag) {
			((Control*)vp->Tag)->Invalidate();
		}
		else {
			vp->Invalidate();
		}
	}
	unsigned setup(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
	{
		int w = *width;
		int h = *height;
		VlcPlayer* vp = (VlcPlayer*)*opaque;
		if (vp->BuffBitmap != NULL) {
			delete vp->BuffBitmap;
		}
		vp->BuffBitmap = new EBitmap(w, h, EBitmap::PixelFormat::PixelFormatARGB);
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
		if (vlc_inst) {
			libvlc_free(vlc_inst);
		}
		if (BuffBitmap) {
			delete BuffBitmap;
		}
	}
	void VlcPlayer::OnBackgroundPaint(PaintEventArgs& pArg) {
		__super::OnBackgroundPaint(pArg);
		if (BuffBitmap) {
			Image img(BuffBitmap->_bitmap);
			img.SizeMode = ImageSizeMode::Zoom;
			//img.Offset = Rect(500,50,200,200);
			pArg.Graphics.DrawImage(&img, GetRect());
		}
	}
	void VlcPlayer::SetConfig()
	{
	}
	void VlcPlayer::OpenPath(const EString& file)
	{
		this->Stop();
		vlc_media = libvlc_media_new_path(vlc_inst, file.c_str());
		libvlc_media_parse(vlc_media);//
		_Duration = libvlc_media_get_duration(vlc_media);//
		vlc_player = libvlc_media_player_new_from_media(vlc_media);//
		libvlc_video_set_format_callbacks(vlc_player, setup, (libvlc_video_cleanup_cb)this);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
	}
	void VlcPlayer::OpenUrl(const EString& url)
	{
		this->Stop();
		vlc_media = libvlc_media_new_location(vlc_inst, url.c_str());
		libvlc_media_parse(vlc_media);//
		_Duration = libvlc_media_get_duration(vlc_media);//
		vlc_player = libvlc_media_player_new_from_media(vlc_media);//
		libvlc_video_set_format_callbacks(vlc_player, setup, (libvlc_video_cleanup_cb)this);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
	}
	void VlcPlayer::Play()
	{
		if (vlc_player) {
			int volume = 60;
			libvlc_audio_set_volume(vlc_player, volume);
			libvlc_media_player_play(vlc_player);
		}
	}
	void VlcPlayer::Pause()
	{
		if (vlc_player) {
			libvlc_media_player_pause(vlc_player);
		}
	}
	void VlcPlayer::Stop()
	{
		if (vlc_player) {
			//https://forum.videolan.org/viewtopic.php?f=32&t=147724&p=484850&hilit=libvlc_media_player_stop+bug#p484835
			libvlc_media_player_stop(vlc_player);//如果停止播放视频那么 vlc会崩溃 无法避免
			libvlc_media_player_release(vlc_player);//
			vlc_player = nullptr;
		}
		if (vlc_media) {
			libvlc_media_release(vlc_media);
			vlc_media = nullptr;
		}
	}
	long long  VlcPlayer::Duration() {
		return _Duration;
	}
	void VlcPlayer::SetDuration(int dur)
	{
		_Duration = dur;
	}
	long long  VlcPlayer::Position() {
		if (vlc_player) {
			libvlc_time_t play_time = libvlc_media_player_get_time(vlc_player);
			return play_time;
		}
		return 0;
	}

	void VlcPlayer::SetPosition(float f_pos)
	{
		if (vlc_player) {
			libvlc_media_player_set_position(vlc_player, f_pos);
		}
	}

	libvlc_state_t VlcPlayer::GetState() {
		if (!vlc_player) {
			return libvlc_state_t::libvlc_Error;
		}
		return libvlc_media_player_get_state(vlc_player);
	}
}
