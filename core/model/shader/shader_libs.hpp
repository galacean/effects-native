//
//  shader_libs.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef shader_libs_hpp
#define shader_libs_hpp

#include <stdio.h>
#include <vector>
#include "model/renderer/material/m_material_base.hpp"


namespace mn {

struct MShaderContext {
    MMaterialBase* material_base;
    std::vector<std::string> feature_list;
};

class ShaderLibs {
    
public:
    
    static void GenShaderCode(const MShaderContext& context, std::string& vertex_shader, std::string& fragment_shader);
    
private:
    
    static void ShaderSourceBuild(std::string& vertex_shader, std::vector<std::string> feature_list);
    
};

}

#endif /* shader_libs_hpp */
