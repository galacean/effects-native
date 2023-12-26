#ifndef MN_PLUGIN_CAL_CALCULATE_VFX_ITEM_H_
#define MN_PLUGIN_CAL_CALCULATE_VFX_ITEM_H_

#include "player/vfx_item.h"
#include "sceneData/scene_data_decoder.h"

namespace mn {

class SpriteRenderData;

class CalculateVFXItem : public VFXItem {
public:
    CalculateVFXItem(ItemData* options, Composition* composition);

    ~CalculateVFXItem();

    std::string GetType() const override {
        return VFX_ITEM_TYPE_NULL;
    }

    void OnConstructed(ItemData* arg) override;

    void InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) override;

    void OnUpdateByLoader(float dt, std::shared_ptr<VFXItem> thiz) override;
    
    SpriteRenderData* GetCurrentRenderData();
    
protected:
    BaseContentItem* InnerCreateContent() override;
    
    void OnLifeTimeBegin(Composition* renderer, void* content) override;

public:
    bool relative_ = false;

    SpriteContentData* cal_ = nullptr;
};

}

#endif
