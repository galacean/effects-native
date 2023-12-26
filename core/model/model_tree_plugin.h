//
//  model_tree_plugin.h
//  PlayGroundIOS
//
//  Created by changxing on 2022/10/18.
//

#ifndef model_tree_plugin_h
#define model_tree_plugin_h

#include "player/plugin_system.h"
#include "plugin/tree/tree_vfx_item.hpp"

namespace mn {

class MAnimationController;
class TreeVFXItem;

class ModelTreePlugin : public MarsPlugin {
public:
    ModelTreePlugin();
    
    ~ModelTreePlugin();
    
    void OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) override;
    
    void OnCompositionUpdate(Composition* comp, float dt) override;

private:
    std::shared_ptr<MAnimationController> controller_;
    std::vector<std::shared_ptr<TreeVFXItem>> tree_items_;
};

class ModelTreePluginBuilder : public MarsPluginBuilder {
public:
    MarsPlugin* CreatePlugin() override {
        return new ModelTreePlugin;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        std::shared_ptr<VFXItem> item = std::make_shared<TreeVFXItem>();
        item->Create(options, composition);
        return item;
    }
};

}

#endif /* model_tree_plugin_h */
