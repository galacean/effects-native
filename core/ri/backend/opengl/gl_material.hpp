//
//  gl_material.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_material_hpp
#define gl_material_hpp

#include <stdio.h>
#include <string>
#include "ri/backend/opengl/gl_api.hpp"

#include "ri/backend/opengl/gl_gpu_renderer.hpp"
#include "ri/backend/opengl/gl_program.hpp"
#include "ri/render_base.hpp"

namespace mn {

class MarsMaterial;

class GLMaterial {
    
public:
    
    GLMaterial(GLGPURenderer *renderer, std::shared_ptr<MaterialRenderStates> material_render_state, const std::string& cache_id);
    
    ~GLMaterial();
    
    void SetUpStates();
    
    GLProgram* GetProgram();
    
private:
    
    GLGPURenderer *renderer_;
    
    std::string shader_cache_id_;
    
    // 和material的states是同一个对象
    std::shared_ptr<MaterialRenderStates> states_;
    
};

}

#endif /* gl_material_hpp */
