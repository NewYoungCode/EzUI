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

class VlcPlayer : public ezui::Control
{
public:
    VlcPlayer(ezui::Object* ownerObj = nullptr);
    ~VlcPlayer() override;

    void OpenPath(const ezui::UIString& file);
    void OpenUrl(const ezui::UIString& url);
    void Play();
    void Pause();
    void Stop();
    void SetVolume(int volume);
    long long Duration() const;
    void SetDuration(int durationSeconds);
    long long Position() const;
    void SetPosition(float positionRatio);
    libvlc_state_t GetState() const;

    std::function<void(ezui::Image* bitmap)> PlayingCallback;

    std::mutex decodeMutex;
    std::mutex imageMutex;
    ezui::Image* playImage = nullptr;

protected:
    void OnBackgroundPaint(ezui::PaintEventArgs* args) override;

private:
    void SetConfig();

    libvlc_instance_t* vlc_ = nullptr;
    libvlc_media_player_t* player_ = nullptr;
    libvlc_time_t durationMs_ = 0;
    ezui::Task* task_ = nullptr;
};
