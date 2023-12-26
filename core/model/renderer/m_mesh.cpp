//
//  m_mesh.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "m_mesh.hpp"
#include "model/renderer/m_scene_manager.hpp"
#include "math/math_util.hpp"
#include <math.h>
#include "model/renderer/material/m_material_pbr.hpp"

namespace mn {

MPrimitive::MPrimitive(MarsRenderer* renderer) : renderer_(renderer), u_joint_matrix_data_(nullptr), u_joint_normal_matrix_data_(nullptr)  {
    
}

MPrimitive::~MPrimitive() {
    if (u_joint_matrix_data_) {
        free(u_joint_matrix_data_);
    }
    
    if (u_joint_normal_matrix_data_) {
        free(u_joint_normal_matrix_data_);
    }
}

void MPrimitive::Create(std::shared_ptr<MarsPrimitiveOptions> options, MMesh* parent_mesh) {
    this->skin_ = parent_mesh->Skin();
    this->geometry_ = std::make_shared<MGeometry>(options->geometry);
    
    auto material_options = options->material_options;
    if (material_options->type == MarsMaterialType::PBR) {
        std::shared_ptr<MarsMaterialPBROptions> pbr_options = std::static_pointer_cast<MarsMaterialPBROptions>(options->material_options);
        this->material_ = std::make_shared<MMaterialPBR>(pbr_options);
    } else {
        this->material_ = std::make_shared<MMaterialBase>(options->material_options);
    }
    
    this->name_ = parent_mesh->Name();
    this->name_.append("-prim");
    this->mri_priority_ = parent_mesh->Priority();
    
    if (skin_) {
        size_t joint_size = skin_->JointCount();
        size_t joint_matrix_byte_size =  16 * sizeof(float) * joint_size;
        u_joint_matrix_data_ = (uint8_t*) malloc(joint_matrix_byte_size);
        memset(u_joint_matrix_data_, 0, joint_matrix_byte_size);
        u_joint_normal_matrix_data_ = (uint8_t*) malloc(joint_matrix_byte_size);
        memset(u_joint_normal_matrix_data_, 0, joint_matrix_byte_size);
        
        if (skin_->IsTextureMode()) {
            MLOGD("MSkinAnimation Use Texture Mode");
            joint_matrix_texture_ = std::make_shared<MAnimTexture>(renderer_);
            joint_matrix_texture_->Create(joint_size, "joint_matrix");
            joint_normal_matrix_texture_ = std::make_shared<MAnimTexture>(renderer_);
            joint_normal_matrix_texture_->Create(joint_size, "joint_normal_matrix");
        }
    }
}

void MPrimitive::Build(size_t light_count, std::shared_ptr<MSkyBox> sky_box) {
    std::vector<std::string> feature_list;
    this->GetFeatureList(feature_list, light_count, sky_box);
    
    this->material_->Build(feature_list);
    
    // setup material uniformValue;
    if (mri_mesh_) {
        mri_mesh_.reset();
    }
    mri_mesh_ = std::make_shared<MarsMesh>(renderer_, name_);
    MaterialOptions mars_materail_option(this->material_->GetRenderState());
    mars_materail_option.shader.fragment = this->material_->FragmentShader();
    mars_materail_option.shader.vertex = this->material_->VertexShader();
        
    std::shared_ptr<MarsMaterial> mars_material = std::make_shared<MarsMaterial>(renderer_, mars_materail_option);
    std::shared_ptr<UniformValue> u_view_proj_matrix_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT4);
    mars_material->SetUniformValue("u_ViewProjectionMatrix", u_view_proj_matrix_value);
    
    std::shared_ptr<UniformValue> u_model_matrix = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT4);
    mars_material->SetUniformValue("u_ModelMatrix", u_model_matrix);

    std::shared_ptr<UniformValue> u_camera_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC3);
    mars_material->SetUniformValue("u_Camera", u_camera_value);
    
    std::shared_ptr<UniformValue> u_normal_matrix_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT4);
    mars_material->SetUniformValue("u_NormalMatrix", u_normal_matrix_value);
    this->material_->SetUpUniforms(mars_material);
    
    if (skin_) {
        if (skin_->IsTextureMode()) {
            std::shared_ptr<UniformValue> u_joint_matrix_texture = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
            u_joint_matrix_texture->SetTexture(joint_matrix_texture_->InnerTexture());
            mars_material->SetUniformValue("u_jointMatrixSampler", u_joint_matrix_texture);

            std::shared_ptr<UniformValue> u_joint_normal_matrix_texture = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
            u_joint_normal_matrix_texture->SetTexture(joint_normal_matrix_texture_->InnerTexture());
            mars_material->SetUniformValue("u_jointNormalMatrixSampler", u_joint_normal_matrix_texture);

        } else {
            size_t joint_size = this->skin_->JointCount();
            std::shared_ptr<UniformValue> u_joint_matrix_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT4, joint_size);
            mars_material->SetUniformValue("u_jointMatrix", u_joint_matrix_value);
            
            std::shared_ptr<UniformValue> u_joint_normal_matrix_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_MAT4, joint_size);
            mars_material->SetUniformValue("u_jointNormalMatrix", u_joint_normal_matrix_value);
        }
        
    }
    
    mri_mesh_->SetMaterial(mars_material);
    mri_mesh_->SetGeometry(this->geometry_->Geometry());
}

