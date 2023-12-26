#include "mars_player.h"
#include "ri/render/mars_renderer.hpp"
#include "ri/render/render_frame.hpp"
#include "ri/render_base.hpp"
#include "plugin/sprite/sprite_mesh.h"
#include "util/util.hpp"
#include "util/log_util.hpp"
#include "util/image_loader.h"
#include "load/loader.h"
#include "video/mars_video_texture.hpp"

namespace mn {

MarsPlayer::MarsPlayer(const MarsPlayerConstructor& options, bool native_tick) : native_tick_(native_tick) {
    MLOGD("MarsPlayer Construct");
    if (!options.manual_render) {
        ticker_ = new Ticker(options.fps);
        if (options.auto_tick) {
            ticker_->AddTicker(this);
        }
        ticker_->Start(options.config, options.lock_tick_event);
    }
    surface_ = options.surface;
    player_id_ = options.idx;
    MLOGD("lock_tick_event: %d", options.lock_tick_event);

    plugin_system_ = new PluginSystem();
    render_level_ = options.config.render_level;
    fix_tick_time_ = options.config.fix_tick_time;
}

MarsPlayer::~MarsPlayer() {
    MLOGD("MarsPlayer Destruct");
    std::unique_lock<std::mutex> lock(ticker_mutex_);
    MN_SAFE_DELETE(ticker_);

    // 停一下所有视频，规避mediaflow内部pthread_join失败引起的闪退
    for (auto& video : videos_) {
        video->Stop();
    }

    for (auto comp : compositions_) {
        MN_SAFE_DELETE(comp);
    }
    MN_SAFE_DELETE(renderer_);
    MN_SAFE_DELETE(clear_frame_);
    MN_SAFE_DELETE(scene_data_);
    MN_SAFE_DELETE(plugin_system_);
    MN_SAFE_DELETE(surface_);
    videos_.clear();
}

void MarsPlayer::Init() {
    if (inited_) {
        return;
    }
    MLOGD("MarsPlayer::Init");
    renderer_ = new MarsRenderer(surface_);

    {
        std::string msg = renderer_->InnerRenderer()->GPUCapability()->SupportCompressedTexture() ? "true_" : "false_";
        msg = msg.append(std::to_string(surface_->GetGLESVersion()));
        EmitEvent(EVENT_TYPE_STATISTICS, msg);
    }

    RenderFrameOptions rf_opt;
    rf_opt.clear_action = std::make_shared<RenderPassClearAction>();
    rf_opt.clear_action->stencil_action = true;
    rf_opt.clear_action->clear_stencil = 0;
    rf_opt.clear_action->depth_action = true;
    rf_opt.clear_action->clear_depth = 1;
    rf_opt.clear_action->color_action = true;
    rf_opt.clear_action->clear_color[0] = 0;
    rf_opt.clear_action->clear_color[1] = 0;
    rf_opt.clear_action->clear_color[2] = 0;
    rf_opt.clear_action->clear_color[3] = 0;
    
    clear_frame_ = new RenderFrame(renderer_, rf_opt);

    SpriteMesh::SetSpriteMeshMaxItemCountByGPU(renderer_->InnerRenderer()->GPUCapability());
    
    inited_ = true;
}

void MarsPlayer::SetRepeatCount(int repeat_count) {
    MLOGD("MarsPlayer::SetRepeatCount %d", repeat_count);
    repeat_count_ = repeat_count;
    if (compositions_.size() > 0) {
        compositions_[0]->SetRepeatCount(repeat_count_);
    }
}

void MarsPlayer::SetSceneData(SceneDataDecoder* scene_data) {
    MLOGD("MarsPlayer::SetSceneData %p", scene_data);
    scene_data_ = scene_data;
    plugin_system_->InitPlugins();
    // 获取动画时长
    if (scene_data_ && (!scene_data_->compositions_.empty())) {
        duration_ms_ = scene_data_->compositions_[0]->duration * 1000.0f;
    }
}

void MarsPlayer::Start() {
    if (!surface_->IsValid()) {
        MLOGE("MarsPlayer::Start surface invalid");
        return;
    }
    if (!renderer_) {
        MLOGE("MarsPlayer::Start renderer invalid");
        return;
    }
    surface_->MakeCurrent();
    if (!CreateTextureContainers()) {
        EmitEvent(EVENT_TYPE_EGL_INIT_ERROR, "decode image fail");
        MLOGE("MarsPlayer::Start image invalid");
        return;
    }

    Composition* composition = InitializeComposition();

    // todo: 多个composition （目前没有这种情况）
    // todo: player复用 （目前没有这种情况）

    compositions_.push_back(composition);
    MLOGD("MarsPlayer::Start composition size:%d", compositions_.size());

    start_ = true;
if (native_tick_) {
    stoped_by_player_ = false;
}
    this->Resume();
    if (start_time_ > 0) {
        Restart(start_time_);
    }
    Tick(0);

    ticker_->OnPlayStart();
    // todo: forwardCompositionTime & renderFrame
    EmitEvent(EVENT_TYPE_START, "");
}

// todo:适配lottiePlayer
void MarsPlayer::Restart(int start_time) {
    MLOGD("MarsPlaer::Restart");
    current_time_ = start_time;
if (native_tick_) {
    stoped_by_player_ = false;
}
    
    if (compositions_.size()) {
//        clear_frame_->Render();
        for (int i = 0; i < compositions_.size(); i++) {
            compositions_[i]->Resume();
            compositions_[i]->Restart(start_time);
        }
    }
}

// 临时适配五福场景调用，只停Tick调用，不停线程
void MarsPlayer::Stop() {
if (native_tick_) {
    MLOGD("MarsPlaer::Stop");
    if (start_ && (!stoped_by_player_)) {
        EmitEvent(EVENT_TYPE_ANIMATION_END, play_token_);
    }
    stoped_by_player_ = true;
}
}

void MarsPlayer::Pause() {
    std::unique_lock<std::mutex> lock(ticker_mutex_);
    if (ticker_) {
        ticker_->Pause();
    }
    for (Composition* comp : compositions_) {
        comp->Pause();
    }
}

void MarsPlayer::Resume() {
    std::unique_lock<std::mutex> lock(ticker_mutex_);
    if (ticker_) {
        ticker_->Resume();
    }
    for (Composition* comp : compositions_) {
        comp->Resume();
    }
}

void MarsPlayer::OnSurfaceCreate(void* surface) {
    if (!surface_->OnSurfaceCreated(surface)) {
        EmitEvent(EVENT_TYPE_EGL_INIT_ERROR, MN_ERROR_EGL_CREATE_CONTEXT);
        return;
    }
    Init();

    renderer_->Resize();
    if (surface) {
        for (Composition* comp : compositions_) {
            comp->render_frame_->OnRendererResize();
        }
    }

    this->TickInternal(33, false);
}

void MarsPlayer::OnSurfaceResize(int width, int height) {
    if (surface_) {
        surface_->OnSurfaceResize(width, height);
    }
    if (!renderer_) {
        MLOGE("OnSurfaceResize fail: no renderer");
        return;
    }
    renderer_->Resize();
    for (Composition* comp : compositions_) {
        comp->render_frame_->OnRendererResize();
    }
}

void MarsPlayer::Tick(int dt) {
    if (surface_ && surface_->IsSurfaceless()) {
        // MLOGD("ignore surfaceless tick");
        return;
    }
    TickInternal(dt, fix_tick_time_);
    if (!glerror_uploaded_) {
        if (glGetError()) {
            glerror_uploaded_ = true;
            EmitEvent(EVENT_TYPE_RUNTIME_ERROR, "glError");
        }
    }
}

void MarsPlayer::TickToFrame(int frame_idx, int frame_duration) {
    if (surface_ && surface_->IsSurfaceless()) {
        // MLOGD("ignore surfaceless tick");
        return;
    }
    int dt = frame_idx * frame_duration - current_time_;
    if (dt < 0) {
        dt = 0;
    }
    TickInternal(dt, false);
    if (!glerror_uploaded_) {
        if (glGetError()) {
            glerror_uploaded_ = true;
            EmitEvent(EVENT_TYPE_RUNTIME_ERROR, "glError");
        }
    }
}

void MarsPlayer::SetFrameControl(int from_frame, int to_frame, int frame_duration) {
if (native_tick_) {
    start_time_ = from_frame * frame_duration;
    end_time_ = to_frame * frame_duration;
    if (end_time_ > duration_ms_) { // 防止end_time超过动画时长
        MLOGD("MarsPlayer::SetFrameControl reset end_time %d -> %d", end_time_, duration_ms_);
        end_time_ = duration_ms_;
    }
}
}

void MarsPlayer::TickInternal(int dt, bool fix_time) {
    bool canTick = start_;
if (native_tick_) {
    canTick = start_ && (!stoped_by_player_);
}
    if (canTick) {
        if (dt > 33 && fix_time) {
            dt = 33;
        }
        current_time_ += dt;

if (native_tick_) {
        if (end_time_ > 0 && current_time_ > end_time_) {
            // MLOGD("cxdebug 11111 %d %d %d %d %d", end_time_, start_time_, current_time_, repeat_count_, stoped_by_player_);
            if (repeat_count_ > 0 || repeat_count_ == -1) {
                Restart(start_time_);
                if (repeat_count_ > 0) {
                    repeat_count_--;
                }
            } else {
                Stop();
            }
            return;
        }
}

        // todo: forceRenderNextFrame
        renderer_->MakeCurrent();
        bool removed = false;
        bool skip_render = false;
        for (auto iter = compositions_.begin(); iter != compositions_.end(); ) {
            auto& composition = *iter;
            composition->Tick(dt);
            if (!composition->renderer_) {
                delete composition;
                removed = true;
                EmitEvent(EVENT_TYPE_ANIMATION_END, play_token_);
            }
            if (removed) {
                iter = compositions_.erase(iter);
            } else {
                iter++;
            }
        }

        if (compositions_.size()) {
            clear_frame_->Render();
            for (int i = 0; i < compositions_.size(); i++) {
                compositions_[i]->render_frame_->Render();
            }
            renderer_->SwapBuffer();
        }
    }
}

bool MarsPlayer::IsSupportCompressedTexture() const {
    if (!renderer_) {
        MLOGE("IsSupportCompressedTexture no renderer");
        assert(0);
        return false;
    }
    return renderer_->InnerRenderer()->GPUCapability()->SupportCompressedTexture();
}

bool MarsPlayer::CreateTextureContainers() {
    textures_.clear();
    bool support_ktx = renderer_->InnerRenderer()->GPUCapability()->SupportCompressedTexture();
    if (!scene_data_) {
        MLOGE("CreateTextureContainers null sceneData");
        return false;
    }
    for (int i = 0; i < scene_data_->GetImageCount(); i++) {
        ImageRawData* raw_data = scene_data_->GetImageData(i);
        if (raw_data) {
            if (raw_data->is_video) {
                VideoContext* vc = ((VideoImageRawData*) raw_data)->video_context_;
                ((VideoImageRawData*) raw_data)->video_context_ = nullptr;
                if (!vc) {
                    MLOGE("CreateTextureContainers invalid video context");
                    return false;
                }
                TextureOption opt(vc->GetWidth(), vc->GetHeight(), TextureSourceType::DATA);
                opt.SetData(nullptr);
                auto video_tex = std::make_shared<MarsVideoTexture>(renderer_, opt, vc);
                textures_.insert(std::make_pair(i, video_tex));
            } else if (ImageLoader::IsKtxImage(raw_data->data, raw_data->byte_length)) {
                if (!support_ktx) {
                    MLOGE("CreateTextureContainers Device not support ktx");
                    return false;
                }
                TextureOption opt(TextureSourceType::COMPRESSED);
                opt.SetData(raw_data->data);
                
                textures_.insert(std::make_pair(i, std::make_shared<MarsTexture>(renderer_, opt)));
            } else {
                // todo: 新春之后 flipY逻辑
                ImageData* data = ImageLoader::ParseImageData(raw_data);
                if (!data) {
                    MLOGE("CreateTextureContainers Decode Failed %d", i);
                    return false;
                }
                TextureOption opt(data->width, data->height, TextureSourceType::DATA);
                opt.premultiply_alpha = data->premultiply_alpha;
                opt.SetData(data->data);
                
                textures_.insert(std::make_pair(i, std::make_shared<MarsTexture>(renderer_, opt)));
                
                delete data;
            }
        } else {
            MLOGE("CreateTextureContainers Failed %d", i);
            return false;
        }
    }
    scene_data_->ReleaseImageData();
    return true;
}

VideoContext* MarsPlayer::CreateVideoContext(const std::string& file_path, const std::string& key) {
    std::shared_ptr<VideoContext> vc = std::make_shared<VideoContext>(file_path, key);
    videos_.push_back(vc);
    return vc.get();
}

Composition* MarsPlayer::InitializeComposition() {
    // todo: willReverse

    // todo: 改成string
    std::string composition_id = scene_data_->GetComposition()->id;

    // todo: 多个合成

    GetVFXItemOptions opts;
    opts.render_level = render_level_;
    VFXCompositionItem* item = Loader::GetVFXItem(scene_data_, opts);

    CompositionConfig cfg;
    cfg.item = item;
    cfg.renderer = renderer_;
    cfg.plugin_system = plugin_system_;
    cfg.scene = scene_data_;
    cfg.camera = item->camera;
    cfg.root_transform = std::make_shared<Transform>();
    cfg.repeat_count = repeat_count_;
    Composition* composition = new Composition(cfg, player_id_, textures_);

    if (item->camera) {
        composition->SetCamera(item->camera);
    }
    
    delete item;
    composition->Start();
    return composition;
}

void MarsPlayer::RegisterEventHandler(PlayerEventHandler* handler) {
    event_handler_ref_ = handler;
}

bool MarsPlayer::RegisterNativePlugin(const std::string& name, MarsNativePluginBuilder* native_plugin) {
    return plugin_system_->RegisterNativePlugin(name, native_plugin);
}

void MarsPlayer::RegisterExtensionPlugin(const std::string& name, void* ptr) {
    plugin_system_->RegisterPlugin(name, (MarsPluginBuilder*) ptr, true);
}

void MarsPlayer::EmitEvent(int type, const std::string& msg) {
    if (event_handler_ref_) {
        event_handler_ref_->OnEvent(type, msg);
        return;
    }
    PlayerEventEmitter::OnEvent(player_id_, type, msg);
}

void MarsPlayer::updateTexture(int idx, int w, int h, uint8_t* data) {
    if (compositions_.empty()) {
        MLOGE("Can't find compostion");
        EmitEvent(EVENT_TYPE_RUNTIME_ERROR, "compositionNotFound");
        return;
    }
    auto texture = compositions_[0]->GetTextureContainerById(idx);
    if (!texture) {
        EmitEvent(EVENT_TYPE_RUNTIME_ERROR, "textureNotFound");
        return;
    }
    texture->UpdateSource(TextureRawData(w, h, data));
}

}
