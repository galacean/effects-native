//
//  bloom_filter_define.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "bloom_filter_define.hpp"
#include "player/composition.h"
#include "util/vfx_value_getter.h"
#include "shader/adjust/bloom_m_frag.h"
#include "shader/adjust/bloom_t_frag.h"
#include "filters/gaussian_filter_define.hpp"

#include "filters/filter_utils.hpp"

namespace mn {

BloomFilterDefine::BloomFilterDefine(Composition* composition, BloomFilterData* filter_data) : FilterDefine(composition, filter_data) {
    
    color_weights_.Set(filter_data->color_weight[0], filter_data->color_weight[1], filter_data->color_weight[2]);
    
    bloom_add_on_ = VFXValueGetter::CreateValueGetter(filter_data->bloom_add_on);
    color_add_on_ = VFXValueGetter::CreateValueGetter(filter_data->color_add_on);
    
    GaussainParams gaussain_params;
    int radius = filter_data->radius->val;
    GaussianFilterDefine::GenerateGaussianFragment(radius, gaussain_params);
    
    gaussian_fragment_ = gaussain_params.fragment;
    int width = composition->renderer_->GetSurfaceWidth() / gaussain_params.down_sample;
    int height = composition->renderer_->GetSurfaceHeight() / gaussain_params.down_sample;
    
    viewport_[2] = width;
    viewport_[3] = height;
    
    TextureOption blur_target_option(width, height, TextureSourceType::FRAMEBUFFER);
    blur_target_ = std::make_shared<MarsTexture>(composition->renderer_, blur_target_option);
    
    TextureOption blur_inner_media_option(width, height, TextureSourceType::FRAMEBUFFER);
    blur_inner_media_ = std::make_shared<MarsTexture>(composition->renderer_, TextureSourceType::FRAMEBUFFER);
    
    mesh_filter_option_.fragment = BLOOM_M_FRAG;
    mesh_filter_option_.shader_cache_id = "bloom_" + std::to_string(gaussain_params.step);
    
    // setup bloom uniforms;
    std::shared_ptr<UniformValue> u_bloom_params_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    mesh_filter_option_.uniform_values.insert({"uBloomParams", u_bloom_params_value});
    mesh_filter_option_.uniform_variables.push_back("uBloomParams");
    
    std::shared_ptr<UniformValue> u_blur_source_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D, true, GAUSSIAN_SOURCE);
    mesh_filter_option_.uniform_values.insert({"uBlurSource", u_blur_source_value});

    std::shared_ptr<UniformValue> u_tex_step_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC2, true, GAUSSIAN_SEMANTIC_DIRECTION);
    mesh_filter_option_.uniform_values.insert({"uTexStep", u_tex_step_value});
    
    std::shared_ptr<UniformValue> u_tex_size_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC2, true, SEMANTIC_PRE_COLOR_ATTACHMENT_SIZE_0);
    mesh_filter_option_.uniform_values.insert({"uTexSize", u_tex_size_value});
    
    std::shared_ptr<UniformValue> u_bloom_blur_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
    u_bloom_blur_value->SetTexture(blur_target_);
    mesh_filter_option_.uniform_values.insert({"uBloomBlur", u_bloom_blur_value});
    
    std::shared_ptr<UniformValue> u_color_threshold_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4);
    float threshold_value[4] = { (color_weights_.m[0] / 255.0f), (color_weights_.m[1] / 255.0f), (color_weights_.m[2] / 255.0f), 0 };
    
    u_color_threshold_value->SetData(threshold_value, 0, 4);
    mesh_filter_option_.uniform_values.insert({"uColorThreshold", u_color_threshold_value});
    
    mesh_filter_option_.material_state.blending = std::make_shared<MBool>(false);
}

BloomFilterDefine::~BloomFilterDefine() {
    DEBUG_MLOGD("BloomFilterDefine Destructor");
    
    if (bloom_add_on_) {
        delete bloom_add_on_;
    }
    
    if (color_add_on_) {
        delete color_add_on_;
    }
}

std::vector<std::shared_ptr<RenderPass>> BloomFilterDefine::GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    std::vector<std::shared_ptr<RenderPass>> pre_render_pass;
    
    class BloomSemanticValue : public SemanticValue {
    public:
        BloomSemanticValue(int type, Composition* composition, BloomFilterDefine* filter_define) : type_(type), composition_(composition), filter_define_(filter_define) {
            
        }
        
        void GetSemantiveValue(const RenderState& state, std::shared_ptr<UniformValue> value) override {
            DEBUG_MLOGD("GetBloomSemanticValue type is : %d", type_);
            switch (type_) {
                case 1: {
                    
                    float h_direction[2] = { 1.0, 0.0 };
                    value->SetData(h_direction, 0, 2);
                    break;
                };
                case 2: {
                    value->SetTexture(filter_define_->blur_target_);
                    break;
                };
                case 3: {
                    auto blur_target = filter_define_->blur_target_;
                    float width = (float) blur_target->Width();
                    float height = (float) blur_target->Height();
                    float size[2] = { width, height };
                    value->SetData(size, 0, 2);
                    break;
                };
                case 4: {
                    float v_direction[2] = { 0.0, 1.0 };
                    value->SetData(v_direction, 0, 2);
                    break;
                };
                case 5: {
                    value->SetTexture(filter_define_->blur_inner_media_);
                    break;
                };
                case 6: {
                    auto blur_inner_media = filter_define_->blur_inner_media_;
                    float width = (float) blur_inner_media->Width();
                    float height = (float) blur_inner_media->Height();
                    float size[2] = { width, height };
                    value->SetData(size, 0, 2);
                    break;
                };
                default:
                    break;
            }
        }
        
    private:
        int type_;
        Composition* composition_;
        BloomFilterDefine* filter_define_;
    };
    
