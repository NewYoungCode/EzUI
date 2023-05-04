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
		vp->Invalidate();
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
		if (vp->BuffBitmap == NULL) {
			vp->BuffBitmap = new EBitmap(w, h, EBitmap::PixelFormat::PixelFormatARGB);
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
			pArg.Graphics.DrawImage( &img, GetRect(), ImageSizeMode::Zoom, 0);
		}
	}
	void VlcPlayer::SetConfig()
	{

	}
	void VlcPlayer::OpenPath(const EString& file)
	{
		if (vlc_player) {
			libvlc_media_player_stop(vlc_player);//??????
			libvlc_media_player_release(vlc_player);//??????????y????
		}
		if (vlc_media) {
			libvlc_media_release(vlc_media);
		}
		vlc_media = libvlc_media_new_path(vlc_inst, file.c_str());
		libvlc_media_parse(vlc_media);//????
		_Duration = libvlc_media_get_duration(vlc_media);//???y?????

		vlc_player = libvlc_media_player_new_from_media(vlc_media);//????y??

		libvlc_video_set_format_callbacks(vlc_player, setup, (libvlc_video_cleanup_cb)this);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
	}
	void VlcPlayer::OpenUrl(const EString& url)
	{
		if (vlc_player) {
			libvlc_media_player_stop(vlc_player);//?????? vlc??bug ????????
			libvlc_media_player_release(vlc_player);//??????????y????
		}
		if (vlc_media) {
			libvlc_media_release(vlc_media);
		}
		vlc_media = libvlc_media_new_location(vlc_inst, url.c_str());
		libvlc_media_parse(vlc_media);//????
		_Duration = libvlc_media_get_duration(vlc_media);//???y?????
		vlc_player = libvlc_media_player_new_from_media(vlc_media);//????y??
		libvlc_video_set_format_callbacks(vlc_player, setup, (libvlc_video_cleanup_cb)this);
		libvlc_video_set_callbacks(vlc_player, lock, unlock, display, this);
	}
	void VlcPlayer::Play()
	{
		if (vlc_player) {
			libvlc_media_player_play(vlc_player);
		}
	}
	void VlcPlayer::Pause()
	{
		if (vlc_player) {
			libvlc_media_player_pause(vlc_player);
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
			//???????? ???y???????????? ?????зр?
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
