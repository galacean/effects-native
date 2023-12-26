//
//  model_plugin.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef model_plugin_hpp
#define model_plugin_hpp

#include <stdio.h>
#include "player/plugin_system.h"
#include "model/renderer/m_scene_manager.hpp"
#include "model_vfx_item.hpp"

namespace mn {

class ModelPlugin : public MarsPlugin {
    
public:
    
    ModelPlugin();
    
    ~ModelPlugin();
    
    void OnCompositionConstructed(Composition* comp, SceneDataDecoder* scene) override;
    
    void OnCompositionWillReset(Composition* comp, MarsPlayerRenderFrame* render_frame) override;

    void OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) override;

    void OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionUpdate(Composition* comp, float dt) override;

    void OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionDestroyed(Composition* comp) override;

    // 合成更新后，在渲染前进行pipeline的配置，添加渲染的Mesh到pipeline中，
    // 如果此函数返回true，将进行pipeline后处理函数
    bool PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) override;
    // 当所有的plugin都调用过preparePipeline后，对于需要进行后处理的plugin，调用此函数，
    // 此函数一般用于切割renderPass，如果对于renderPass有切割，记得在销毁时还原切割
    void PostProcessFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) override;
    
private:
    
    void UpdateDefaultCamera(Composition* comp);
    
    std::shared_ptr<MSceneManager> scene_manger_;
    
    float delta_time_; 
    
};

class ModelPluginBuilder : public MarsPluginBuilder {
public:
    MarsPlugin* CreatePlugin() override {
        return new ModelPlugin;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        std::shared_ptr<VFXItem> item = std::make_shared<ModelVFXItem>();
        item->Create(options, composition);
        return item;
    }
};

}

#endif /* model_plugin_hpp */
