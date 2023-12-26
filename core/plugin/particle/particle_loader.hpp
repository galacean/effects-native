//
//  particle_loader.hpp
//  PlayGroundIOS
//
//  Created by Zongming Liu on 2022/4/13.
//

#ifndef particle_loader_hpp
#define particle_loader_hpp

#include <stdio.h>
#include <vector>
#include <string>

#include "player/plugin_system.h"
#include "plugin/particle/particle_system.hpp"
#include "plugin/particle/particle_vfx_item.hpp"

namespace mn {

class ParticleLoader : public MarsPlugin {

public:
    
    ParticleLoader();

    ~ParticleLoader();
    
    void AddParticle(ParticleSystem* particle_system);
    
    void RemoveParticle(ParticleSystem* particle_system, MarsPlayerRenderFrame* render_frame);

    void OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) override;

    void OnCompositionUpdate(Composition* comp, float dt) override;

    bool PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) override;
    
private:
    
    std::vector<std::weak_ptr<MarsMesh>> meshes_to_add_;
    
};


class ParticlePluginBuilder : public MarsPluginBuilder {
public:
    MarsPlugin* CreatePlugin() override {
        return new ParticleLoader;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        return std::make_shared<ParticleVFXItem>(options, composition);
    }
};

}

#endif /* particle_loader_hpp */
