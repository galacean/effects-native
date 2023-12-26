//
//  mars_material.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef mars_material_hpp
#define mars_material_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "ri/backend/opengl/gl_material.hpp"
#include "ri/render/mars_renderer.hpp"
#include "ri/render_base.hpp"

namespace mn {

// 1. states; 2. uniform values;

// MarsMaterial 构建：
// 1. Constructor，Shader & MaterialRenderStates
// 2. SetUniformValues;

class MarsMaterial {
    
public:
    
    // todo: 10.2.30重构掉;
    MarsMaterial(MarsRenderer *renderer, const MaterialOptions& material_option);
    
    MarsMaterial(MarsRenderer* renderer, const Shader& shader, const MaterialRenderStates& states, const std::string& name = "Material");
    
    ~MarsMaterial();
    
    bool HasUnifromValue(const std::string& name);
    
    void SetUniformValue(const std::string& name, std::shared_ptr<UniformValue> value);
    
    void SetUniformValues(const std::unordered_map<std::string, std::shared_ptr<UniformValue>> uniform_values);
        
    std::shared_ptr<UniformValue> GetUniformValue(const std::string& name);
    
    std::shared_ptr<UniformValue> CreateUniformValueIfNot(const std::string& name, UniformDataType data_type);

    std::unordered_map<std::string, std::shared_ptr<UniformValue>>& GetUniformValues();
    
    // ??
    // void SetUniformValues();
    void AssignRenderer(MarsRenderer *renderer);
    
    GLMaterial* GetGLMaterial() {
        return inner_material_;
    }
    
public:
    
    std::shared_ptr<MaterialRenderStates> states_;
    
private:
#ifdef UNITTEST
public:
#endif
    
    std::shared_ptr<MaterialRenderStates> CreateMaterialStates(const MaterialRenderStates& states);

    void AssignUniformValueRenderer(std::shared_ptr<UniformValue> value);
    
    std::string name_;
    
    Shader shader_;
        
    MarsRenderer *renderer_;
    
    std::string shader_cached_id_;
    
    std::unordered_map<std::string, std::shared_ptr<UniformValue>> uniform_values_;
        
    GLMaterial *inner_material_;
    
};

}

#endif /* mars_material_hpp */
