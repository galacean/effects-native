#ifndef MN_PLAYER_PLUGIN_SYSTEM_H_
#define MN_PLAYER_PLUGIN_SYSTEM_H_

#include <map>
#include <vector>
#include <string>
#include "vfx_item.h"
#include "plugin/types.h"
#include "sceneData/scene_data_decoder.h"
#include "player/vfx_item.h"

namespace mn {

class Composition;
class MarsPlayerRenderFrame;
class MarsNativePluginBuilder;

class MarsPlugin {
public:
    virtual ~MarsPlugin() {}
    
    virtual void OnCompositionConstructed(Composition* comp, SceneDataDecoder* scene) {}
    
    virtual void OnCompositionWillReset(Composition* comp, MarsPlayerRenderFrame* render_frame) {}

    virtual void OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) {}

    virtual void OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) {}

    virtual void OnCompositionUpdate(Composition* comp, float dt) {}

    virtual void OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) {}

    virtual void OnCompositionDestroyed(Composition* comp) {}

    // 合成更新后，在渲染前进行pipeline的配置，添加渲染的Mesh到pipeline中，
    // 如果此函数返回true，将进行pipeline后处理函数
    virtual bool PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {
        return false;
    }

    // 当所有的plugin都调用过preparePipeline后，对于需要进行后处理的plugin，调用此函数，
    // 此函数一般用于切割renderPass，如果对于renderPass有切割，记得在销毁时还原切割
    virtual void PostProcessFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {}

    // native自定义插件专用，用于在每一帧的最开始更新插件
    virtual void OnCompositionWillUpdateItems(float dt) {}

    int order_ = 100;
    
    std::string name_ = "";
};

class MarsPluginBuilder {
public:
    virtual ~MarsPluginBuilder() {}

    virtual MarsPlugin* CreatePlugin() = 0;

    virtual std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) = 0;
};

class PluginSystem {
public:
    PluginSystem();

    ~PluginSystem();

    // const std::vector<std::string>& GetDefaultPlugins();

//    MarsPlugin* GetPluginLoader(const std::string& name);

    void InitPlugins();
    
    void InitializeComposition(Composition* comp, SceneDataDecoder* scene);

    void ResetComposition(Composition* comp, MarsPlayerRenderFrame* render_frame);
    
    void DestroyComposition(Composition* comp);

    std::shared_ptr<VFXItem> CreatePluginItem(const std::string& type, ItemData* options, Composition* composition);

    bool RegisterNativePlugin(const std::string& name, MarsNativePluginBuilder* native_loader_builder);

    void RegisterPlugin(const std::string& name, MarsPluginBuilder* loader_builder, bool is_default = false);

public:
    std::vector<MarsPlugin*> loaders_;

    std::map<std::string, MarsPluginBuilder*> plugin_builder_map_;

    std::vector<std::string> default_plugins_;

//    std::map<std::string, MarsPlugin*> plugin_loader_map_;
};

}

#endif
