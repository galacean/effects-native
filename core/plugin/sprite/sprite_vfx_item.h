//
//  sprite_vfx_item.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/4/13.
//

#ifndef MN_PLUGIN_SPRITE_SPRITE_VFX_ITEM_H_
#define MN_PLUGIN_SPRITE_SPRITE_VFX_ITEM_H_

#include "player/vfx_item.h"
#include "math/vec2.hpp"

namespace mn {

class SpriteRenderData;

struct SpriteVFXItemRenderData {
    Vec3 position;
    Vec2 size;
    Vec4 quat;
    Vec4 color;
};

class SpriteVFXItem : public VFXItem {
public:
    SpriteVFXItem(ItemData* options, Composition* composition);

    ~SpriteVFXItem();
    
    std::string GetType() const override {
        return VFX_ITEM_TYPE_SPRITE;
    }
    
    virtual void OnConstructed(ItemData* arg) override;
    
    BaseContentItem* InnerCreateContent() override;
    
    void OnLifeTimeBegin(Composition* renderer, void* content) override;
    
    void OnItemRemoved(Composition* renderer, void* content) override;
    
    void Precompile(GLShaderLibrary* shader_library) override;
    
    void OnItemUpdate(float dt, float lifetime) override;
    
    SpriteVFXItemRenderData* GetCurrentRenderData();
    
public:
    SpriteContentData* sprite_ = nullptr;
};

}

#endif /* MN_PLUGIN_SPRITE_SPRITE_VFX_ITEM_H_ */
