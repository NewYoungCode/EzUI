#include "VlcPlayer.h"

using namespace ezui;

namespace {

void* lock_cb(void* opaque, void** planes)
{
    auto* player = static_cast<VlcPlayer*>(opaque);
    player->decodeMutex.lock();
    std::lock_guard<std::mutex> guard(player->imageMutex);
    if (player->playImage) {
        player->playImage->LockPixels([&](uint8_t* pixels, uint32_t) {
            *planes = pixels;
        });
    }
    return nullptr;
}

void unlock_cb(void* opaque, void*, void* const*)
{
    auto* player = static_cast<VlcPlayer*>(opaque);
    player->decodeMutex.unlock();
}

void display_cb(void* opaque, void*)
{
    auto* player = static_cast<VlcPlayer*>(opaque);
    auto weakPlayer = player->GetWeakPtr<VlcPlayer>();
    BeginInvoke([weakPlayer] {
        if (!weakPlayer.IsAlive()) {
            return;
        }
        if (weakPlayer->PlayingCallback) {
            weakPlayer->PlayingCallback(weakPlayer->playImage);
        }
        weakPlayer->Invalidate();
    });
}

void cleanup_cb(void*) {}

unsigned setup_cb(void** opaque, char* chroma, unsigned* width, unsigned* height, unsigned* pitches, unsigned* lines)
{
    auto* player = static_cast<VlcPlayer*>(*opaque);
    std::lock_guard<std::mutex> guard(player->imageMutex);
    delete player->playImage;
    player->playImage = new Image(*width, *height);

    memcpy(chroma, "RV32", 4);
    *pitches = *width * 4;
    *lines = *height;
    return 1;
}

} // namespace

VlcPlayer::VlcPlayer(Object* ownerObj) : Control(ownerObj)
{
    vlc_ = libvlc_new(0, NULL);
    player_ = libvlc_media_player_new(vlc_);
    libvlc_video_set_callbacks(player_, lock_cb, unlock_cb, display_cb, this);
    libvlc_video_set_format_callbacks(player_, setup_cb, cleanup_cb);
    Style->BackColor = Color::Black;
}

VlcPlayer::~VlcPlayer()
{
    Stop();

    if (task_) {
        task_->Wait();
        delete task_;
        task_ = nullptr;
    }

    if (player_) {
        libvlc_media_player_release(player_);
        player_ = nullptr;
    }

    if (vlc_) {
        libvlc_release(vlc_);
        vlc_ = nullptr;
    }

    std::lock_guard<std::mutex> guard(imageMutex);
    delete playImage;
    playImage = nullptr;
}

void VlcPlayer::OnBackgroundPaint(PaintEventArgs* args)
{
    __super::OnBackgroundPaint(args);

    std::lock_guard<std::mutex> guard(imageMutex);
    if (playImage) {
        playImage->SizeMode = ImageSizeMode::Fit;
        args->Graphics()->DrawImage(playImage, GetRect());
    }
}

void VlcPlayer::SetConfig() {}

void VlcPlayer::OpenPath(const UIString& filePath)
{
    if (task_) {
        task_->Wait();
        delete task_;
        task_ = nullptr;
    }

    task_ = new Task([this, filePath]() {
        Stop();
        Invoke([this, filePath]() {
            const UIString path = filePath.replace("/", "\\");
            libvlc_media_t* media = libvlc_media_new_path(vlc_, path.c_str());
            libvlc_media_parse(media);
            libvlc_media_player_set_media(player_, media);
            durationMs_ = libvlc_media_get_duration(media);
            libvlc_media_player_play(player_);
            libvlc_media_release(media);
        });
    });
}

void VlcPlayer::OpenUrl(const UIString& url)
{
    Stop();
    libvlc_media_t* media = libvlc_media_new_location(vlc_, url.c_str());
    libvlc_media_parse(media);
    libvlc_media_player_set_media(player_, media);
    durationMs_ = libvlc_media_get_duration(media);
    libvlc_media_player_play(player_);
    libvlc_media_release(media);
}

void VlcPlayer::Play()
{
    libvlc_media_player_play(player_);
}

void VlcPlayer::Pause()
{
    if (libvlc_media_player_can_pause(player_)) {
        libvlc_media_player_pause(player_);
    }
}

void VlcPlayer::Stop()
{
    if (player_) {
        libvlc_media_player_stop(player_);
    }
}

void VlcPlayer::SetVolume(int volume)
{
    libvlc_audio_set_volume(player_, volume);
}

long long VlcPlayer::Duration() const
{
    return durationMs_;
}

void VlcPlayer::SetDuration(int durationSeconds)
{
    durationMs_ = static_cast<libvlc_time_t>(durationSeconds) * 1000;
}

long long VlcPlayer::Position() const
{
    return libvlc_media_player_get_time(player_);
}

void VlcPlayer::SetPosition(float positionRatio)
{
    libvlc_media_player_set_position(player_, positionRatio);
}

libvlc_state_t VlcPlayer::GetState() const
{
    return libvlc_media_player_get_state(player_);
}
