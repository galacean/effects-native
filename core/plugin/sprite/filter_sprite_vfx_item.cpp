//
//  filter_sprite_vfx_item.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "filter_sprite_vfx_item.hpp"
#include "sprite_item.h"
#include "filters/filter_utils.hpp"
#include "sprite_group.h"

namespace mn {

FilterSpriteVFXItem::FilterSpriteVFXItem(ItemData* options, Composition* composition) : SpriteVFXItem(options, composition)  {
    MLOGD("FilterSpriteVFXItem Constructed");
}

FilterSpriteVFXItem::~FilterSpriteVFXItem() {
    MLOGD("FilterSpriteVFXItem Destructed");
}

void FilterSpriteVFXItem::OnConstructed(ItemData* arg) {
    SpriteVFXItem::OnConstructed(arg);
    this->filter_option_ = sprite_->filter_data;
}

void FilterSpriteVFXItem::OnItemRemoved(Composition* renderer, void* content) {
    // filter->remove,好像不太需要；会自动xigou；
    SpriteVFXItem::OnItemRemoved(renderer, content);
}


void FilterSpriteVFXItem::OnItemUpdate(float dt, float lifetime) {
    SpriteVFXItem::OnItemUpdate(dt, lifetime);
    auto uniform_variables = filter_define_->GetMeshFilterOptions().uniform_variables;
    
    if (uniform_variables.size() > 0) {
        const auto item = (SpriteItem*) content_;
        std::shared_ptr<MarsMaterial> material;
        if (item) {
            const auto ig = composition_->loader_data_.sprite_group;
            if (ig) {
                const auto mesh = ig->GetSpriteMesh(item);
                if (mesh) {
                    material = mesh->GetInnerMesh()->GetMaterial();
                }
            }
        }
        
        if (material) {
            for (auto& uniform_variable : uniform_variables) {
                auto uniform_value = material->GetUniformValue(uniform_variable);
                if (uniform_value) {
                    filter_define_->GetVariableUniformValue(uniform_variable, lifetime, uniform_value);
                }
            }
        }
    }
}

BaseContentItem* FilterSpriteVFXItem::InnerCreateContent() {
    SpriteItem* sprite_item = (SpriteItem *) SpriteVFXItem::InnerCreateContent();
    filter_define_ = FilterUtils::GenerateFilter(composition_, filter_option_);
    if (filter_define_) {
        sprite_item->render_info_->cache_id.replace(sprite_item->render_info_->cache_id.find("$F$"), 3, filter_define_->GetMeshFilterOptions().shader_cache_id);
        sprite_item->SetFilter(filter_define_);
        sprite_item->render_info_->filter = filter_define_;
    } else {
        MLOGE("Filter VFX filter_define_ is nullptr");
    }
    
    return sprite_item;
}

}
