#ifndef MN_PLUGIN_CAL_CALULATE_LOADER_H_
#define MN_PLUGIN_CAL_CALULATE_LOADER_H_

#include "player/plugin_system.h"
#include "calculate_vfx_item.h"

namespace mn {

class CalculateGroup;

class CalculateLoader : public MarsPlugin {
public:
    CalculateLoader();

    ~CalculateLoader();

    void OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* pipeline) override;

    void OnCompositionUpdate(Composition* comp, float dt) override;

    void OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) override;

private:
    void SetItemsParentTransform(CalculateGroup* calculate_group, std::vector<std::shared_ptr<VFXItem>>* items, std::shared_ptr<VFXItem> item);
    
private:
    std::map<std::string, std::shared_ptr<VFXItem>> item_map_;
};

class CalPluginBuilder : public MarsPluginBuilder {
public:
    MarsPlugin* CreatePlugin() override {
        return new CalculateLoader;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        return std::make_shared<CalculateVFXItem>(options, composition);
    }
};

}

#endif
