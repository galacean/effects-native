#include "include/mars_native_player.h"
#include "mars_player.h"
#include "sceneData/scene_data_decoder_new.hpp"
#ifdef ANDROID
#include "platform/android/mars_offscreen_surface.h"
#else
#include "platform/ios/mars_offscreen_surface.h"
#endif

#define WRAP(ptr) ((MarsPlayer*) ptr)

namespace mn {

class NativePlayerEventHandler : public PlayerEventHandler {
public:
    void OnEvent(int type, const std::string& msg) override {
        MLOGD("NativePlayerEventHandler %d", type);
        last_event_type_ = type;
        last_event_msg_ = msg;
    }

public:
    int last_event_type_ = -1;

    std::string last_event_msg_;
};

MarsNativePlayer* MarsNativePlayer::CreateNew(const MarsPlayerOptions& options, std::string* err) {
    if (!options.scene_data_buffer || !options.shared_egl_content) {
        if (err) {
            *err = "MarsNativePlayer invalid options";
        }
        MLOGE("MarsNativePlayer invalid options");
        return nullptr;
    }

    MarsOffscreenSurface* surface = new MarsOffscreenSurface;
    SystemSurface system_surface;
    system_surface.surface = options.shared_egl_content;
    if (!surface->Create(system_surface, options.width, options.height)) {
        delete surface;
        if (err) {
            *err = "MarsNativePlayer create surface fail";
        }
        MLOGE("MarsNativePlayer create surface fail");
        return nullptr;
    }

    MarsNativePlayer* player = new MarsNativePlayer;
    player->texture_id_ = surface->GetColorAttachment();
    
    // todo: leak;
    player->event_handler_ = new NativePlayerEventHandler;

    MarsPlayerConstructor constructor;
    constructor.surface = surface;
    constructor.auto_tick = false;
    constructor.idx = 1;
    constructor.manual_render = true;
    player->impl_ = new MarsPlayer(constructor, true);
    WRAP(player->impl_)->RegisterEventHandler((PlayerEventHandler*) player->event_handler_);
    const auto& plugins = options.plugins;
    if (!plugins.empty()) {
        for (auto iter : plugins) {
            if (!WRAP(player->impl_)->RegisterNativePlugin(iter.first, iter.second)) {
                delete player;
                MLOGE("MarsNativePlayer RegisterNativePlugin(%s) fail", iter.first.c_str());
                if (err) {
                    *err = "MarsNativePlayer RegisterNativePlugin " + iter.first + " fail";
                }
                return nullptr;
            }
        }
    }
    
    WRAP(player->impl_)->OnSurfaceCreate(nullptr);
    if (((NativePlayerEventHandler*) player->event_handler_)->last_event_type_ == EVENT_TYPE_EGL_INIT_ERROR) {
        delete player;
        if (err) {
            *err = "MarsNativePlayer OnSurfaceCreate error";
        }
        MLOGE("MarsNativePlayer OnSurfaceCreate error");
        return nullptr;
    }

    WRAP(player->impl_)->SetRepeatCount(options.repeat_count);
    player->scene_data_ = SceneDataDecoderNew::CreateNew((uint8_t*) options.scene_data_buffer);
    if (!player->scene_data_) {
        delete player;
        if (err) {
            *err = "MarsNativePlayer create sceneData fail";
        }
        MLOGE("MarsNativePlayer create sceneData fail");
        return nullptr;
    }
    WRAP(player->impl_)->SetSceneData((SceneDataDecoder*) player->scene_data_);

    return player;
}

void MarsNativePlayer::Destroy() {
    delete this;
}

bool MarsNativePlayer::Start() {
    if (started_) {
        WRAP(impl_)->Restart(0);
        return true;
    }
    started_ = true;
    WRAP(impl_)->Start();
    if (((NativePlayerEventHandler*) event_handler_)->last_event_type_ == EVENT_TYPE_EGL_INIT_ERROR) {
        MLOGE("MarsNativePlayer Start error");
        return false;
    }
    return true;
}

MarsPlayerResult MarsNativePlayer::Tick(int ms) {
    if (!started_) {
        return MarsPlayerResult::NOT_INIT;
    }
    WRAP(impl_)->Tick(ms);
    if (((NativePlayerEventHandler*) event_handler_)->last_event_type_ == EVENT_TYPE_ANIMATION_END) {
        started_ = false;
        return MarsPlayerResult::ANIMATION_FINISH;
    }
    return MarsPlayerResult::SUCCESS;
}

bool MarsNativePlayer::Pause() {
    WRAP(impl_)->Pause();
    return true;
}

bool MarsNativePlayer::Resume() {
    WRAP(impl_)->Resume();
    return true;
}

bool MarsNativePlayer::SetImageData(MarsNativeImageData* image_data) {
    if (!image_data || !image_data->buffer) {
        MLOGE("SetImageData null image data");
        return false;
    }
    uint8_t* data;
    int length;
    if (image_data->width > 0 && image_data->height > 0) {
        length = image_data->width * image_data->height * 4;
    } else if (image_data->buffer_length > 0) {
        length = image_data->buffer_length;
    } else {
        MLOGE("invalid buffer length: %d, size: %d %d", image_data->buffer_length, image_data->width, image_data->height);
        return false;
    }
    if (!image_data->need_free) {
        data = (uint8_t*) malloc(length);
        if (!data) {
            MLOGE("malloc buffer fail %d", length);
            return false;
        }
        memcpy(data, image_data->buffer, length);
    } else {
        data = (uint8_t*) image_data->buffer;
        image_data->need_free = false;
    }
    ImageRawData* raw_data;
    if (image_data->width > 0 && image_data->height > 0) {
        raw_data = new ImageRawData(data, image_data->width, image_data->height);
        raw_data->premultiply_alpha = image_data->premultiply_alpha;
    } else {
        raw_data = new ImageRawData(data, image_data->buffer_length);
    }
    ((SceneDataDecoder*) scene_data_)->SetImageData(image_data->url, raw_data);
    return true;
}

uint32_t MarsNativePlayer::GetGLTexture() {
    return texture_id_;
}

MarsNativePlayer::~MarsNativePlayer() {
    if (impl_) {
        delete WRAP(impl_);
        impl_ = nullptr;
    }
}

}

#undef WRAP
