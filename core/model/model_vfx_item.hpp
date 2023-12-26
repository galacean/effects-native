//
//  model_vfx_item.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef model_vfx_item_hpp
#define model_vfx_item_hpp

#include "player/vfx_item.h"
#include "math/transform.h"
#include "util/constant.hpp"

namespace mn {

struct MarsItemModelBase;
class MEntity;

class ModelVFXItem : public VFXItem {
public:
    ModelVFXItem();
    
    std::string GetType() const override {
        return VFX_ITEM_TYPE_3D;
    }
    
    void OnConstructed(ItemData* arg) override;
    
    BaseContentItem* InnerCreateContent() override;
    
    void SetContent3D(std::shared_ptr<MEntity> content_3d) {
        content_3d_ = content_3d;
    }
    
    void OnLifeTimeBegin(Composition* renderer, void* content) override;
    
    void OnItemUpdate(float dt, float lifetime) override;
    
    void OnItemRemoved(Composition* renderer, void* content) override;
    
public:
    std::shared_ptr<MarsItemModelBase> options_;
    std::shared_ptr<MEntity> content_3d_;
    
    ContentDataBase* model_content_ref_ = nullptr;
};

}

#endif /* model_vfx_item_hpp */