void MPrimitive::AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set) {
    if (mri_mesh_) {
        render_object_set.push_back(mri_mesh_);
    }
}

void MPrimitive::UpdateUniformsForScene(const Mat4& world_matrix, const Mat4& normal_matrix, const MSceneState& scene_state) {
    this->UpdateUniformsByAnimation(world_matrix, normal_matrix);
    this->InnerUpdateUniformsByScene(scene_state);
    this->material_->UpdateUniforms(mri_mesh_->GetMaterial());
}

void MPrimitive::InnerUpdateUniformsByScene(const MSceneState& scene_state) {
    const auto& material = mri_mesh_->GetMaterial();
    
    auto u_view_project_matrix_value = material->GetUniformValue("u_ViewProjectionMatrix");
    u_view_project_matrix_value->SetData(scene_state.view_projection_matrix);
    
    auto u_camera_value = material->GetUniformValue("u_Camera");
    if (u_camera_value) {
        u_camera_value->SetData(scene_state.camera_position, 0);
    }
    
    if (this->material_->GetMaterialType() != MarsMaterialType::UNLit) {
        const auto& lights = scene_state.lights;
        for (size_t i=0; i<lights.size(); i++) {
            auto& light = lights[i];
            float intensity_value = light->Visiable() ? light->Intensity() : 0;
            
            std::stringstream ss_tmp;
            ss_tmp << "u_Lights[" << i << "].";
            
            std::string direction = ss_tmp.str() + "direction";
            auto direction_u_value = material->CreateUniformValueIfNot(direction, UniformDataType::FLOAT_VEC3);
            direction_u_value->SetData(light->WorldDireciton(), 0);
            
            std::string range = ss_tmp.str() + "range";
            auto range_u_value = material->CreateUniformValueIfNot(range, UniformDataType::FLOAT_VEC);
            range_u_value->SetData(light->Range(), 0);
            
            std::string color = ss_tmp.str() + "color";
            auto color_u_value = material->CreateUniformValueIfNot(color, UniformDataType::FLOAT_VEC3);
            color_u_value->SetData(light->Color(), 0);
            
            std::string intensity = ss_tmp.str() + "intensity";
            auto intensity_u_value = material->CreateUniformValueIfNot(intensity, UniformDataType::FLOAT_VEC);
            intensity_u_value->SetData(intensity_value, 0);
            
            std::string position = ss_tmp.str() + "position";
            auto position_u_value = material->CreateUniformValueIfNot(position, UniformDataType::FLOAT_VEC3);
            position_u_value->SetData(light->GetWorldPosition(), 0);
            
            std::string inner_cone = ss_tmp.str() + "innerConeCos";
            auto inner_cone_u_value = material->CreateUniformValueIfNot(inner_cone, UniformDataType::FLOAT_VEC);
            inner_cone_u_value->SetData(std::cos(light->InnerConeAngle()), 0);
            
            std::string outer_cone = ss_tmp.str() + "outerConeCos";
            auto outer_cone_u_value = material->CreateUniformValueIfNot(outer_cone, UniformDataType::FLOAT_VEC);
            outer_cone_u_value->SetData(std::cos(light->OuterConeAngle()), 0);
            
            std::string type = ss_tmp.str() + "type";
            auto type_u_value = material->CreateUniformValueIfNot(type, UniformDataType::INT);
            type_u_value->SetData(light->GetLightType());
            
            std::string padding = ss_tmp.str() + "padding";
            auto padding_u_value = material->CreateUniformValueIfNot(padding, UniformDataType::FLOAT_VEC2);
            padding_u_value->SetData(light->Padding(), 0);
        }
        
        // todo: 放到初始化里面做;
        auto sky_box = scene_state.sky_box;
        if (sky_box && sky_box->IsAvailable()) {
            auto u_ibl_intensity_value = material->CreateUniformValueIfNot("u_IblIntensity", UniformDataType::FLOAT_VEC2);
            float intensity[2] = { sky_box->GetIntensity(), sky_box->GetReflectionIntensity()};
            u_ibl_intensity_value->SetData(intensity, 0, 2);
        
            auto u_brdf_lut_value = material->CreateUniformValueIfNot("u_brdfLUT", UniformDataType::SAMPLER_2D);
            u_brdf_lut_value->SetTexture(sky_box->GetBrdfLutTexture());
            
            if (sky_box->GetDiffuseImageTexture()) {
                auto u_diffuse_env_sampler_value = material->CreateUniformValueIfNot("u_DiffuseEnvSampler", UniformDataType::SAMPLER_CUBE);
                u_diffuse_env_sampler_value->SetTexture(sky_box->GetDiffuseImageTexture());
            } else {
                std::vector<std::string> coeffs_alias = { "l00", "l1m1", "l10", "l11", "l2m2", "l2m1", "l20", "l21", "l22" };
                auto irradiance_coeffs = sky_box->GetIrradianceCoeffs();
                if (irradiance_coeffs.size() == coeffs_alias.size()) {
                    for (size_t i=0; i<coeffs_alias.size(); i++) {
                        std::string coeff_perfix = "u_shCoefficients.";
                        coeff_perfix.append(coeffs_alias[i]);
                        auto u_sh_coefficients_value = material->CreateUniformValueIfNot(coeff_perfix, UniformDataType::FLOAT_VEC3);
                        u_sh_coefficients_value->SetData(irradiance_coeffs[i], 0);
                    }
                } else {
                    DEBUG_MLOGD("sky box irradiance coeffs size not match");
                    assert(0);
                }
            }
            
            auto u_mip_count_value = material->CreateUniformValueIfNot("u_MipCount", UniformDataType::FLOAT_VEC);
            u_mip_count_value->SetData(sky_box->GetMipCount(), 0);
            
            auto u_specular_env_sampler = material->CreateUniformValueIfNot("u_SpecularEnvSampler", UniformDataType::SAMPLER_CUBE);
            u_specular_env_sampler->SetTexture(sky_box->GetSpecularEnvSamplerTexutre());
        }
    }
}

