#ifndef MN_PLUGIN_INTERACT_INTERACT_LOADER_H_
#define MN_PLUGIN_INTERACT_INTERACT_LOADER_H_

#include "player/plugin_system.h"
#include "interact_vfx_item.h"

namespace mn {

class InteractLoader : public MarsPlugin {
public:
    InteractLoader() {}

    ~InteractLoader() {}
};

class InteractPluginBuilder : public MarsPluginBuilder {
public:
    MarsPlugin* CreatePlugin() override {
        return new InteractLoader;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        return std::make_shared<InteractVFXItem>(options, composition);
    }
};

}

#endif
