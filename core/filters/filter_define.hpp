//
//  filter_define.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef filter_define_hpp
#define filter_define_hpp

#include <stdio.h>
#include <string>
#include <unordered_map>
#include "math/value/value_getter.hpp"
#include "ri/render_base.hpp"
#include "ri/render/render_pass.hpp"
#include "ri/render/mars_mesh.hpp"
#include "sceneData/data/common_properties.h"

const std::string GAUSSIAN_SEMANTIC_DIRECTION = "GAUSSIAN_DIR";
const std::string GAUSSIAN_SOURCE = "GAUSSIAN_SOURCE";

namespace mn {

struct ParticleFilterOptions {
    
    std::string vertex_shader;
    
    std::string fragment_shader;
    
    std::unordered_map<std::string, ValueGetter*> uniforms;
    
    std::unordered_map<std::string, std::shared_ptr<UniformValue>> uniform_values;
    
};

struct MeshFilterOptions {
    
    std::string vertex;
    
    std::string fragment;
    
    std::string shader_cache_id;
    
    MaterialRenderStates material_state;
    
    // unifrom_value和semantic_value定义到一起了；
    std::unordered_map<std::string, std::shared_ptr<UniformValue>> uniform_values;
        
    std::vector<std::string> uniform_variables;
    
};

class Composition;

class FilterDefine {
    
public:
    
    FilterDefine(Composition* composition, ItemFilterData* filter_data) : composition_(composition) {
        
    };
    
    virtual ~FilterDefine() {};
    
    virtual void OnItemUpdate() {};
    
    virtual void OnItemRemove() {};
    
    virtual void OnRenderPassCreated() {};
    
    virtual std::vector<std::shared_ptr<RenderPass>> GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) {
        std::vector<std::shared_ptr<RenderPass>> pre_passes;
        return pre_passes;
    };
    
    virtual void GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) {};
    
    MeshFilterOptions& GetMeshFilterOptions() {
        return mesh_filter_option_;
    }
    
    const std::string& GetShaderCacheId() {
        return mesh_filter_option_.shader_cache_id;
    }
    
    std::shared_ptr<RenderPassDelegate> GetRenderPassDelegate() {
        return delegate_;
    }
        
protected:
    
    ParticleFilterOptions particle_filter_option_;
    
    MeshFilterOptions mesh_filter_option_;
            
    Composition* composition_ = nullptr;
    
    std::shared_ptr<RenderPassDelegate> delegate_;
    
};

}

#endif /* filter_define_hpp */
