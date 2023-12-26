//
//  shader_libs.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "shader_libs.hpp"
#include "model/shader/glsl/primitive_frag_glsl.h"
#include "model/shader/glsl/primitive_vert_glsl.h"
#include "model/shader/glsl/animation_glsl.h"
#include "model/shader/glsl/texture_glsl.h"
#include "model/shader/glsl/metallic_roughness_frag_glsl.h"
#include "model/shader/glsl/functions_glsl.h"
#include "model/shader/glsl/tone_mapping_glsl.h"
#include "model/shader/glsl/skybox_frag_glsl.h"
#include "model/shader/glsl/skybox_vert_glsl.h"

namespace mn {

void ShaderLibs::GenShaderCode(const MShaderContext& context, std::string& vertex_shader, std::string& fragment_shader) {
    MarsMaterialType type =  context.material_base->GetMaterialType();
    
    if (type == MarsMaterialType::UNLit || type == MarsMaterialType::PBR) {
        fragment_shader = METALLIC_ROUGHNESS_FRAG;
        vertex_shader = PRIMITIVE_VERT;
    } else if (type == MarsMaterialType::SKYBOX) {
        fragment_shader = SKY_BOX_FRAG_GLSL;
        vertex_shader = SKY_BOX_VERT_GLSL;
    }
    
    ShaderLibs::ShaderSourceBuild(vertex_shader, context.feature_list);
    ShaderLibs::ShaderSourceBuild(fragment_shader, context.feature_list);
}

// todo: 
void ShaderLibs::ShaderSourceBuild(std::string& shader, std::vector<std::string> feature_list) {
    std::string feature_string = "";
    for (auto iter = feature_list.cbegin(); iter != feature_list.cend(); iter++) {
        feature_string += "#define " + *iter + "\n";
    }

    if (shader.find("#define FEATURES") != shader.npos) {
        shader.replace(shader.find("#define FEATURES"), sizeof("#define FEATURES"), feature_string);
    }
    
    if (shader.find("#include ANIMATION_GLSL") != shader.npos) {
        shader.replace(shader.find("#include ANIMATION_GLSL"), sizeof("#include ANIMATION_GLSL"), ANIMATION_GLSL);
    }
    
    if (shader.find("#include TEXTURE_GLSL") != shader.npos) {
        shader.replace(shader.find("#include TEXTURE_GLSL"), sizeof("#include TEXTURE_GLSL"), TEXTURE_GLSL);
    }
    
    if (shader.find("#include FUNCTION_GLSL") != shader.npos) {
        shader.replace(shader.find("#include FUNCTION_GLSL"), sizeof("#include FUNCTION_GLSL"), FUNCTION_GLSL);
    }
    
    if (shader.find("#include TONE_MAPPING_GLSL") != shader.npos) {
        shader.replace(shader.find("#include TONE_MAPPING_GLSL"), sizeof("#include TONE_MAPPING_GLSL"), TONE_MAPPING_GLSL);
    }
}

}

