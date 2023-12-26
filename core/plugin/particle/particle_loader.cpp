//
//  particle_loader.cpp
//
//  Created by Zongming Liu on 2022/4/13.
//

#include "particle_loader.hpp"
#include "plugin/particle/particle_vfx_item.hpp"
#include "ri/render/mars_player_render_frame.h"
#include "player/composition.h"

namespace mn {

ParticleLoader::ParticleLoader() {
    name_ = "ParticleLoader";
}

ParticleLoader::~ParticleLoader() {
    
}

void ParticleLoader::AddParticle(ParticleSystem* particle_system) {
    if (particle_system) {
        auto& meshes = particle_system->Meshes();
        for (auto& mesh : meshes) {
            meshes_to_add_.push_back(mesh);
        }
    }
}

void ParticleLoader::RemoveParticle(ParticleSystem* particle_system, MarsPlayerRenderFrame* render_frame) {
    if (particle_system) {
        auto& meshes = particle_system->Meshes();
        for (auto& mesh : meshes) {
            for (auto iter = meshes_to_add_.begin(); iter != meshes_to_add_.end();) {
                auto iter_mesh = (*iter).lock();
                if (iter_mesh == mesh) {
                    meshes_to_add_.erase(iter);
                    break;
                } else {
                    ++iter;
                }
            }
            render_frame->RemoveMeshToDefaultRenderPass(mesh);
        }
    }
}

void ParticleLoader::OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) {
//    MLOGD("ParticleLoader::%s", __FUNCTION__);
    if (item->GetType().compare(VFX_ITEM_TYPE_PARTICLE) == 0) {
        if (item->content_) {
            this->AddParticle((ParticleSystem*) item->content_);
        }
    }
}

void ParticleLoader::OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) {
    // MLOGD("ParticleLoader::%s", __FUNCTION__);
    if (item->GetType().compare(VFX_ITEM_TYPE_PARTICLE) == 0) {
        if (item->content_) {
            this->RemoveParticle((ParticleSystem*)item->content_, comp->render_frame_);
        }
    }
}

void ParticleLoader::OnCompositionUpdate(Composition* comp, float dt) {
//    MLOGD("ParticleLoader::%s", __FUNCTION__);
}

bool ParticleLoader::PrepareRenderFrame(Composition* comp, MarsPlayerRenderFrame* render_frame) {
//    MLOGD("ParticleLoader::%s renderMesh size: %d", __FUNCTION__, meshes_to_add_.size());
    for (auto& mesh : meshes_to_add_) {
        auto temp = mesh.lock();
        render_frame->AddMeshToDefaultRenderPass(temp);
    }
    
    this->meshes_to_add_.clear();
    return false;
}

}
