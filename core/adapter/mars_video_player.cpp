//
// Created by duzening on 2023/12/8.
//

#include "mars_video_player.h"
#include <cassert>

namespace mn {

#ifndef BUILD_MARS_OSS
MarsVideoPlayer::MarsVideoPlayer(const std::string& name, const std::string& key)
    : engine_{mflow::MFVideoPlayerEngineInterface::Create(name, key)} { }

MarsVideoPlayer::~MarsVideoPlayer() = default;

class MarsVideoPlayerListenerWrapper: public mflow::MFVideoPlayerEngineListener {
public:
    explicit MarsVideoPlayerListenerWrapper(const std::shared_ptr<MarsVideoPlayerListener>& listener)
       : listener_(listener) {
        assert(listener != nullptr);
    }

    ~MarsVideoPlayerListenerWrapper() override = default;

    bool onEvent(int what, int arg1, int arg2, const char *extra) override {
        return listener_->onEvent(what, arg1, arg2, extra);
    }

    bool onError(int what, int arg1, int arg2, const char *desc, void *extra) override {
        return listener_->onError(what, arg1, arg2, desc, extra);
    }

    bool onSendData(int type, uint8_t *data, int size, int width,
                    int height, int sampleRate, int channels, int sampleFmt, int64_t pts) override {
        return listener_->onSendData(type, data, size, width, height, sampleRate, channels, sampleFmt, pts);
    }

#ifdef ANDROID
    void Release(JNIEnv* env, jobject thiz) override {
        listener_->Release(env, thiz);
    }

    void AddGlobalRef(int what, jobject ref) override {
        listener_->AddGlobalRef(what, ref);
    }
#endif

private:
   std::shared_ptr<MarsVideoPlayerListener> listener_;
};

void MarsVideoPlayer::SetEventListener(std::shared_ptr<MarsVideoPlayerListener> listener) {
    if (listener) {
        engine_->SetEventListener(std::make_shared<MarsVideoPlayerListenerWrapper>(listener));
    }
}

void MarsVideoPlayer::Prepare(const std::string & url, bool rotated) {
    engine_->Prepare(url, rotated);
}

void MarsVideoPlayer::Start(const std::string & url, bool rotated) {
    engine_->Start(url, rotated);
}

void MarsVideoPlayer::Stop() {
    engine_->Stop();
}

void MarsVideoPlayer::Pause() {
    engine_->Pause();
}

void MarsVideoPlayer::Resume() {
    engine_->Resume();
}

#ifdef ANDROID
void MarsVideoPlayer::SetSurface(void* surface, int width, int height) {
    engine_->SetSurface(surface, width, height);
}
#else
void MarsVideoPlayer::SetSurface(void* surface, void* fun) {
engine_->SetSurface(surface, fun);
}
#endif

void MarsVideoPlayer::SeekTo(int64_t position, int accurateSeek, int playMode, int64_t loopDuration) {
    engine_->SeekTo(position, accurateSeek, playMode, loopDuration);
}

void MarsVideoPlayer::SetHardDecoder() {
    engine_->SetHardDecoder();
}

#endif // if not def BUILD_MARS_OSS

#ifdef BUILD_MARS_OSS
MarsVideoPlayer::MarsVideoPlayer(const std::string& name, const std::string& key) {

}

MarsVideoPlayer::~MarsVideoPlayer() {

}

void MarsVideoPlayer::SetEventListener(std::shared_ptr<MarsVideoPlayerListener> listener) {

}

void MarsVideoPlayer::Prepare(const std::string & url, bool rotated) {

}

void MarsVideoPlayer::Start(const std::string & url, bool rotated) {

}

void MarsVideoPlayer::Stop() {

}

void MarsVideoPlayer::Pause() {

}

void MarsVideoPlayer::Resume() {

}

#ifdef ANDROID
void MarsVideoPlayer::SetSurface(void* surface, int width, int height) {

}
#else
void MarsVideoPlayer::SetSurface(void* surface, void* fun) {

}
#endif

void MarsVideoPlayer::SeekTo(int64_t position, int accurateSeek, int playMode, int64_t loopDuration) {

}

void MarsVideoPlayer::SetHardDecoder() {

}
#endif // if def BUILD_MARS_OSS

} // mn
