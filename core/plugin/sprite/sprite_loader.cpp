#include "sprite_loader.h"
#include "sprite_group.h"
#include "player/composition.h"
#include "filters/filter_define.hpp"

namespace mn {

SpriteLoader::SpriteLoader() {
    name_ = "SpriteLoader";
}

SpriteLoader::~SpriteLoader() {

}

void SpriteLoader::SpriteMeshShaderFromFilter(Shader& shader, FilterDefine* filter_define) {
    SpriteMesh::SpriteMeshShaderFromFilter(shader, filter_define, 2);
}

void SpriteLoader::OnCompositionDestroyed(Composition* comp) {
//    MLOGD("SpriteLoader::%s", __FUNCTION__);
    SpriteGroup* sprite_group = comp->loader_data_.sprite_group;
    sprite_group->Destroy();
}

void SpriteLoader::OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) {
//    MLOGD("SpriteLoader::%s", __FUNCTION__);
    MN_SAFE_DELETE(comp->loader_data_.sprite_group);
    comp->loader_data_.sprite_group = new SpriteGroup(comp->calculate_group_);
    comp->loader_data_.sprite_group->composition_ = comp;
    comp->loader_data_.sprite_group->ResetMeshSplits();
}

void SpriteLoader::OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) {
//    MLOGD("SpriteLoader::%s", __FUNCTION__);
    SpriteGroup* sprite_group = comp->loader_data_.sprite_group;
    if (item->GetType().compare(VFX_ITEM_TYPE_COMPOSITION) != 0) {
        sprite_group->AddItem(item);
    }
}

void SpriteLoader::OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) {
    // MLOGD("SpriteLoader::%s", __FUNCTION__);
    SpriteGroup* sprite_group = comp->loader_data_.sprite_group;
    sprite_group->RemoveItem(item);
}

void SpriteLoader::OnCompositionUpdate(Composition* comp, float dt) {
//    MLOGD("SpriteLoader::%s", __FUNCTION__);
    SpriteGroup* sprite_group = comp->loader_data_.sprite_group;
    sprite_group->OnUpdate(dt);
}

bool SpriteLoader::PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {
//    MLOGD("SpriteLoader::%s", __FUNCTION__);
    SpriteGroup* sprite_group = comp->loader_data_.sprite_group;
    MeshSplitsDiff ret;
    sprite_group->DiffMeshSplits(ret);
    
    for (auto mesh : ret.remove) {
        render_frame->RemoveMeshToDefaultRenderPass(mesh);
    }
    for (auto mesh : ret.add) {
        render_frame->AddMeshToDefaultRenderPass(mesh);
    }
    for (auto mesh : ret.modify) {
        render_frame->RemoveMeshToDefaultRenderPass(mesh);
        render_frame->AddMeshToDefaultRenderPass(mesh);
    }
    
    if (ret.layer.size() > 0) {
        this->layer_info_ = ret.layer;
        return true;
    }
    
    return false;
}

void SpriteLoader::PostProcessFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    if (layer_info_.size() > 0) {
        for (int i=0; i<layer_info_.size(); i++) {
            const auto& sprite_mesh = layer_info_[i];
            const auto& item = sprite_mesh->items_[0];
            if (item->GetFilter()) {
                auto filter = item->GetFilter();
                auto render_pass = render_frame->SplitDefaultRenderPassByMesh(sprite_mesh->mesh_, filter.get());
                if (filter->GetRenderPassDelegate()) {
                    render_pass->SetRenderPassDelegate(filter->GetRenderPassDelegate());
                }
            }
        }
    }
}

}
