//
//  particle_system.hpp
//
//  Created by Zongming Liu on 2022/4/12.
//

#ifndef particle_system_hpp
#define particle_system_hpp

#include <stdio.h>
#include <string>
#include "math/vec3.hpp"
#include "math/vec4.hpp"

#include "util/util.hpp"
#include "math/value/value_getter.hpp"

#include "sceneData/mars_data_base.h"

#include "plugin/particle/shape/sphere.hpp"
#include "plugin/particle/shape/shape.hpp"

#include "ri/render/mars_mesh.hpp"
#include "plugin/particle/particle_mesh.hpp"
#include "plugin/particle/link_chain.hpp"
#include "math/transform.h"
#include "plugin/types.h"

namespace mn {

class ParticleSystem;
class ParticleVFXItem;
class VFXItem;

class ParticleSystemDestroyListener {
public:
    virtual void DestroyParticleItem(ParticleSystem* item) = 0;
};

class ParticleSystem : public BaseContentItem {
    
public:
    
    ParticleSystem(MarsRenderer* renderer, ParticleContentData* props, MeshRendererOptions* opt, ParticleVFXItem* vfx_item);
    
    ~ParticleSystem();
    
    void OnUpdate(float dt);
    
    void Start(ParticleSystemDestroyListener* destroy_listener);
    
    void Stop();
    
    void StopParticleEmission() {
        emission_stopped_ = true;
    }
    
    void ResumeParticleEmission() {
        emission_stopped_ = false;
    }
    
    std::vector<std::shared_ptr<MarsMesh>>& Meshes() {
        return meshes_;
    }
    
    void SetHide(bool hide);
    
    void SetParentTransform(std::shared_ptr<Transform> transform) {
        parent_transform_ = transform;
    }
    
private:
#ifdef UNITTEST
public:
#endif
    
    void SetOptions(ParticleContentData* props, VFXItem* vfx_item);
    
    void SetEmissionOption(ParticleContentData* props);
    
    void SetShapeOption(ParticleContentData* props);
    
    void SetTextureSheedAnimation(ParticleContentData* props);
    
    void SetSplits(ParticleContentData* props);
    
    void SetFilter();
    
    float TimePassed() {
        return this->last_update_time_ - this->loop_start_time_;
    }
    
    float LifeTime() {
        return (this->TimePassed()) / this->options_->duration;
    }
    
    void Reset();
    
    bool InitTextureUV(ParticleContentData* props);
    
    std::shared_ptr<MarsTexture> InnerInitTexture(MInt* texture, MeshRendererOptions* opt);
    
    std::unique_ptr<ParticlePoint> CreatePoint(float life_time);
    
    std::unique_ptr<ParticlePoint> InitPoint(const ShapeData& shape_data);
    
    void AddParticle(std::unique_ptr<ParticlePoint> point, size_t max_count);
    
    void GetParentMatrix();
    
    void UpdateEmitterTransform(float dt);
    
    bool ShouldSkipGenerate();
    
    void InnerOnDestroy();
    
    // todo: 好像没有啥作用
    void Raycast();
    
private:
#ifdef UNITTEST
public:
#endif
    bool reusable_ = false;
    Mat4 render_matrix_;
    ParticleMesh* particle_mesh_ = nullptr;
    ParticleStartOptions* options_ = nullptr;
    ParticleEmissionOptions* emission_ = nullptr;
    std::vector<std::shared_ptr<MarsMesh>> meshes_;
    TextureSheetAnimation* texture_sheet_animation_ = nullptr;
    bool emission_stopped_ = false;
    size_t generated_count_ = 0;
    
    float last_update_time_ = 0;
    float loop_start_time_ = 0;
    std::shared_ptr<LinkChain> particle_link_;
    std::shared_ptr<Transform> parent_transform_;
    bool started_ = false;
    bool ended_ = false;
    float last_emit_time_ = 0;
    bool freezed_ = false;
    Vec3* up_direction_world_ = nullptr;
    std::vector<Vec4> uvs_;
    ParticleTransform* inner_transform_formula_ = nullptr;
    std::shared_ptr<Transform> transform_;
    Mat4 world_matrix_;
    
    std::string name_;
    
    ParticleMeshConstructor* mesh_options_ = nullptr;
    ParticleShapeOption* shape_ = nullptr;
    
    bool destroy_on_update_ = false;
    std::shared_ptr<Shape> particle_shape_;
    ParticleSystemDestroyListener* destroy_listener_ = nullptr;
    
    friend class ParticleMesh;
    friend class ParticleTranslateTarget;
};

}

#endif /* particle_system_hpp */
