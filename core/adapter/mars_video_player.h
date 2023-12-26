//
// Created by duzening on 2023/12/8.
//

#ifndef ANDROID_MARS_VIDEO_PLAYER_H
#define ANDROID_MARS_VIDEO_PLAYER_H

#include <string>
#include <memory>

#ifdef ANDROID
#include <jni.h>
#endif

#ifndef BUILD_MARS_OSS
#include <MediaFlow/mf_video_player_engine_interface.h>
#endif

namespace mn {

class MarsVideoPlayerListener {
public:
    virtual ~MarsVideoPlayerListener() { }

    virtual bool onEvent(int what, int arg1, int arg2, const char *extra) = 0;

    virtual bool onError(int what, int arg1, int arg2, const char *desc, void *extra) = 0;

    virtual bool onSendData(int type, uint8_t *data, int size, int width, int height, int sampleRate, int channels, int sampleFmt, int64_t pts) = 0;

#ifdef ANDROID
    virtual void Release(JNIEnv* env, jobject thiz) = 0;

    virtual void AddGlobalRef(int what, jobject ref) = 0;
#endif
};
    
class MarsVideoPlayer {
public:
    MarsVideoPlayer(const std::string& name, const std::string& key);

    ~MarsVideoPlayer();

    void SetEventListener(std::shared_ptr<MarsVideoPlayerListener> listener);

    void Prepare(const std::string & url, bool rotated);

    void Start(const std::string & url, bool rotated);

    void Stop();

    void Pause();

    void Resume();

#ifdef ANDROID
    void SetSurface(void* surface, int width, int height);
#else
    void SetSurface(void* surface, void* fun);
#endif

    void SeekTo(int64_t position, int accurateSeek, int playMode, int64_t loopDuration);

    void SetHardDecoder();
    
    private:
#ifndef BUILD_MARS_OSS
    std::unique_ptr<mflow::MFVideoPlayerEngineInterface> engine_;
#else
    // todo: opensdk 版本
#endif
};

} // mn

#endif //ANDROID_MARS_VIDEO_PLAYER_H
