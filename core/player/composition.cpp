#include "composition.h"
#include "comp_vfx_item.h"
#include "util/log_util.hpp"
#include "util/util.hpp"
#include "player/plugin_system.h"
#include "ri/material.h"
#include "plugin/sprite/sprite_group.h"
#include "mars_player.h"
#include "video/mars_video_texture.hpp"

namespace mn {

static int s_id_seed = 1;

CompositionLoaderData::~CompositionLoaderData() {
    MN_SAFE_DELETE(sprite_group);
}

Composition::Composition(const CompositionConfig& config, int player_id, std::map<int, std::shared_ptr<MarsTexture>>& textures) {
    MLOGD("Composition::Composition");
    
    renderer_ = config.renderer;
    id_ = s_id_seed++;
    speed_ = config.speed;
    plugin_system_ = config.plugin_system;
    repeat_count_ = config.repeat_count;
    player_id_ = player_id;
    // todo: 对齐属性
    
    textures_.swap(textures);
    for (auto& iter : textures_) {
        if (iter.second->IsVideo()) {
            video_textures_.push_back(std::static_pointer_cast<MarsVideoTexture>(iter.second));
        }
    }

    CompositionData* composition = config.scene->GetComposition();

    std::shared_ptr<CompVFXItem> vfx_item = std::make_shared<CompVFXItem>(composition, this);
    if (config.root_transform) {
        vfx_item->transform_ = config.root_transform;
    } else {
        vfx_item->transform_ = std::make_shared<Transform>();
    }
    root_transform_ = vfx_item->transform_;
    
    content_ = vfx_item;

    if (composition->preview_size) {
        preview_size.push_back(composition->preview_size[0]);
        preview_size.push_back(composition->preview_size[1]);
    }

    // todo: name
    camera_params_ = std::make_shared<CameraParams>();
    camera_ = std::make_shared<Camera>();
    SetCamera(config.camera);
    plugin_system_->InitializeComposition(this, config.scene);
    InnerReset();
    InnerPrecompile();
}

Composition::~Composition() {
    MLOGD("Composition::~Composition");
    Destroy();
    MN_SAFE_DELETE(render_frame_);
    MN_SAFE_DELETE(calculate_group_);
    MN_SAFE_DELETE(renderer_options_);
}

void Composition::SetCamera(CameraData* camera_data) {
    camera_params_->near = camera_data->near;
    camera_params_->far = camera_data->far;
    camera_params_->fov = camera_data->fov;
    if (camera_data->position) {
        camera_params_->position.Set(camera_data->position[0], camera_data->position[1], camera_data->position[2]);
    }
    if (camera_data->rotation) {
        camera_params_->rotation = std::make_shared<Vec3>(camera_data->rotation[0], camera_data->rotation[1], camera_data->rotation[2]);
    }
    if (camera_data->clip_mode) {
        camera_params_->clip_mode = camera_data->clip_mode->val;
    }
    if (renderer_->GetSurfaceWidth() > 1 && renderer_->GetSurfaceHeight() > 1) {
        camera_params_->aspect = ((float) renderer_->GetSurfaceWidth()) / ((float) renderer_->GetSurfaceHeight());
    } else if (preview_size.size() == 2 && preview_size[0] != 0 && preview_size[1] != 0) {
        camera_params_->aspect = preview_size[0] / preview_size[1];
    } else {
        MLOGD("Composition::SetCamera aspect preview size is null");
        camera_params_->aspect = 1;
    }
    
    MLOGD("Composition::SetCamera aspect: %f", camera_params_->aspect);
    camera_->SetParams(camera_params_);
}

void Composition::Start() {
    StartContent();
}

void Composition::Pause() {
    for (const auto& v : video_textures_) {
        v->Pause();
    }
}

void Composition::Resume() {
    for (const auto& v : video_textures_) {
        v->Resume();
    }
}

bool Composition::GetShouldRestart() {
    if (!content_->ended_) {
        // 动画没结束，不需要restart
        return false;
    }
    if (repeat_count_ >= 0) {
        // 重播次数小于重复次数，应该restart
        return (restart_times_ < repeat_count_);
    }
    if (repeat_count_ == -1) {
        // 一直循环，应该restart
        return true;
    }
    // 没设置过repear_count，走动画的设置
    return content_->end_behavior_ == END_BEHAVIOR_RESTART;
}

bool Composition::GetShouldDestroy() {
    if (!content_->ended_) {
        // 动画没结束，不需要restart
        return false;
    }
    if (repeat_count_ == -1) {
        // 一直循环，还应该继续播
        return false;
    }
    if (repeat_count_ >= 0) {
        // 如果重播次数小于重复次数，还应该继续播
        return (restart_times_ >= repeat_count_);
    }
    // 没设置过repear_count，走动画的设置
    return !content_->end_behavior_ || (content_->end_behavior_ == END_BEHAVIOR_PAUSE_AND_DESTROY);
}

void Composition::Tick(float time) {
    if (!tick_enabled_) {
        return;
    }
    if (renderer_) {
        if (GetShouldRestart()) {
            Restart(0);
        }

        float t = content_->GetUpdateTime(time * speed_);
//        MLOGD("Composition::Tick time %f", t);
        OnWillUpdateItems(t);
        for (const auto& v : video_textures_) {
            if (!v->UpdateVideoFrame()) {
                PlayerEventEmitter::OnEvent(
                        player_id_, EVENT_TYPE_RUNTIME_ERROR, "update video frame fail");
                return;
            }
        }
        calculate_group_->OnUpdate(t);
        content_->InnerOnUpdate(t, content_);
        camera_->UpdateMatrix();
        if (GetShouldDestroy()) {
            Destroy();
        } else {
            InnerPrepareRender();
        }
    }
}

void Composition::Destroy() {
    if (renderer_) {
        content_->Destroy(content_);
        // todo: keepResource keepCompiled _rendererOptions
        plugin_system_->DestroyComposition(this);
        tick_enabled_ = false;
        renderer_ = nullptr;
    }
}

void Composition::OnWillUpdateItems(float t) {
    for (auto& loader : plugin_system_->loaders_) {
        loader->OnCompositionWillUpdateItems(t);
    }
}

void Composition::UpdatePluginLoaders(float dt) {
    for (auto& loader : plugin_system_->loaders_) {
        loader->OnCompositionUpdate(this, dt);
    }
}

void Composition::ItemLifetimeBegin(std::shared_ptr<VFXItem> item) {
    for (int i = 0; i < plugin_system_->loaders_.size(); i++) {
        plugin_system_->loaders_[i]->OnCompositionItemLifeBegin(this, item);
    }
}

void Composition::DestroyItem(std::shared_ptr<VFXItem> item) {
    if (content_->RemoveItem(item)) {
        for (int i = 0; i < plugin_system_->loaders_.size(); i++) {
            plugin_system_->loaders_[i]->OnCompositionItemRemoved(this, item);
        }
    }
}

void Composition::OnMessageItem(const std::string& item_name, const std::string& phrase, const std::string& item_id) {
    int type = -1;
    if (phrase.compare("MESSAGE_ITEM_PHRASE_BEGIN") == 0) {
        type = 6;
    } else if (phrase.compare("MESSAGE_ITEM_PHRASE_END") == 0) {
        type = 7;
    }
    PlayerEventEmitter::OnEvent(player_id_, type, item_name);
}

void Composition::PausePlayer(VFXItem* vfx_item) {
}

std::vector<std::shared_ptr<VFXItem>>* Composition::GetItems() {
    return content_->_items_;
}

MeshRendererOptions* Composition::GetParticleOptions() {
    if (!renderer_options_) {
        renderer_options_ = new MeshRendererOptions;
        renderer_options_->renderer = renderer_;
        renderer_options_->empty_texture = render_frame_->empty_texture_;
        renderer_options_->cache_prefix = "-";
        renderer_options_->composition = this;
    }
    return renderer_options_;
}

void Composition::Restart(int start_time) {
    ++restart_times_;
    if (content_) {
        content_->Reset();
    }
    InnerPrepareRender();
    InnerReset();
    content_->Start();
    
    // todo: forwardTime
    if (start_time > 0) {
        Tick(start_time); // 五福场景跳帧播没有粒子，最小改动临时方案
    }
    for (const auto& v : video_textures_) {
        v->SeekTo(start_time);
    }
    
    content_->OnUpdate(0, content_);
    calculate_group_->OnUpdate(0);
}

void Composition::ForwardTime(float current_time_sec) {
    const int time = (int) roundf(current_time_sec * 1000);
    if (time) {
        const bool reverse = time < 0;
        int t = abs(time);
        const int step = 15;
        inner_forwarding_ = true;
        for (int ss = reverse ? -step : step; t > step; t -= step) {
            if (t < 0) {
                inner_forwarding_ = false;
            }
            Tick(ss);
        }
        inner_forwarding_ = false;
        if (t > 0) {
            const int ss = reverse ? -t : t;
            Tick(ss);
        }
    }
}

void Composition::InnerReset() {
    MLOGD("Composition::InnerReset");
    if (render_frame_) {
        MN_SAFE_DELETE(renderer_options_);
        for (int i = 0; i < plugin_system_->loaders_.size(); i++) {
            plugin_system_->loaders_[i]->OnCompositionWillReset(this, render_frame_);
        }
        MN_SAFE_DELETE(render_frame_);
    }
    
    // todo: 解决自增的shader泄漏
    renderer_->InnerRenderer()->ShaderLibrary().RemoveUnsharedShader();
    
    RenderFrameOptions opt;
    opt.camera = camera_;
    render_frame_ = new MarsPlayerRenderFrame(renderer_, opt);
    MN_SAFE_DELETE(calculate_group_);
    calculate_group_ = new CalculateGroup();
    content_->CreateContent();
    plugin_system_->ResetComposition(this, render_frame_);
}

void Composition::StartContent() {
    content_->Start();
}

void Composition::InnerPrepareRender() {
//    MLOGD("Composition::%s", __FUNCTION__);
    const auto& frame = render_frame_;
    const auto& loaders = plugin_system_->loaders_;
    post_loaders_.clear();
    for (int i = 0; i < loaders.size(); i++) {
        MarsPlugin* loader = loaders[i];
        if (loader->PrepareRenderFrame(this, frame)) {
            post_loaders_.push_back(loader);
        }
    }
    for (int i = 0; i < post_loaders_.size(); i++) {
        MarsPlugin* loader = post_loaders_[i];
        loader->PostProcessFrame(this, frame);
    }
}

void Composition::InnerPrecompile() {
    const auto& items = GetItems();
    for (auto& item : *items) {
        item->Precompile(&renderer_->InnerRenderer()->ShaderLibrary());
    }
}

std::shared_ptr<MarsTexture> Composition::GetTextureContainerById(int idx) {
    auto iter = textures_.find(idx);
    if (iter == textures_.end()) {
        return nullptr;
    }
    return iter->second;
}

}
