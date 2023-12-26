#ifndef MN_PLUGIN_SPRITE_SPRITE_LOADER_H_
#define MN_PLUGIN_SPRITE_SPRITE_LOADER_H_

#include "player/plugin_system.h"
#include "sprite_vfx_item.h"
#include "filter_sprite_vfx_item.hpp"
#include "ri/render_base.hpp"
#include "plugin/sprite/sprite_mesh.h"

namespace mn {

class FilterDefine;

class SpriteLoader : public MarsPlugin {
public:
    SpriteLoader();

    ~SpriteLoader();
    
    void SpriteMeshShaderFromFilter(Shader& shader, FilterDefine* filter_define);

    void OnCompositionDestroyed(Composition* comp) override;

    void OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* render_frame) override;

    void OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionUpdate(Composition* comp, float dt) override;

    bool PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) override;
    
    void PostProcessFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) override;
    
private:
    
    std::vector<std::shared_ptr<SpriteMesh>> layer_info_;
};

class SpritePluginBuilder : public MarsPluginBuilder {
public:
    MarsPlugin* CreatePlugin() override {
        return new SpriteLoader;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        std::shared_ptr<VFXItem> item;
        if (options->type == 1) {
            item = std::make_shared<SpriteVFXItem>(options, composition);
        } else if (options->type == 8) {
            item = std::make_shared<FilterSpriteVFXItem>(options, composition);
        } else {
            assert(0);
        }
        item->Create(options, composition);
        return item;
    }
};

}

#endif
