//
//  m_skybox.cpp
//
//  Created by Zongming Liu on 2022/10/28.
//

#include "m_skybox.hpp"
#include <vector>
#include "util/util.hpp"
#include "util/image_loader.h"
#include "model/utility/skybox_utils.hpp"
#include "model/renderer/m_scene_manager.hpp"

namespace mn {

// todo: 效率低，大；优化；

const static std::string BRDF_LUT = "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";

std::shared_ptr<MarsTexture> MSkyBox::CreateBrdfLutTexture(MarsRenderer* renderer) {
    size_t brdf_lut_data_length;
    unsigned char* brdf_lut_data = Utils::Base64Decode(BRDF_LUT.data(), BRDF_LUT.size(), &brdf_lut_data_length);
    
    ImageRawData brdf_lut_raw((uint8_t*) brdf_lut_data, brdf_lut_data_length);
    brdf_lut_raw.need_free = true;
    
    auto img = ImageLoader::ParseImageData(&brdf_lut_raw);
    if (!img) {
        MLOGE("Load BRDFLut Image Failed");
        return nullptr;
    }
    
    TextureOption brdf_lut_texture_option(img->width, img->height, TextureSourceType::DATA);
    brdf_lut_texture_option.SetData(img->data);
    std::shared_ptr<MarsTexture> brdf_lut_texture = std::make_shared<MarsTexture>(renderer, brdf_lut_texture_option);
    
    delete img;
    
    return brdf_lut_texture;
}

MSkyBox::MSkyBox(MarsRenderer* renderer):renderer_(renderer) {
    
}

MSkyBox::~MSkyBox() {
    
}

void MSkyBox::Build() {
    skybox_material_ = std::make_shared<MMaterialSkybox>();
    skybox_material_->Create(this);
    
    std::vector<std::string> feature_list;
    this->GetShaderFeatures(feature_list);
    skybox_material_->Build(feature_list);
    
    skybox_mesh_ = SkyboxUtils::CreateSkyboxMesh("skybox", renderer_, skybox_material_);
    skybox_material_->SetUpUniforms(skybox_mesh_->GetMaterial());
}

void MSkyBox::Create(std::shared_ptr<MarsItemSkybox> sky_box, std::shared_ptr<MarsTexture> brdf_lut, std::shared_ptr<ModelVFXItem> owner_item) {
    auto options = sky_box->options;
    intensity_ = options->intensity;
    reflection_intensity_ = options->reflection_intensity;
    brdf_lut_ = brdf_lut;
    
    TransformIrradianceCoeffs(options->irradiance_coeffs);
    diffuse_image_ = options->diffuse_image;
    specular_image_ = options->specular_image;
    
    specular_image_size_ = options->specular_image_size;
    specular_mip_count_ = options->specular_mip_count;
}

bool MSkyBox::IsAvailable() {
    return true;
}

std::shared_ptr<MarsTexture> MSkyBox::GetBrdfLutTexture() {
    return brdf_lut_;
}

std::shared_ptr<MarsTexture> MSkyBox::GetDiffuseImageTexture() {
    return diffuse_image_;
}

std::shared_ptr<MarsTexture> MSkyBox::GetSpecularEnvSamplerTexutre() {
    return specular_image_;
}

void MSkyBox::AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set) {
    if (visiable_ && renderable_ && skybox_mesh_) {
        render_object_set.push_back(skybox_mesh_);
    }
}

void MSkyBox::UpdateUniformsForScene(const MSceneState& scene_state) {
    if (visiable_ && renderable_ && skybox_mesh_ && skybox_material_) {
        
        const auto& camera = scene_state.camera;
        const Mat4& view_matrix = scene_state.view_matrix;
        
        Mat4 inv_view_project_mat;
        camera->GetNewProjectionMatrix(inv_view_project_mat);
        MathUtil::Mat4Multiply(inv_view_project_mat, inv_view_project_mat, view_matrix);
        inv_view_project_mat.Inverse();
        
        auto material = skybox_mesh_->GetMaterial();
        auto inv_view_project_matrix_u_value = material->GetUniformValue("u_InvViewProjectionMatrix");
        inv_view_project_matrix_u_value->SetData(inv_view_project_mat);
    }
}

int MSkyBox::GetMipCount() {
    return specular_mip_count_;
}

float MSkyBox::GetIntensity() {
    return intensity_;
}

float MSkyBox::GetReflectionIntensity() {
    return reflection_intensity_;
}
    
void MSkyBox::TransformIrradianceCoeffs(const std::vector<std::vector<float>>& irradiance_coffes) {
    for (size_t i=0; i<irradiance_coffes.size(); i++) {
        const auto& irradiance_coffe = irradiance_coffes[i];
        if (irradiance_coffe.size() != 3) {
            assert(0);
        }
        
        Vec3 irradiance;
        for (size_t i=0; i<irradiance_coffe.size(); i++) {
            irradiance.m[i] = irradiance_coffe[i];
        }
        
        irradiance_coeffs_.push_back(irradiance);
    }
}

void MSkyBox::GetShaderFeatures(std::vector<std::string>& feature_list) {
    feature_list.push_back("USE_IBL 1");
//    feature_list.push_back("USE_TEX_LOD 1");
    
//    if (!diffuse_image_texture_) {
//        feature_list.push_back("IRRADIANCE_COEFFICIENTS 1");
//    }
}

}
