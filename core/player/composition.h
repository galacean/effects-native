#ifndef MN_PLAYER_COMPOSITION_H_
#define MN_PLAYER_COMPOSITION_H_

#include "load/loader.h"
#include "ri/render/mars_renderer.hpp"
#include "math/camera.hpp"
#include "ri/render/mars_player_render_frame.h"
#include "sceneData/scene_data_decoder.h"
#include "plugin/cal/calculate_group.h"

namespace mn {

class CompVFXItem;
class MarsPlugin;
class VFXItem;
class SpriteGroup;
class MarsVideoTexture;

// todo: 对齐前端字段
struct CompositionConfig {
    VFXCompositionItem* item;
    MarsRenderer* renderer;
    PluginSystem* plugin_system;
    SceneDataDecoder* scene;
    float speed = 1;
    CameraData* camera = nullptr;
    std::shared_ptr<Transform> root_transform;
    int repeat_count = -100;
};

struct CompositionLoaderData {
    ~CompositionLoaderData();
    
    SpriteGroup* sprite_group = nullptr;
};


class Composition {
public:
    Composition(const CompositionConfig& config, int player_id, std::map<int, std::shared_ptr<MarsTexture>>& textures);

    ~Composition();

    void SetCamera(CameraData* camera_data);

    void Start();

    void Tick(float time);
    
    void Pause();
    
    void Resume();

    void Destroy();

    void UpdatePluginLoaders(float dt);
    
    void ItemLifetimeBegin(std::shared_ptr<VFXItem> item);

    void DestroyItem(std::shared_ptr<VFXItem> item);

    void OnMessageItem(const std::string& item_name, const std::string& phrase, const std::string& item_id);

    void PausePlayer(VFXItem* vfx_item);

    std::vector<std::shared_ptr<VFXItem>>* GetItems();

    MeshRendererOptions* GetParticleOptions();
    
    std::shared_ptr<MarsTexture> GetTextureContainerById(int idx);

    void Restart(int start_time);

    void ForwardTime(float current_time_sec);
    
    bool GetForwarding() const {
        return inner_forwarding_;
    }

    void SetRepeatCount(int repeat_count) {
        if (repeat_count < 0) {
            repeat_count = -1;
        } 
        repeat_count_ = repeat_count;
        restart_times_ = 0;
    }

    std::shared_ptr<Camera> GetCamera() const {
        return camera_;
    }
    
    std::shared_ptr<CameraParams> GetCameraParams() const {
        return camera_params_;
    }

private:

    void InnerReset();

    void StartContent();

    void InnerPrepareRender();
    
    void InnerPrecompile();
    
    bool GetShouldRestart();
    
    bool GetShouldDestroy();

    void OnWillUpdateItems(float t);

public:
    MarsRenderer* renderer_ = nullptr;

    MarsPlayerRenderFrame* render_frame_ = nullptr;

    CalculateGroup* calculate_group_ = nullptr;

public:
    int id_;

    float speed_ = 1;

    PluginSystem* plugin_system_ = nullptr;

    CompositionLoaderData loader_data_;
    
    std::shared_ptr<CompVFXItem> content_;

    std::shared_ptr<Camera> camera_;

    std::vector<MarsPlugin*> post_loaders_;

    bool tick_enabled_ = true;

    MeshRendererOptions* renderer_options_ = nullptr;

    std::shared_ptr<Transform> root_transform_;
    
    std::vector<float> preview_size;
        
private:
    
    std::shared_ptr<CameraParams> camera_params_;
    
    std::map<int, std::shared_ptr<MarsTexture>> textures_;
    std::vector<std::shared_ptr<MarsVideoTexture>> video_textures_;
    
    bool inner_forwarding_ = false;

    int repeat_count_ = -100;

    int restart_times_ = 0;

    int player_id_ = -1;

};

}

#endif
