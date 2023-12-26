//
//  video_image_data.cpp
//  MarsNative
//
//  Created by changxing on 2023/9/26.
//  Copyright © 2023 Alipay. All rights reserved.
//

#include "video_image_data.hpp"
#include "adapter/mars_video_player.h"
#include <mutex>

#ifdef ANDROID
#include <set>
#include "android/video_buffer_processor.h"
#include "platform/android/jni_bridge.h"
#else
#include "ios/video_buffer_processor.hpp"
#endif

namespace mn {

class VideoPlayerEngineListener : public mn::MarsVideoPlayerListener {
public:
    VideoPlayerEngineListener(bool transparent) {
        MLOGD("VideoPlayerEngineListener %p", this);
        processor_ = new VideoBufferProcessor(transparent);
    }
    
    ~VideoPlayerEngineListener() override {
        MLOGD("~VideoPlayerEngineListener %p", this);
        MN_SAFE_DELETE(processor_);
    }
    
    void SetInvalid() {
        std::lock_guard<std::mutex> lock(state_mutex_);
        invalid_ = true;
        if (processor_) {
            processor_->ReleaseGLObject();
        }
    }
    
    void SetPrepareCallback(const VideoPrepareCallback& callback) {
        std::lock_guard<std::mutex> lock(state_mutex_);
        prepare_callback_ = callback;
    }
    
    bool onEvent(int what, int arg1, int arg2, const char *extra) override {
        std::lock_guard<std::mutex> lock(state_mutex_);
        if (invalid_) {
            MLOGE("Video onEvent invalid state, skip");
            return false;
        }
        switch (what) {
            case 1: // prepared
                MLOGD("Video prepared: width %d, height %d", arg1, arg2);
                this->width = arg1;
                this->height = arg2;
                break;

            case 5: // 播放进度
                MLOGD("Video progress %d ms", arg1);
                break;
                
            case 11: // 读到首个视频包
                MLOGD("Video FIRST_V_PACKET_RECEIVED: duration %d", arg1);
                if (prepare_callback_) {
                    prepare_callback_(true, "");
                    prepare_callback_ = nullptr;
                }
                break;
                
            case 12: // 解码首帧
                MLOGD("Video FIRST_V_FRAME_DECODED: width %d height %d", arg1, arg2);
                break;
                
            case 102: // 视频基础信息
                MLOGD("Video info: duration %d ms, start %d ms, %s", arg1, arg2, extra);
                break;
                
            case 133: //解码器初始化通知
                MLOGD("Video DECODER_OPENED: err %d type %s", arg1, extra);
                break;

            case 2006:
                MLOGD("Video MF_MSG_VOD_ENCODER_METADATA_UPDATE");
                break;
                
            default:
                MLOGD("Video onEvent %d %d %d %s", what, arg1, arg2, extra ? extra : "null");
                break;
        }
        return true;
    }

    bool onError(int what, int arg1, int arg2, const char *desc, void *extra) override {
        MLOGD("Video onError %d %d %d %s %p", what, arg1, arg2, desc ? desc : "null", extra);
        return true;
    }

    bool onSendData(int type, uint8_t *data, int size, int w, int h, int sampleRate, int channels, int sampleFmt, int64_t pts) override {
        MLOGD("Video onSendData %d %p %d %d %d %d %d %d %lld", type, data, size, w, h, sampleRate, channels, sampleFmt, pts);
        return true;
    }

#ifdef ANDROID
    // 释放全部globalRef
    void Release(JNIEnv* e, jobject thiz) override {
        MLOGD("Video Release JNI");
        if (!global_ref_set_.empty()) {
            JNIEnv* env = JNIUtil::GetEnv();
            for (auto iter : global_ref_set_) {
                jobject obj = iter;
                MLOGD("Video DeleteGlobalRef %p", obj);
                if (obj) {
                    env->DeleteGlobalRef(obj);
                }
            }
        }
    }

