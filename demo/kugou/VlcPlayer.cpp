#include "vlcPlayer.h"
void* lock_cb(void* opaque, void** planes)
{
	VlcPlayer* vp = (VlcPlayer*)opaque;
	vp->mtx.lock();
	vp->play_mtx.lock();
	vp->play_img->LockPixels([&](uint8_t* pPixels, uint32_t bitmapStride) {
		*planes = pPixels;   /*tell VLC to put decoded data to this buffer*/
		});
	vp->play_mtx.unlock();
	return NULL;
}
/*##get the argb picture AND save to file*/
void unlock_cb(void* opaque, void* picture, void* const* planes)
{
	VlcPlayer* vp = (VlcPlayer*)opaque;
	vp->mtx.unlock();
}
void display_cb(void* opaque, void* picture)
{
	VlcPlayer* vp = (VlcPlayer*)opaque;
	// 通知主线程刷新
	BeginInvoke([=] {
		if (vp->PlayingCallback) {
			vp->PlayingCallback(vp->play_img);
		}
		vp->Invalidate();
		});
}
void cleanup_cb(void* opaque)
{
	VlcPlayer* vp = (VlcPlayer*)opaque;
}

unsigned setup_cb(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
{
	int w = *width;
	int h = *height;

	VlcPlayer* vp = (VlcPlayer*)*opaque;
	vp->play_mtx.lock();
	if (vp->play_img != NULL) {
		delete vp->play_img;
		vp->play_img = NULL;
	}
	vp->play_img = new Image(w, h);
	vp->play_mtx.unlock();

	memcpy(chroma, "RV32", 4);
	*pitches = w * 4;
	*lines = h;
	return 1;
}
VlcPlayer::VlcPlayer(Object* ownerObj) :Control(ownerObj)
{
	m_vlc = libvlc_new(NULL, NULL);
	m_vlcplayer = libvlc_media_player_new(m_vlc);
	libvlc_video_set_callbacks(m_vlcplayer, lock_cb, unlock_cb, display_cb, this);
	libvlc_video_set_format_callbacks(m_vlcplayer, setup_cb, cleanup_cb);
	//设置背景色为黑色
	this->Style->BackColor = Color::Black;
}

VlcPlayer::~VlcPlayer()
{
	Stop();
	if (m_task) {
		delete m_task;
	}
	if (m_vlcplayer) {
		libvlc_media_player_release(m_vlcplayer);
	}
	libvlc_release(m_vlc);

	this->play_mtx.lock();
	if (play_img) {
		delete play_img;
		play_img = NULL;
	}
	this->play_mtx.unlock();
}
void VlcPlayer::OnBackgroundPaint(PaintEventArgs* args) {
	__super::OnBackgroundPaint(args);
	this->play_mtx.lock();
	if (this->play_img) {
		this->play_img->SizeMode = ImageSizeMode::Fit;
		args->Graphics()->DrawImage(this->play_img, GetRect());
	}
	this->play_mtx.unlock();
}
void VlcPlayer::SetConfig()
{
}
void VlcPlayer::OpenPath(const UIString& file_)
{
	if (m_task && m_task->IsFinished()) {
		delete m_task;
		m_task = NULL;
	}
	else if (m_task && !m_task->IsFinished()) {
		//上一次播放请求尚未完成
		return;
	}
	m_task = new Thread([&, file_]() {
#ifdef _DEBUG
		OutputDebugStringA("-----------------------------------------------------------stop in..\n");
#endif // _DEBUG
		Stop();
		//回到主线程进行处理
		Invoke([&]() {
			UIString file = file_.replace("/", "\\");
			libvlc_media_t* pmedia = libvlc_media_new_path(m_vlc, file.c_str());
			libvlc_media_parse(pmedia);//
			libvlc_media_player_set_media(m_vlcplayer, pmedia);
			m_duration = libvlc_media_get_duration(pmedia) / 1000;// Duration() 统一返回秒
			libvlc_media_player_play(m_vlcplayer);
			libvlc_media_release(pmedia);
			});
#ifdef _DEBUG
		OutputDebugStringA("-----------------------------------------------------------stop out..\n");
#endif // _DEBUG

		});

}
void VlcPlayer::OpenUrl(const UIString& url)
{
	Stop();
	libvlc_media_t* pmedia = libvlc_media_new_location(m_vlc, url.c_str());
	libvlc_media_parse(pmedia);//
	libvlc_media_player_set_media(m_vlcplayer, pmedia);
	m_duration = libvlc_media_get_duration(pmedia);//
	libvlc_media_player_play(m_vlcplayer);
	libvlc_media_release(pmedia);
}
void VlcPlayer::Play()
{
	libvlc_media_player_play(m_vlcplayer);
}

void VlcPlayer::SetVolume(int volume) {
	libvlc_audio_set_volume(m_vlcplayer, volume);
}

void VlcPlayer::Pause()
{
	if (libvlc_media_player_can_pause(m_vlcplayer))
	{
		libvlc_media_player_pause(m_vlcplayer);
	}
}
void VlcPlayer::Stop()
{
	libvlc_media_player_stop(m_vlcplayer);
}
long long  VlcPlayer::Duration() {
	return m_duration;
}
void VlcPlayer::SetDuration(int dur)
{
	m_duration = dur;
}
long long VlcPlayer::Position() {
	libvlc_time_t play_time = libvlc_media_player_get_time(m_vlcplayer);
	return play_time;
}

void VlcPlayer::SetPosition(float f_pos)
{
	libvlc_media_player_set_position(m_vlcplayer, f_pos);
}

libvlc_state_t VlcPlayer::GetState() {
	return libvlc_media_player_get_state(m_vlcplayer);
}
