//
//  sprite_vfx_item.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/4/13.
//

#include "sprite_vfx_item.h"
#include "sprite_item.h"
#include "sprite_group.h"
#include "player/composition.h"

namespace mn {

SpriteVFXItem::SpriteVFXItem(ItemData* options, Composition* composition) {
//    Create(options, composition);
}

SpriteVFXItem::~SpriteVFXItem() {
}

void SpriteVFXItem::OnConstructed(ItemData* options) {
    sprite_ = (SpriteContentData*) options->content;
}

BaseContentItem* SpriteVFXItem::InnerCreateContent() {
    MeshRendererOptions* opt = composition_->GetParticleOptions();
    return new SpriteItem(sprite_, opt, this);
}

void SpriteVFXItem::OnLifeTimeBegin(Composition* renderer, void* content) {
    content_visible_ = true;
}

void SpriteVFXItem::OnItemRemoved(Composition* composition, void* content) {
    content_visible_ = false;
    if (content) {
        // tood: destroy textures;
        // MLOGD("!!!! SpriteVFXItem::OnItemRemoved tood: destroy textures");
//        composition->DestroyTextures(((SpriteItem*) content)->GetTextures);
    }
}

void SpriteVFXItem::Precompile(GLShaderLibrary* shader_library) {
    CreateContent();
    const auto& item = ((SpriteItem*)content_)->render_info_;
    
    auto iter = shader_library->shader_result_.find(SpriteMesh::GetSpriteMeshShaderIdFromRenderInfo(item, 2));
    if (iter == shader_library->shader_result_.end()) {
        int count = 1;
        for (int i = 1; i < 4; i++) {
            count *= 2;
            if (count <= SpriteMesh::GetMaxSpriteMeshItemCount()) {
                Shader shader;
                std::string cache_id = SpriteMesh::SpriteMeshShaderFromRenderInfo(shader, item, count);
                std::string id = shader_library->AddShader(shader);
                shader_library->CompileShader(id);
            }
        }
    }
}

void SpriteVFXItem::OnItemUpdate(float dt, float lifetime) {
    ((SpriteItem*) content_)->motph_lifetime_ = lifetime;
}

SpriteVFXItemRenderData* SpriteVFXItem::GetCurrentRenderData() {
    const auto item = (SpriteItem*) content_;
    if (item) {
        const auto ig = composition_->loader_data_.sprite_group;
        if (ig) {
            const auto mesh = ig->GetSpriteMesh(item);
            if (mesh) {
                SpriteVFXItemRenderData* data = new SpriteVFXItemRenderData;
                if (mesh->GetItemRenderData(*data, item)) {
                    return data;
                }
                delete data;
            }
        }
    }
    return nullptr;
}

}
