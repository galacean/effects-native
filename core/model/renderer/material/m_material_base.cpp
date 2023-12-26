//
//  m_material_base.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "m_material_base.hpp"
#include "model/shader/shader_libs.hpp"

namespace mn {

// todo: check code
MMaterialBase::MMaterialBase() {
    
}

MMaterialBase::MMaterialBase(std::shared_ptr<MarsMaterialOptions> material_options) {
    base_color_texture_ = material_options->base_color_texture;
    base_color_factor_ = material_options->base_color_factor;
    blend_mode_ = material_options->blending;
    side_mode_ = material_options->side;
    name_ = material_options->name;
    type_ = material_options->type;
    DEBUG_MLOGD("MateirialBase Constructed: %s" , name_.data());
}

MMaterialBase::~MMaterialBase() {
    DEBUG_MLOGD("MMaterialBase Destructed %s", name_.data());
}

void MMaterialBase::Build(std::vector<std::string> feature_list) {
    MShaderContext context;
    this->GetShaderFeatures(feature_list);
    auto& final_feature_list = context.feature_list;
    final_feature_list.insert(final_feature_list.cend(), feature_list.cbegin(), feature_list.cend());
    context.material_base = this;
    
    ShaderLibs::GenShaderCode(context, vertex_shader_, frag_shader_);
}

std::shared_ptr<MaterialRenderStates> MMaterialBase::GetRenderState() {
    std::shared_ptr<MaterialRenderStates> render_state = std::make_shared<MaterialRenderStates>();
        if (blend_mode_ == MarsMaterialBlending::TRANSLUCENT || blend_mode_ == MarsMaterialBlending::ADDITIVE) {
            render_state->blending = std::make_shared<MBool>(true);
            render_state->depth_test = std::make_shared<MBool>(true);;
            render_state->depth_mask = std::make_shared<MBool>(true);;
    
            render_state->blend_equation_rgb = std::make_shared<MUint>(GL_FUNC_ADD);
            render_state->blend_equation_alpha = std::make_shared<MUint>(GL_FUNC_ADD);
    
            if (blend_mode_ == MarsMaterialBlending::TRANSLUCENT) {
                render_state->blend_src = std::make_shared<MUint>(GL_ONE);
                render_state->blend_dst = std::make_shared<MUint>(GL_ONE_MINUS_SRC_ALPHA);
            } else {
                render_state->blend_src = std::make_shared<MUint>(GL_ONE);
                render_state->blend_dst = std::make_shared<MUint>(GL_ONE);
            }
        } else {
            render_state->blending = std::make_shared<MBool>(false);
            render_state->depth_test = std::make_shared<MBool>(true);
            render_state->depth_mask = std::make_shared<MBool>(true);
        }
    
        if (side_mode_ == MarsMaterialSide::BOTH) {
            render_state->cull_face_enabled = std::make_shared<MBool>(false);
        } else if (side_mode_ ==  MarsMaterialSide::BACK) {
            render_state->cull_face_enabled = std::make_shared<MBool>(true);
            render_state->cull_face = std::make_shared<MUint>(GL_FRONT);
            render_state->front_face = std::make_shared<MUint>(GL_CCW);
        } else {
            render_state->cull_face_enabled = std::make_shared<MBool>(true);
            render_state->cull_face = std::make_shared<MUint>(GL_BACK);
            render_state->front_face = std::make_shared<MUint>(GL_CCW);
        }
    
    return render_state;
}

void MMaterialBase::GetShaderFeatures(std::vector<std::string>& feature_list) {    
    feature_list.push_back("MATERIAL_METALLICROUGHNESS 1");
    
    if (base_color_texture_) {
        feature_list.push_back("HAS_BASE_COLOR_MAP 1");
    }
    
    feature_list.push_back("MATERIAL_UNLIT 1");
}

void MMaterialBase::SetUpUniforms(std::shared_ptr<MarsMaterial> material) {
    std::shared_ptr<UniformValue> u_base_color_factor = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    u_base_color_factor->SetData(base_color_factor_, 0);
    material->SetUniformValue("u_BaseColorFactor", u_base_color_factor);
    
    if (base_color_texture_) {
        std::shared_ptr<UniformValue> u_base_color_texture = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
        u_base_color_texture->SetTexture(base_color_texture_);
        material->SetUniformValue("u_BaseColorSampler", u_base_color_texture);
        
        std::shared_ptr<UniformValue> u_base_color_uv_set = std::make_shared<UniformValue>(UniformDataType::INT);
        u_base_color_uv_set->SetData((int)0, 0);
        material->SetUniformValue("u_BaseColorUVSet", u_base_color_uv_set);
        
        std::shared_ptr<UniformValue> u_base_color_uv_transform = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT3);
        Mat3 mat = Mat3::IDENTITY;
        u_base_color_uv_transform->SetData(mat);
        material->SetUniformValue("u_BaseColorUVTransform", u_base_color_uv_transform);
    }
    
    std::shared_ptr<UniformValue> u_metallic_factor_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
    u_metallic_factor_value->SetData((float) 0, 0);
    material->SetUniformValue("u_MetallicFactor", u_metallic_factor_value);
    
    std::shared_ptr<UniformValue> u_roughness_factor_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
    u_roughness_factor_value->SetData((float) 0, 0);
    material->SetUniformValue("u_RoughnessFactor", u_roughness_factor_value);
    
    std::shared_ptr<UniformValue> u_exposure_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC);
    u_exposure_value->SetData((float)0, 0);
    material->SetUniformValue("u_Exposure", u_exposure_value);
}

void MMaterialBase::UpdateUniforms(std::shared_ptr<MarsMaterial> material) {
    
}

MarsMaterialBlending MMaterialBase::GetBlendMode() {
    return blend_mode_;
}

MarsMaterialSide MMaterialBase::GetFaceSideMode() {
    return side_mode_;
}

MarsMaterialType MMaterialBase::GetMaterialType() {
    return type_;
}

}
