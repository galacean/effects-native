//
//  particle_mesh.hpp
//
//  Created by Zongming Liu on 2022/4/12.
//

#ifndef particle_mesh_hpp
#define particle_mesh_hpp

#include <stdio.h>
#include <string>
#include "math/vec3.hpp"
#include "math/transform.h"
#include "math/value/value_getter.hpp"
#include "ri/render/mars_mesh.hpp"
#include "plugin/types.h"

namespace mn {

class ParticleSystem;

struct ParticlePoint {
    
    ParticlePoint() {};
    ~ParticlePoint() {};
    
    Vec3 pos;
    Vec3 vel;
    Vec3 rot;
    Vec4 color;
    Vec2 size;
    Vec3 dir_x;
    Vec3 dir_y;
    Vec4 uv;
    float lifetime;
    float delay;
    Vec3 sprite;
};

class ParticleMesh {
  
public:
    
    ParticleMesh(ParticleMeshConstructor* props, MeshRendererOptions* opt);

    ~ParticleMesh();

    void SetPoint(std::unique_ptr<ParticlePoint> point, size_t index);
    
    float GetTime();

    void SetTime(float now);
    
    void ReverseTime(float time);
    
    void ClearPoints();
    
    void RemovePoint(size_t index);
    
    std::shared_ptr<MarsMesh> GetMesh() {
        return mesh_;
    }
    
    void GetPointPosition(Vec3& out, int index);
    
private:
    void CreateGrometry(MarsRenderer* renderer, bool use_sprite);
        
private:
#ifdef UNITTEST
public:
#endif
    
    float duration_ = 0;
    
    std::shared_ptr<MarsGeometry> geometry_;
    
    std::shared_ptr<MarsMesh> mesh_;
    
    size_t particle_count_ = 0;
    
    size_t max_particle_buffer_count_ = 0;
    
    bool use_sprite_ = false;
    
    std::vector<float> texture_offsets_;
    
    size_t max_count_ = 0;
    
    Vec2 anchor_;
    
    ParticleMeshConstructor* props_ref_ = nullptr;
};

}

#endif /* particle_mesh_hpp */