void MPrimitive::UpdateUniformsByAnimation(const Mat4& world_matrix, const Mat4& normal_matrix) {
    const auto& material = mri_mesh_->GetMaterial();
    auto model_uniform_value = material->GetUniformValue("u_ModelMatrix");
    model_uniform_value->SetData(world_matrix);
    
    auto normal_uniform_value = material->GetUniformValue("u_NormalMatrix");
    if (normal_uniform_value) {
        normal_uniform_value->SetData(normal_matrix);
    }
    
    if (skin_) {
        size_t joint_size = this->skin_->JointCount();
        size_t joint_matrix_byte_size =  16 * sizeof(float) * joint_size;
        
        Mat4 inverse_world_matrix = world_matrix.Clone();
        inverse_world_matrix.Inverse();
        
        auto& animation_matrices = this->skin_->AnimationMatrices();
        for (size_t i=0; i<animation_matrices.size(); i++) {
            Mat4 temp_mat;
            MathUtil::Mat4Multiply(temp_mat, inverse_world_matrix, animation_matrices[i]);
            MathUtil::CopyValueFromeMat4(temp_mat, u_joint_matrix_data_ + (sizeof(float) * 16 * i));

            Mat4 normal_matrix = temp_mat.Clone();
            normal_matrix.Inverse();
            normal_matrix.Transpose();
            MathUtil::CopyValueFromeMat4(normal_matrix, u_joint_normal_matrix_data_ + (sizeof(float) * 16 * i));
        }
        
        if (skin_->IsTextureMode()) {
            joint_matrix_texture_->Update(u_joint_matrix_data_);
            joint_normal_matrix_texture_->Update(u_joint_normal_matrix_data_);
            
            auto u_joint_matrix_texture = material->GetUniformValue("u_jointMatrixSampler");
            u_joint_matrix_texture->SetTexture(joint_matrix_texture_->InnerTexture());
            
            auto u_joint_normal_matrix_texture = material->GetUniformValue("u_jointNormalMatrixSampler");
            u_joint_normal_matrix_texture->SetTexture(joint_normal_matrix_texture_->InnerTexture());
            
            
        } else {
            auto u_joint_matrix_value = material->GetUniformValue("u_jointMatrix");
            u_joint_matrix_value->SetData(u_joint_matrix_data_, joint_matrix_byte_size);
            auto u_joint_normal_matrix_value = material->GetUniformValue("u_jointNormalMatrix");
            u_joint_normal_matrix_value->SetData(u_joint_normal_matrix_data_, joint_matrix_byte_size);
        }
    }
}

