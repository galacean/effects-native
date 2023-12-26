//
//  model_tree_plugin.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/10/18.
//

#include "model_tree_plugin.h"
#include "player/composition.h"
#include "plugin/tree/tree_vfx_item.hpp"
#include "renderer/animation/m_animation.hpp"

namespace mn {

ModelTreePlugin::ModelTreePlugin() {
    name_ = "model-tree";
    order_ = 2;
}

ModelTreePlugin::~ModelTreePlugin() {
    
}

void ModelTreePlugin::OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    auto items = comp->GetItems();
    if (items) {
        tree_items_.clear();
        
        for (int i = 0; i < items->size(); i++) {
            auto item = items->at(i);
            if (item->GetType().compare("tree") == 0) {
                tree_items_.push_back(std::static_pointer_cast<TreeVFXItem>(item));
            }
        }
        
        controller_ = std::make_shared<MAnimationController>();
        controller_->Create(tree_items_);
    }
}

void ModelTreePlugin::OnCompositionUpdate(Composition* comp, float dt) {
    if (controller_) {
        controller_->Tick(dt);
    }
}

}
