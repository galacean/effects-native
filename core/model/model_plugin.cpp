//
//  model_plugin.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "model_plugin.hpp"
#include "player/composition.h"
#include "model/model_vfx_item.hpp"

namespace mn {



ModelPlugin::ModelPlugin() {
    delta_time_ = 0;
    name_ = "model";
}

ModelPlugin::~ModelPlugin() {
    
}

void ModelPlugin::OnCompositionConstructed(Composition* comp, SceneDataDecoder* scene) {
    scene_manger_ = std::make_shared<MSceneManager>();
}

void ModelPlugin::OnCompositionWillReset(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    
}

void ModelPlugin::OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    MSceneOptions options;
    options.renderer = comp->renderer_;
    options.composition_name = std::to_string(comp->id_);
    scene_manger_->Initial(options);
    this->UpdateDefaultCamera(comp);
    
    auto items = comp->GetItems();
    std::vector<std::shared_ptr<ModelVFXItem>> item_models;
    
    for (size_t i=0; i<items->size(); i++) {
        auto item = items->at(i);
        if (item->GetType() == VFX_ITEM_TYPE_3D) {
            std::shared_ptr<ModelVFXItem> item_3d = std::static_pointer_cast<ModelVFXItem>(item);
            std::shared_ptr<MEntity> content_3d = scene_manger_->CreateItem(item_3d);
            
            if (content_3d) {
                item_3d->SetContent3D(content_3d);
                item_models.push_back(item_3d);
            }
        }
    }
//    scene_manger_->PostCreate(); // for shadow
    scene_manger_->Build();
    for (int i = 0; i < item_models.size(); i++) {
        item_models[i]->CreateContent();
    }
}

void ModelPlugin::OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) {
    
}

void ModelPlugin::OnCompositionUpdate(Composition* comp, float dt) {
    delta_time_ = dt;
}

void ModelPlugin::OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) {
    
}

void ModelPlugin::OnCompositionDestroyed(Composition* comp) {
    
}

bool ModelPlugin::PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    this->UpdateDefaultCamera(comp);
    scene_manger_->Tick(this->delta_time_);
    scene_manger_->OnUpdateRenderObject(render_frame);
    return false;
}

void ModelPlugin::PostProcessFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    
}

void ModelPlugin::UpdateDefaultCamera(Composition* comp) {
    scene_manger_->UpdateDefaultCamera(comp->GetCameraParams());
}

}
