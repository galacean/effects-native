//
//  particle_vfx_item.hpp
//
//  Created by Zongming Liu on 2022/4/15.
//

#ifndef particle_vfx_item_hpp
#define particle_vfx_item_hpp

#include <stdio.h>
#include "player/vfx_item.h"
#include "plugin/particle/particle_system.hpp"

namespace mn {

class ParticleVFXItem : public VFXItem, public ParticleSystemDestroyListener {
    
public:
    
    ParticleVFXItem(ItemData* options, Composition* composition);

    ~ParticleVFXItem();
    
    std::string GetType() const override {
        return VFX_ITEM_TYPE_PARTICLE;
    }
    
    void OnConstructed(ItemData* arg) override;
    
    BaseContentItem* InnerCreateContent() override;
    
    void OnLifeTimeBegin(Composition* renderer, void* content) override;
    
    void OnItemRemoved(Composition* renderer, void* content) override;
    
    void Precompile(GLShaderLibrary* shader_library) override;
    
    void OnItemUpdate(float dt, float lifetime) override;

    bool IsEnded(float now) override;
    
    void DestroyParticleItem(ParticleSystem* item) override;
    
    bool GetContentVisible() override;
    
private:
    
    ParticleContentData* particle_ = nullptr;
    
    bool particle_destroyed_ = false;

};

}


#endif /* particle_vfx_item_hpp */
