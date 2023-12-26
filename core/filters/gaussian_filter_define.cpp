//
//  gaussian_filter_define.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/7/27.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include <cmath>
#include <sstream>
#include <iomanip>
#include "gaussian_filter_define.hpp"
#include "shader/adjust/copy_frag.h"
#include "filters/filter_utils.hpp"
#include "player/composition.h"

namespace mn {

void GaussianFilterDefine::GenerateGaussianFragment(int raw_radius, GaussainParams& gaussian_params) {
    int down_sample = raw_radius <= 3 ? 1 : 2;
    int radius = raw_radius / down_sample;
    
    int max_step = 4;
    while (radius > 10 && down_sample < max_step) {
        down_sample *= 2;
        radius = raw_radius / down_sample;
    }
    
    gaussian_params.step = 1 + (raw_radius % down_sample) / down_sample / down_sample;
    radius = std::floor(radius);
    std::string steps = "";
    std::vector<float> floats = GetGaussianParams(radius);
    for (int i=-radius; i<=radius; i++) {
        std::stringstream ss;
        float index = i;
        float weight = floats[i + radius];
        ss << "color += texture2D(uBlurSource, getTexCoord(" << std::fixed << std::setprecision(1) << index << ")) * ";
        ss << std::fixed << std::setprecision(8) << weight << ";";
        steps += ss.str();
    }
    
    std::string gaussian_fragment = R"(
        uniform sampler2D uBlurSource;
        uniform vec2 uTexSize;
        uniform vec2 uTexStep;
        #define getTexCoord(i) coord + uTexStep/uTexSize * i
        vec4 filterMain(vec2 coord,sampler2D tex){
            vec4 color = vec4(0.);
            vec2 texCoord;
            $steps
            return color;
      }
    )";
    
    gaussian_fragment.replace(gaussian_fragment.find("$steps"), 6, steps);
    gaussian_params.fragment = gaussian_fragment;
    gaussian_params.radius = radius;
    gaussian_params.down_sample = down_sample;
    
    MLOGD("GenerateGaussianParames radius %d, step %d, down_sample %d", gaussian_params.radius, gaussian_params.step, gaussian_params.down_sample);
}

GaussianFilterDefine::GaussianFilterDefine(Composition* composition, GaussianFilterData* filter_data) : FilterDefine(composition, filter_data) {
    
    GaussainParams gaussian_params;
    int radius = filter_data->radius->val;
    GenerateGaussianFragment(radius, gaussian_params);
    step_ = gaussian_params.step;
    gaussian_fragment_ = gaussian_params.fragment;
    
    int tex_width = (int) composition->renderer_->GetSurfaceWidth() / gaussian_params.down_sample;
    int tex_height = (int) composition->renderer_->GetSurfaceHeight() / gaussian_params.down_sample;
    
    viewport_[2] = tex_width;
    viewport_[3] = tex_height;
    
    TextureOption texture_option_v(tex_width, tex_height, TextureSourceType::FRAMEBUFFER);
    gaussian_texture_v_ = std::make_shared<MarsTexture>(composition->renderer_, texture_option_v);
    
    TextureOption texture_option_h(tex_width, tex_height, TextureSourceType::FRAMEBUFFER);
    gaussian_textuer_h_ = std::make_shared<MarsTexture>(composition->renderer_, texture_option_h);
    
    // Init MeshOptions
    mesh_filter_option_.fragment = COPY_FRAG;
    mesh_filter_option_.shader_cache_id = "gaussian:" + std::to_string(step_);
    
    auto frame_source_uniform_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
    frame_source_uniform_value->SetTexture(gaussian_texture_v_);
    mesh_filter_option_.uniform_values["uFrameSource"] = frame_source_uniform_value;
    
    auto tex_size_uniform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC2, 2);
    float tex_size[2] = { (float)tex_width, (float)tex_height };
    tex_size_uniform_value->SetData(tex_size, 0, 2);
    mesh_filter_option_.uniform_values["uTexSize"] = tex_size_uniform_value;
    
    auto blur_source_uniform_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D, true, GAUSSIAN_SOURCE);
    mesh_filter_option_.uniform_values["uBlurSource"] = blur_source_uniform_value;
    
    auto tex_step_uniform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC2, true, GAUSSIAN_SEMANTIC_DIRECTION);
    mesh_filter_option_.uniform_values["uTexStep"] = tex_step_uniform_value;
    
    auto filter_source_uniform_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D, true, SEMANTIC_PRE_COLOR_ATTACHMENT_0);
    mesh_filter_option_.uniform_values["uFilterSource"] = filter_source_uniform_value;
    
    auto filter_source_size_uniform_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC2, true, SEMANTIC_PRE_COLOR_ATTACHMENT_SIZE_0);
    mesh_filter_option_.uniform_values["uFilterSourceSize"] = filter_source_size_uniform_value;
    
    mesh_filter_option_.material_state.blending = std::make_shared<MBool>(false);
    mesh_filter_option_.material_state.cull_face_enabled = std::make_shared<MBool>(false);
}

GaussianFilterDefine::~GaussianFilterDefine() {
    DEBUG_MLOGD("GaussianFilterDefine Destructor");
}

