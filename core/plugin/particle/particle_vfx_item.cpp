//
//  particle_vfx_item.cpp
//
//  Created by Zongming Liu on 2022/4/15.
//

#include "particle_vfx_item.hpp"
#include "player/composition.h"

namespace mn {

ParticleVFXItem::ParticleVFXItem(ItemData* options, Composition* composition) {
    Create(options, composition);
}

ParticleVFXItem::~ParticleVFXItem() {
    
}

void ParticleVFXItem::OnConstructed(ItemData* options) {
    particle_ = (ParticleContentData*) options->content;
}

BaseContentItem* ParticleVFXItem::InnerCreateContent() {
    if (particle_) {
        MeshRendererOptions* opt = composition_->GetParticleOptions();
        return new ParticleSystem(composition_->renderer_, particle_, opt, this);
    }
    return nullptr;
}

void ParticleVFXItem::OnLifeTimeBegin(Composition* renderer, void* content) {
    if (content) {
        ParticleSystem* particle_system = (ParticleSystem*)content;
        particle_system->Start(this);
        
        // todo destroy;
    }
}

void ParticleVFXItem::OnItemRemoved(Composition* renderer, void* content) {
    ParticleSystem* particle_system = (ParticleSystem*) content;
    if (particle_system) {
        auto& meshes = particle_system->Meshes();
        for (auto& mesh : meshes) {
            mesh->Destroy();
        }
    }
}

void ParticleVFXItem::Precompile(GLShaderLibrary* shader_library) {
    CreateContent();
//    ParticleSystem* particle_system = (ParticleSystem*)this->content_;
//    particle_system->ParticleMesh()->GetMesh()->AssignRenderer(composition_->renderer_);

    // todo: trailMesh;
}

void ParticleVFXItem::OnItemUpdate(float dt, float lifetime) {
    if (content_) {
        bool hide = this->hide_;
        ParticleSystem* particle_system = (ParticleSystem*) content_;
        if (!hide && !this->parent_id_.empty()) {
            SpriteRenderData* parent_data = composition_->calculate_group_->GetRenderData(this->parent_id_);
            if (parent_data) {
                particle_system->SetParentTransform(parent_data->transform);
                if (parent_data->hide) {
                    hide = true;
                }
            }
        }

        if (hide) {
            particle_system->SetHide(true);
        } else {
            particle_system->SetHide(false);
            particle_system->OnUpdate(dt);
        }
    }
}

bool ParticleVFXItem::IsEnded(float now) {
    return VFXItem::IsEnded(now) && particle_destroyed_;;
}

void ParticleVFXItem::DestroyParticleItem(ParticleSystem* item) {
    particle_destroyed_ = true;
}

bool ParticleVFXItem::GetContentVisible() {
    return !particle_destroyed_;
}

}
