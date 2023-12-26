//
//  m_material_pbr.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef m_material_pbr_hpp
#define m_material_pbr_hpp

#include <stdio.h>
#include "model/renderer/material/m_material_base.hpp"

namespace mn {

class MMaterialPBR : public MMaterialBase{
    
public:
    
    MMaterialPBR(std::shared_ptr<MarsMaterialPBROptions> material_options);
    
    ~MMaterialPBR();
    
    void SetUpUniforms(std::shared_ptr<MarsMaterial> material) override;
    
    void UpdateUniforms(std::shared_ptr<MarsMaterial> material) override;
    
    void GetShaderFeatures(std::vector<std::string>& feature_list) override;
    
private:
    
    bool HasEmissiveFactor();
    
    std::shared_ptr<MarsTexture> metallic_roughness_texture_;
    
    float metallic_factor_;
    
    float roughness_factor_;
    
    std::shared_ptr<MarsTexture> normal_texture_;
    
    float normal_texture_scale_;
    
    std::shared_ptr<MarsTexture> occlusion_texture_;
    
    float occlusion_texture_strength_;
    
    std::shared_ptr<MarsTexture> emissive_texture_;
    
    Vec3 emissive_factor_;
    
    float emissive_intensity_;

};

}

#endif /* m_material_pbr_hpp */
