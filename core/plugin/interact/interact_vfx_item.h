#ifndef MN_PLUGIN_INTERACT_INTERACT_VFX_ITEM_H_
#define MN_PLUGIN_INTERACT_INTERACT_VFX_ITEM_H_

#include "player/vfx_item.h"

namespace mn {

class InteractVFXItem : public VFXItem {
public:
    InteractVFXItem(ItemData* options, Composition* composition);

    ~InteractVFXItem();
    
    std::string GetType() const override {
        return VFX_ITEM_TYPE_INTERACT;
    }

    void OnConstructed(ItemData* arg) override;
    
    BaseContentItem* InnerCreateContent() override;
    
    void OnLifeTimeBegin(Composition* renderer, void* content) override;
    
    void OnItemRemoved(Composition* renderer, void* content) override;

public:
    InteractContentData* interact_ = nullptr;
};

}

#endif