std::vector<std::shared_ptr<RenderPass>> GaussianFilterDefine::GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    
    class GaussainSemanticValue : public SemanticValue {
    public:
        GaussainSemanticValue(int type, Composition* composition, GaussianFilterDefine* filter_define) : type_(type), composition_(composition), filter_define_(filter_define) {
        }
        
        void GetSemantiveValue(const RenderState& state, std::shared_ptr<UniformValue> value) override {
            DEBUG_MLOGD("GetGaussainSemanticValue type is : %d", type_);
            switch (type_) {
                case 1: {
                    // H_GAUSSIAN_SEMANTIC_DIRECTION;
                    float step = filter_define_->Step();
                    float h_direction[2] = { 0, step };
                    value->SetData(h_direction, 0, 2);
                    break;
                }
                case 2: {
                    // H_GAUSSIAN_SOURCE;
                    // todo: ？？？ bug？
                    std::shared_ptr<RenderPass> pre_pass = composition_->render_frame_->FindPreviousRenderPass(state.current_render_pass);
                    std::shared_ptr<MarsTexture> color_texture = pre_pass->attachments_[0]->GetMarsTexture();
                    value->SetTexture(color_texture);
                    break;
                }
                case 3: {
                    // V_GAUSSIAN_SEMANTIC_DIRECTION;
                    float step = filter_define_->Step();
                    float v_direction[2] = { step, 0 };
                    value->SetData(v_direction, 0, 2);
                    break;
                }
                case 4:
                    // V_GAUSSIAN_SOURCE;
                    value->SetTexture(filter_define_->GetGaussianHTexture());
                    break;
                default:
                    break;
            }
        }
        
    private:
        int type_;
        GaussianFilterDefine* filter_define_;
        Composition* composition_;
        int step_;
    };
    
    
    std::vector<std::shared_ptr<RenderPass>> gaussian_render_passes;
    auto pre_pass_meshes = this->GeneratePassMeshes(meshes);
    // gaussain_h pass
    auto gaussain_h_pass = std::make_shared<RenderPass>(composition_->renderer_, viewport_, "gaussianH");
    RenderPassAttachmentOptions attachment_options_h;
    attachment_options_h.AddRenderPassColorAttachmentOption(gaussian_textuer_h_);
    gaussain_h_pass->SetRenderPassAttachent(attachment_options_h, CLEAR_ACTION_TYPE::CLEAR_COLOR);
    gaussain_h_pass->SetSemanticValues(GAUSSIAN_SEMANTIC_DIRECTION, std::make_shared<GaussainSemanticValue>(1, composition_, this));
    gaussain_h_pass->SetSemanticValues(GAUSSIAN_SOURCE, std::make_shared<GaussainSemanticValue>(2, composition_, this));
    gaussain_h_pass->SetMeshes(pre_pass_meshes);
    gaussian_render_passes.push_back(gaussain_h_pass);
    
    auto gaussain_v_pass = std::make_shared<RenderPass>(composition_->renderer_, viewport_, "gaussianV");
    RenderPassAttachmentOptions attachment_options_v;
    attachment_options_v.AddRenderPassColorAttachmentOption(gaussian_texture_v_);
    gaussain_v_pass->SetRenderPassAttachent(attachment_options_v, CLEAR_ACTION_TYPE::CLEAR_COLOR);
    gaussain_v_pass->SetSemanticValues(GAUSSIAN_SEMANTIC_DIRECTION, std::make_shared<GaussainSemanticValue>(3, composition_, this));
    gaussain_v_pass->SetSemanticValues(GAUSSIAN_SOURCE, std::make_shared<GaussainSemanticValue>(4, composition_, this));
    gaussain_v_pass->SetMeshes(pre_pass_meshes);
    gaussian_render_passes.push_back(gaussain_v_pass);
    
    return gaussian_render_passes;
}

std::vector<float> GaussianFilterDefine::GetGaussianParams(int radius) {
    float sigma = (radius + 1) / 3.329f;
    std::vector<float> nums;
    for (int i=-radius ; i<=radius; i++) {
        nums.push_back(GaussianNumber(i, sigma));
    }
    return nums;
}

float GaussianFilterDefine::GaussianNumber(int x, float sig) {
    float num = std::exp(-(x * x) / (2 * sig * sig));
    float sqrt = std::sqrt(2 * M_PI) * sig;
    return num / sqrt;
}

std::vector<std::shared_ptr<MarsMesh>> GaussianFilterDefine::GeneratePassMeshes(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    std::vector<std::shared_ptr<MarsMesh>> pre_meshes;
    if (meshes.size() >= 2) {
        pre_meshes.push_back(meshes[0]);
        
        // todo: hack, 先创建正常的copy fragment Mesh； 在生成RenderPass的时候，再创建gaussian fragment mesh；
        mesh_filter_option_.fragment = gaussian_fragment_;
        auto clone_mesh = FilterUtils::CloneSpriteMesh(meshes[1], composition_, this);
        pre_meshes.push_back(clone_mesh);
    } else {
        MLOGE("Generate pass meshes failed");
    }
    
    return pre_meshes;
}

}