void MPrimitive::GetFeatureList(std::vector<std::string>& feature_list ,size_t light_count, std::shared_ptr<MSkyBox> sky_box) {
    if (geometry_->HasNormals()) {
        feature_list.push_back("HAS_NORMALS 1");
    }
    
    if (geometry_->HasTangents()) {
        feature_list.push_back("HAS_TANGENTS 1");
    }
    
    if (geometry_->HasUVCoord(1)) {
        feature_list.push_back("HAS_UV_SET1 1");
    }
    
    if (geometry_->HasUVCoord(2)) {
        feature_list.push_back("HAS_UV_SET2 1");
    }
    
    if (skin_) {
        feature_list.push_back("USE_SKINNING 1");
        std::string joint_count_define("JOINT_COUNT ");
        size_t joint_count = this->skin_->JointCount();
        joint_count_define.append(std::to_string(joint_count));
        feature_list.push_back(joint_count_define);
        feature_list.push_back("HAS_JOINT_SET1 1");
        feature_list.push_back("HAS_WEIGHT_SET1 1");
        
        if (skin_->IsTextureMode()) {
            feature_list.push_back("USE_SKINNING_TEXTURE 1");
        }
    }
    
    if (material_->GetMaterialType() != MarsMaterialType::UNLit) {
        if (light_count > 0 && this->geometry_->HasNormals()) {
            feature_list.push_back("USE_PUNCTUAL 1");
            std::string light_count_define("LIGHT_COUNT ");
            light_count_define.append(std::to_string(light_count));
            feature_list.push_back(light_count_define);
        }
        
        if (sky_box && sky_box->IsAvailable()) {
            feature_list.push_back("USE_IBL 1");
            
            // todo: 10.3.80
//            feature_list.push_back("USE_TEX_LOD 1");
            if (sky_box->GetDiffuseImageTexture()) {
                // do nothing;
            } else {
                feature_list.push_back("IRRADIANCE_COEFFICIENTS 1");
            }
        }
    }
    
    // todo: shadow;
}

MMesh::MMesh(MarsRenderer* renderer) : renderer_(renderer) {
    
}

MMesh::~MMesh() {
    
}

void MMesh::Create(std::shared_ptr<MarsItemMesh> mesh_item_options, std::shared_ptr<ModelVFXItem> vfx_item, std::shared_ptr<TreeVFXItem> parent_item) {
    auto& mesh_options = mesh_item_options->options;
//    parent_item_ = parent_item;
    owner_item_ = vfx_item;
    name_ = mesh_item_options->name;
    
    if (mesh_options->skin) {
        skin_ = std::make_shared<MSkin>(renderer_);
        skin_->Create(mesh_options->skin, parent_item);
    }
 
    auto primitive_options = mesh_options->primitives;
    for (size_t i=0; i<primitive_options.size(); i++) {
        auto m_primitive = std::make_shared<MPrimitive>(renderer_);
        m_primitive->Create(primitive_options[i], this);
        this->primitivies_.push_back(m_primitive);
    }
}

void MMesh::Build(size_t light_count, std::shared_ptr<MSkyBox> sky_box) {
    for (size_t i=0; i<primitivies_.size(); i++) {
        primitivies_[i]->Build(light_count, sky_box);
    }
}

void MMesh::Tick(float delta_seconds) {
    if (std::shared_ptr<ModelVFXItem> owner_item = owner_item_.lock()) {
        Mat4 world_matrix;
        owner_item->transform_->GetWorldMatrix(world_matrix);
        transform_.SetMatrix(world_matrix);
    }
    
    if (skin_) {
        skin_->UpdateSkinMatrices();
    }
}

void MMesh::UpdateParentItem(std::shared_ptr<TreeVFXItem> parent_item) {
//    parent_item_ = parent_item;
    if (skin_) {
        skin_->UpdateParentItem(parent_item);
    }
}

void MMesh::AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set) {
    for(size_t i=0; i<primitivies_.size(); i++) {
        primitivies_[i]->AddToRenderObjectSet(render_object_set);
    }
}

// todo: UniformsByAnimation; ByScene;
void MMesh::UpdateUniformsForScene(const MSceneState& scene_state) {
    Mat4 world_mat = this->WorldMatrix();
    Mat4 normal_mat = world_mat.Clone();
    normal_mat.Inverse();
    normal_mat.Transpose();
    
    for (size_t i=0; i<primitivies_.size(); i++) {
        primitivies_[i]->UpdateUniformsForScene(world_mat, normal_mat, scene_state);
    }
}

const Mat4& MMesh::WorldMatrix() {
    return this->transform_.Matrix();
}

}
