#ifndef MN_PLAYER_COMP_VFX_ITEM_H_
#define MN_PLAYER_COMP_VFX_ITEM_H_

#include <map>
#include "vfx_item.h"

namespace mn {

class CompVFXItem : public VFXItem {
public:
    CompVFXItem(ItemData* arg_t, Composition* comp);

    ~CompVFXItem();

    std::string GetType() const override {
        return VFX_ITEM_TYPE_COMPOSITION;
    }
    
    void OnConstructed(ItemData* arg) override;

    float GetUpdateTime(float t);
    
    void OnUpdate(float dt, std::shared_ptr<VFXItem> thiz);

    void OnItemUpdate(float dt, float lifetime) override;

    void InnerStop() override;

    void OnItemRemoved(Composition* renderer, void* content) override;

    BaseContentItem* CreateContent() override;

    void OnLifeTimeBegin(Composition* renderer, void* content) override;

    std::shared_ptr<VFXItem> GetItemById(const std::string& id);

    bool RemoveItem(std::shared_ptr<VFXItem> item);

private:
    std::vector<std::shared_ptr<VFXItem>>* CreateItemsWidthOrder(Composition* renderer);

public:
    std::vector<ItemData*>* items_;

    std::vector<std::shared_ptr<VFXItem>>* _items_ = nullptr;

    std::map<std::string, std::shared_ptr<VFXItem>> item_cache_map_;

    std::vector<std::shared_ptr<VFXItem>> items_to_remove_;

    bool freeze_on_end_ = false;

    float start_time_in_ms_ = 0;

    float start_time_ = 0;

    float time_in_ms_ = 0;
};

}

#endif
