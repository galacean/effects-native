//
//  m_material_pbr.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "m_material_pbr.hpp"

namespace mn {

MMaterialPBR::MMaterialPBR(std::shared_ptr<MarsMaterialPBROptions> material_options) : MMaterialBase(material_options) {
    if (material_options->metallic_roughness_texture) {
        metallic_roughness_texture_ = material_options->metallic_roughness_texture;
    }
    
    metallic_factor_ = material_options->metallic_factor;
    roughness_factor_ = material_options->roughness_factor;
    
    if (material_options->normal_texture) {
        normal_texture_ = material_options->normal_texture;
    }
    normal_texture_scale_ = material_options->normal_texture_scale;
    
    if (material_options->occlusion_texture) {
        occlusion_texture_ = material_options->occlusion_texture;
    }
    occlusion_texture_strength_ = 1.0;
    
    if (material_options->emissive_texture) {
        emissive_texture_ = material_options->emissive_texture;
    }
    emissive_factor_.Set(material_options->emissive_factor.m[0], material_options->emissive_factor.m[1], material_options->emissive_factor.m[2]);
    emissive_intensity_ = material_options->emissive_intensity;
    
    
}

MMaterialPBR::~MMaterialPBR() {
    
}

void MMaterialPBR::SetUpUniforms(std::shared_ptr<MarsMaterial> material) {
    
    MMaterialBase::SetUpUniforms(material);
    
    std::shared_ptr<UniformValue> u_metallic_factor_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
    u_metallic_factor_value->SetData(metallic_factor_, 0);
    material->SetUniformValue("u_MetallicFactor", u_metallic_factor_value);
    
    std::shared_ptr<UniformValue> u_roughness_factor_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
    u_roughness_factor_value->SetData(roughness_factor_, 0);
    material->SetUniformValue("u_RoughnessFactor", u_roughness_factor_value);
    
    Mat3 uv_transform = Mat3::IDENTITY;
    
    if (metallic_roughness_texture_) {
        std::shared_ptr<UniformValue> u_metallic_roughness_sampler_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
        u_metallic_roughness_sampler_value->SetTexture(metallic_roughness_texture_);
        material->SetUniformValue("u_MetallicRoughnessSampler", u_metallic_roughness_sampler_value);
        
        std::shared_ptr<UniformValue> u_metallic_roughness_uvset_value = std::make_shared<UniformValue>(UniformDataType::INT);
        u_metallic_roughness_uvset_value->SetData((float) 0, 0);
        material->SetUniformValue("u_MetallicRoughnessUVSet", u_metallic_roughness_uvset_value);
        
        std::shared_ptr<UniformValue> u_metallic_roughness_uv_transform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT3);
        u_metallic_roughness_uv_transform_value->SetData(uv_transform);
        material->SetUniformValue("u_MetallicRoughnessUVTransform", u_metallic_roughness_uv_transform_value);
    }
    
    if (normal_texture_) {
        std::shared_ptr<UniformValue> u_normal_sampler_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
        u_normal_sampler_value->SetTexture(normal_texture_);
        material->SetUniformValue("u_NormalSampler", u_normal_sampler_value);
        
        std::shared_ptr<UniformValue> u_normal_scale_value = std::make_shared<UniformValue>(UniformDataType::INT);
        u_normal_scale_value->SetData(normal_texture_scale_, 0);
        material->SetUniformValue("u_NormalScale", u_normal_scale_value);
        
        std::shared_ptr<UniformValue> u_normal_uv_set_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
        u_normal_uv_set_value->SetData((float) 0, 0);
        material->SetUniformValue("u_NormalUVSet", u_normal_uv_set_value);
        
        std::shared_ptr<UniformValue> u_normal_uv_transform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT3);
        u_normal_uv_transform_value->SetData(uv_transform);
        material->SetUniformValue("u_NormalUVTransform", u_normal_uv_transform_value);
    }
    
    if (occlusion_texture_) {
        std::shared_ptr<UniformValue> u_occlusion_sampler_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
        u_occlusion_sampler_value->SetTexture(occlusion_texture_);
        material->SetUniformValue("u_OcclusionSampler", u_occlusion_sampler_value);
        
        std::shared_ptr<UniformValue> u_occlusion_stength_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
        u_occlusion_stength_value->SetData(occlusion_texture_strength_, 0);
        material->SetUniformValue("u_OcclusionStrength", u_occlusion_stength_value);
        
        std::shared_ptr<UniformValue> u_occlusion_uv_set_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
        u_occlusion_uv_set_value->SetData((float) 0, 0);
        material->SetUniformValue("u_OcclusionUVSet", u_occlusion_uv_set_value);
        
        std::shared_ptr<UniformValue> u_occlusion_uv_transform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT3);
        u_occlusion_uv_transform_value->SetData(uv_transform);
        material->SetUniformValue("u_OcclusionUVTransform", u_occlusion_uv_transform_value);
    }
    
    if (emissive_texture_) {
        std::shared_ptr<UniformValue> u_emissive_sampler_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
        u_emissive_sampler_value->SetTexture(emissive_texture_);
        material->SetUniformValue("u_EmissiveSampler", u_emissive_sampler_value);
        
        std::shared_ptr<UniformValue> u_emissive_uv_set_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
        u_emissive_uv_set_value->SetData((float) 0, 0);
        material->SetUniformValue("u_EmissiveUVSet", u_emissive_uv_set_value);
        
        std::shared_ptr<UniformValue> u_emissive_uv_transform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT3);
        u_emissive_uv_transform_value->SetData(uv_transform);
        material->SetUniformValue("u_EmissiveUVTransform", u_emissive_uv_transform_value);
    }
    
    std::shared_ptr<UniformValue> u_emissive_factor_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC3);
    u_emissive_factor_value->SetData(emissive_factor_, 0);
    material->SetUniformValue("u_EmissiveFactor", u_emissive_factor_value);
    
    auto u_exposure_value = material->GetUniformValue("u_Exposure");
    if (u_exposure_value) {
        u_exposure_value->SetData(3.0, 0);
    }
}

// todo: update;
void MMaterialPBR::UpdateUniforms(std::shared_ptr<MarsMaterial> material) {
    
}

void MMaterialPBR::GetShaderFeatures(std::vector<std::string>& feature_list) {    
    feature_list.push_back("MATERIAL_METALLICROUGHNESS 1");
    
    if (base_color_texture_) {
        feature_list.push_back("HAS_BASE_COLOR_MAP 1");
    }
    
    if (metallic_roughness_texture_) {
        feature_list.push_back("HAS_METALLIC_ROUGHNESS_MAP 1");
    }
    
    if (normal_texture_) {
        feature_list.push_back("HAS_NORMAL_MAP 1");
    }
    
    if (occlusion_texture_) {
        feature_list.push_back("HAS_OCCLUSION_MAP 1");
    }
    
    if (emissive_texture_) {
        feature_list.push_back("HAS_EMISSIVE_MAP 1");
    } else if (this->HasEmissiveFactor()) {
        feature_list.push_back("HAS_EMISSIVE 1");
    }
}

bool MMaterialPBR::HasEmissiveFactor() {
    return emissive_factor_.m[0] + emissive_factor_.m[1] + emissive_factor_.m[2] > 0;
}

}