//    // threshold mesh
    auto pre_threshold_meshes = this->GenerateThresholdMesh(meshes);
    auto threshold_pass = std::make_shared<RenderPass>(composition_->renderer_, viewport_, "threshold");
    RenderPassAttachmentOptions attachment_threshold;
    attachment_threshold.AddRenderPassColorAttachmentOption(blur_target_);
    threshold_pass->SetRenderPassAttachent(attachment_threshold, CLEAR_ACTION_TYPE::CLEAR_COLOR);
    threshold_pass->SetMeshes(pre_threshold_meshes);
    pre_render_pass.push_back(threshold_pass);
    
    // gaussain mesh;
    auto pre_gaussain_meshes = this->GenerateGaussianMesh(meshes);
    
    // gaussain_h pass
    auto gaussain_h_pass = std::make_shared<RenderPass>(composition_->renderer_, viewport_, "gaussianH");
    RenderPassAttachmentOptions attachment_options_h;
    attachment_options_h.AddRenderPassColorAttachmentOption(blur_inner_media_);
    gaussain_h_pass->SetRenderPassAttachent(attachment_options_h, CLEAR_ACTION_TYPE::CLEAR_COLOR);
    
    gaussain_h_pass->SetSemanticValues(GAUSSIAN_SEMANTIC_DIRECTION, std::make_shared<BloomSemanticValue>(1, composition_, this));
    gaussain_h_pass->SetSemanticValues(GAUSSIAN_SOURCE, std::make_shared<BloomSemanticValue>(2, composition_, this));
    gaussain_h_pass->SetSemanticValues(SEMANTIC_PRE_COLOR_ATTACHMENT_SIZE_0, std::make_shared<BloomSemanticValue>(3, composition_, this));
    gaussain_h_pass->SetMeshes(pre_gaussain_meshes);
    pre_render_pass.push_back(gaussain_h_pass);
    
    // gaussain_v pass
    auto gaussain_v_pass = std::make_shared<RenderPass>(composition_->renderer_, viewport_, "gaussianV");
    RenderPassAttachmentOptions attachment_options_v;
    attachment_options_v.AddRenderPassColorAttachmentOption(blur_target_);
    gaussain_v_pass->SetRenderPassAttachent(attachment_options_v, CLEAR_ACTION_TYPE::CLEAR_COLOR);
    
    gaussain_v_pass->SetSemanticValues(GAUSSIAN_SEMANTIC_DIRECTION, std::make_shared<BloomSemanticValue>(4, composition_, this));
    gaussain_v_pass->SetSemanticValues(GAUSSIAN_SOURCE, std::make_shared<BloomSemanticValue>(5, composition_, this));
    gaussain_v_pass->SetSemanticValues(SEMANTIC_PRE_COLOR_ATTACHMENT_SIZE_0, std::make_shared<BloomSemanticValue>(6, composition_, this));
    gaussain_v_pass->SetMeshes(pre_gaussain_meshes);
    pre_render_pass.push_back(gaussain_v_pass);

    return pre_render_pass;
}

void BloomFilterDefine::GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) {
    if (key == "uBloomParams") {
        float bloom_add_on = bloom_add_on_->GetValue(life);
        float color_add_on = color_add_on_->GetValue(life);
        
        float bloom_params[4] = { bloom_add_on, color_add_on, 0, 1 };
        value->SetData(bloom_params, 0, 4);
    }
}

void BloomFilterDefine::OnItemRemove() {
    
}

std::vector<std::shared_ptr<MarsMesh>> BloomFilterDefine::GenerateThresholdMesh(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    std::vector<std::shared_ptr<MarsMesh>> pre_meshes;
    if (meshes.size() >= 2) {
        mesh_filter_option_.fragment = BLOOM_T_FRAG;
        auto clone_mesh = FilterUtils::CloneSpriteMesh(meshes[1], composition_, this);
        pre_meshes.push_back(clone_mesh);
    } else {
        MLOGE("Generate pass threshold meshes failed");
    }
    
    return pre_meshes;
}

std::vector<std::shared_ptr<MarsMesh>> BloomFilterDefine::GenerateGaussianMesh(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    std::vector<std::shared_ptr<MarsMesh>> pre_meshes;

    if (meshes.size() >= 2) {
        // todo: hack, 先创建正常的copy fragment Mesh； 在生成RenderPass的时候，再创建gaussian fragment mesh；
        mesh_filter_option_.fragment = gaussian_fragment_ ;
        auto clone_mesh = FilterUtils::CloneSpriteMesh(meshes[1], composition_, this);
        pre_meshes.push_back(clone_mesh);
    } else {
        MLOGE("Generate pass gaussain meshes failed");
    }
    
    return pre_meshes;

}

}
