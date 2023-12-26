//
//  filter_sprite_vfx_item.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef filter_sprite_vfx_item_hpp
#define filter_sprite_vfx_item_hpp

#include <stdio.h>
#include <vector>
#include "plugin/sprite/sprite_vfx_item.h"
#include "ri/render/mars_mesh.hpp"
#include "filters/filter_define.hpp"

namespace mn {

class FilterSpriteVFXItem : public SpriteVFXItem {
  
public:
    
    FilterSpriteVFXItem(ItemData* options, Composition* composition);
    
    ~FilterSpriteVFXItem();
    
    std::string GetType() const override {
        return VFX_ITEM_TYPE_FILTER_SPRITE;
    }
    
    void OnConstructed(ItemData* arg) override;
    
    void OnItemUpdate(float dt, float lifetime) override;
    
    void OnItemRemoved(Composition* renderer, void* content) override;
    
    BaseContentItem* InnerCreateContent() override;
    
private:
    
    std::vector<MarsMesh> layers_;
    
    ItemFilterData* filter_option_ = nullptr;
    
    std::shared_ptr<FilterDefine> filter_define_;
    
};
}


#endif /* filter_sprite_vfx_item_hpp */
