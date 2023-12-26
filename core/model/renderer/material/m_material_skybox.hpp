//
//  m_material_skybox.hpp
//
//  Created by Zongming Liu on 2023/2/9.
//

#ifndef m_material_skybox_hpp
#define m_material_skybox_hpp

#include <stdio.h>
#include "model/renderer/material/m_material_base.hpp"
#include "model/renderer/m_skybox.hpp"

namespace mn {

class MMaterialSkybox : public MMaterialBase {
    
public:
    
    MMaterialSkybox();
    
    ~MMaterialSkybox();
    
    void Create(MSkyBox* skybox);
    
    void SetUpUniforms(std::shared_ptr<MarsMaterial> material) override;
    
    void UpdateUniforms(std::shared_ptr<MarsMaterial> material) override;
        
private:
    
    std::shared_ptr<MarsTexture> brdf_lut_texture_;
    
    std::shared_ptr<MarsTexture> diffuse_image_texture_;
    
    std::shared_ptr<MarsTexture> specular_image_texture_;
    
    int specular_mip_count_;
    
    float intensity_;
    
    float reflection_intensity_;
    
    std::vector<Vec3> irradiance_coeffs_;
    
};

}

#endif /* m_material_skybox_hpp */