    // 增加一个globalRef
    void AddGlobalRef(int what, jobject ref) override {
        MLOGD("Video AddGlobalRef %d %p", what, ref);
        auto iter = global_ref_set_.find(ref);
        if (iter != global_ref_set_.end()) {
            MLOGE("already has object, do nothing!");
            return;
        }
        global_ref_set_.insert(ref);
    }
#endif
    
public:
    int width = 0;
    int height = 0;

    VideoBufferProcessor* processor_ = nullptr;
    
private:
    VideoPrepareCallback prepare_callback_ = nullptr;
    std::mutex state_mutex_;
    bool invalid_ = false;
#ifdef ANDROID
    std::set<jobject> global_ref_set_;
#endif
};

VideoContext::VideoContext(const std::string& file_path, const std::string& key) {
    is_destroyed_ = std::make_shared<bool>(false);
    file_path_ = file_path;
    key_ = key;
    MLOGD("VideoContext %p %s %s", this, key.c_str(), file_path.c_str());
}

VideoContext::~VideoContext() {
    MLOGD("~VideoContext %p", this);
    if (!*is_destroyed_) {
        *is_destroyed_ = true;
    }
    if (engine_listener_) {
        engine_listener_->SetInvalid();
        engine_listener_.reset();
    }
}

void VideoContext::Prepare(bool transparent, bool hard_decoder, const VideoPrepareCallback& callback) {
    engine_ = std::make_unique<MarsVideoPlayer>("MarsNative", key_);
    MLOGD("VideoContext.Prepare %p %p %d", this, engine_.get(), hard_decoder);
    if (hard_decoder) {
        engine_->SetHardDecoder();
    }
    engine_listener_ = std::make_shared<VideoPlayerEngineListener>(transparent);
    engine_->SetEventListener(engine_listener_);
    
    engine_listener_->SetPrepareCallback(callback);
    engine_->Prepare(file_path_, false);
}

int VideoContext::GetWidth() const {
    if (engine_listener_) {
        return engine_listener_->width;
    }
    return 0;
}

int VideoContext::GetHeight() const {
    if (engine_listener_) {
        return engine_listener_->height;
    }
    return 0;
}

bool VideoContext::RenderToFBO(int frame_buffer) {
    if (!started_) {
        if (!StartEngine()) {
            return false;
        }
        started_ = true;
    }
    if (engine_listener_->processor_ && started_) {
        return engine_listener_->processor_->RenderToFBO(frame_buffer);
    }
    MLOGE("VideoContext::RenderToFBO no valid processor %p %d", engine_listener_->processor_, started_);
    return false;
}

void VideoContext::SeekTo(int ms) {
    MLOGD("SeekTo %d", ms);
    if (!started_) {
        if (!StartEngine()) {
            return;
        }
        started_ = true;
    }
    if (engine_) {
        engine_->SeekTo(ms, false, 0, 0);
    }
}

void VideoContext::Pause() {
    if (engine_ && started_) {
        engine_->Pause();
    }
}

void VideoContext::Resume() {
    if (engine_ && started_) {
        engine_->Resume();
    }
}

void VideoContext::Stop() {
    if (engine_) {
        engine_->Stop();
    }
}

bool VideoContext::StartEngine() {
    if (engine_ && engine_listener_) {
        MLOGD("StartEngine");
#ifdef ANDROID
        int width = engine_listener_->width;
        int height = engine_listener_->height;
        ANativeWindow* window = engine_listener_->processor_->CreateSurface(width, height);
        if (window) {
            engine_->SetSurface(window, width, height);
        } else {
            MLOGE("VideoContext::RenderToFBO CreateSurface fail");
            return false;
        }
#else
        engine_->SetSurface(engine_listener_->processor_, (void*) (&VideoBufferProcessor::OnSurfaceData));
#endif
        engine_->Start(file_path_, false);
    }
    return true;
}

VideoImageRawData::VideoImageRawData(VideoContext* video_context) : ImageRawData(nullptr, 0) {
    is_video = true;
    video_context_ = video_context;
}

VideoImageRawData::~VideoImageRawData() {
}

}
