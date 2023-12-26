//
//  m_material_skybox.cpp
//
//  Created by Zongming Liu on 2023/2/9.
//

#include "m_material_skybox.hpp"

namespace mn {

MMaterialSkybox::MMaterialSkybox() {
    type_ = MarsMaterialType::SKYBOX;
}

MMaterialSkybox::~MMaterialSkybox() {
    
}

void MMaterialSkybox::Create(MSkyBox* skybox) {
    intensity_ = skybox->GetIntensity();
    reflection_intensity_ = skybox->GetReflectionIntensity();
    
    diffuse_image_texture_ = skybox->GetDiffuseImageTexture();
    specular_image_texture_ = skybox->GetSpecularEnvSamplerTexutre();
    
    irradiance_coeffs_ = skybox->GetIrradianceCoeffs();
    brdf_lut_texture_ = skybox->GetBrdfLutTexture();
}

void MMaterialSkybox::SetUpUniforms(std::shared_ptr<MarsMaterial> material) {

    auto inv_view_project_matrix_uniform = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT4);
    material->SetUniformValue("u_InvViewProjectionMatrix", inv_view_project_matrix_uniform);
    
    auto ibl_intensity_u_value = material->CreateUniformValueIfNot("u_IblIntensity", UniformDataType::FLOAT_VEC2);

    Vec2 intensity_value_tmp(intensity_, reflection_intensity_);
    ibl_intensity_u_value->SetData(intensity_value_tmp, 0);
    
    auto brdf_texture_u_value = material->CreateUniformValueIfNot("u_brdfLUT", UniformDataType::SAMPLER_2D);
    brdf_texture_u_value->SetTexture(brdf_lut_texture_);
    
    if (diffuse_image_texture_) {
        auto diffuse_image_texture_u_value = material->CreateUniformValueIfNot("u_DiffuseEnvSampler", UniformDataType::SAMPLER_CUBE);
        diffuse_image_texture_u_value->SetTexture(diffuse_image_texture_);
    } else {
        // todo: coeffs;
        assert(false);
    }
    
    auto mipmap_count_u_value = material->CreateUniformValueIfNot("u_MipCount", UniformDataType::INT);
    mipmap_count_u_value->SetData(specular_mip_count_);
    
    auto specular_env_u_value = material->CreateUniformValueIfNot("u_SpecularEnvSampler", UniformDataType::SAMPLER_CUBE);
    specular_env_u_value->SetTexture(specular_image_texture_);
    
}

void MMaterialSkybox::UpdateUniforms(std::shared_ptr<MarsMaterial> material) {
    
}

}
