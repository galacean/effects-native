//
//  skybox_utils.hpp
//
//  Created by Zongming Liu on 2023/2/10.
//

#ifndef skybox_utils_hpp
#define skybox_utils_hpp

#include <stdio.h>
#include "model/renderer/material/m_material_skybox.hpp"
#include "ri/render/mars_mesh.hpp"

namespace mn {

class SkyboxUtils {
  
public:
    
    static std::shared_ptr<MarsMesh> CreateSkyboxMesh(const std::string& name, MarsRenderer* renderer, std::shared_ptr<MMaterialSkybox> skybox_material);
    
private:
    
    
};

}

#endif /* skybox_utils_hpp */
