//
//  m_skybox.hpp
//
//  Created by Zongming Liu on 2022/10/28.
//

#ifndef m_skybox_hpp
#define m_skybox_hpp

#include <stdio.h>
#include "model/renderer/m_entity.hpp"
#include "ri/render/mars_texture.hpp"
#include "model/model_vfx_item.hpp"

namespace mn {

class MMaterialSkybox;

class MSkyBox : public MEntity {
    
public:

    static std::shared_ptr<MarsTexture> CreateBrdfLutTexture(MarsRenderer* renderer);
    
    MSkyBox(MarsRenderer* renderer);
    
    ~MSkyBox();
    
    MEntityType GetType() {
        return MEntityType::Skybox;
    }
        
    void Build();
    
    void Create(std::shared_ptr<MarsItemSkybox> sky_box, std::shared_ptr<MarsTexture> brdf_lut, std::shared_ptr<ModelVFXItem> owner_item);
    
    bool IsAvailable();
    
    void AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set) override;

    void UpdateUniformsForScene(const MSceneState& scene_state) override;
    
    std::shared_ptr<MarsTexture> GetBrdfLutTexture();
    
    std::shared_ptr<MarsTexture> GetDiffuseImageTexture();
    
    std::shared_ptr<MarsTexture> GetSpecularEnvSamplerTexutre();
    
    int GetMipCount();
    
    float GetIntensity();
    
    float GetReflectionIntensity();
    
    const std::vector<Vec3>& GetIrradianceCoeffs() {
        return irradiance_coeffs_;
    }
    
    void GetShaderFeatures(std::vector<std::string>& feature_list);
    
private:
    
    void TransformIrradianceCoeffs(const std::vector<std::vector<float>>& irradiance_coffes);
    
    float intensity_;
    
    float reflection_intensity_;
    
    std::shared_ptr<ModelVFXItem> owner_item_;

    std::shared_ptr<MarsTexture> diffuse_image_;
    
    std::shared_ptr<MarsTexture> specular_image_;
    
    float specular_image_size_;
    
    int specular_mip_count_;
    
    std::shared_ptr<MarsTexture> brdf_lut_;
    
    std::vector<Vec3> irradiance_coeffs_;
    
    std::shared_ptr<MarsMesh> skybox_mesh_;
    
    std::shared_ptr<MMaterialSkybox> skybox_material_;
    
    bool renderable_;
    
    MarsRenderer* renderer_;
    
};

}

#endif /* m_skybox_hpp */
