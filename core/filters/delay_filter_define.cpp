//
//  delay_filter_define.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "delay_filter_define.hpp"
#include "shader/adjust/delay_frag.h"
#include "player/composition.h"
#include "ri/render/render_frame_internal.hpp"
#include "ri/render/render_pass.hpp"

namespace mn {

DelayFilterDefine::DelayFilterDefine(Composition* composition, DelayFilterData* filter_data) : FilterDefine(composition, filter_data) {
    
    TextureOption tex_option(TextureSourceType::FRAMEBUFFER);
    last_source_texture_ = std::make_shared<MarsTexture>(composition->renderer_, tex_option);
    
    mesh_filter_option_.uniform_variables.push_back("uParams");
    
    mesh_filter_option_.fragment = DELAY_FRAG;
    mesh_filter_option_.shader_cache_id = "delay";
    
    mesh_filter_option_.material_state.blending = std::make_shared<MBool>(true);
    mesh_filter_option_.material_state.blend_src =std::make_shared<MUint>(GL_SRC_ALPHA);
    mesh_filter_option_.material_state.blend_src_alpha = std::make_shared<MUint>(GL_SRC_ALPHA);
    mesh_filter_option_.material_state.blend_dst = std::make_shared<MUint>(GL_ONE_MINUS_SRC_ALPHA);
    mesh_filter_option_.material_state.blend_dst_alpha = std::make_shared<MUint>(GL_ONE_MINUS_SRC_ALPHA);
    mesh_filter_option_.material_state.depth_test = std::make_shared<MBool>(false);
    
    std::shared_ptr<UniformValue> u_last_source_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
    u_last_source_value->SetTexture(this->last_source_texture_);
    mesh_filter_option_.uniform_values.insert({ "uLastSource", u_last_source_value });
    
    std::shared_ptr<UniformValue> u_params_value = std::make_shared<UniformValue>(UniformDataType::FLOAT_VEC4, true, "uParams");
    mesh_filter_option_.uniform_values.insert({ "uParams", u_params_value });
    
    class DelayFilterRenderPassDelegate : public RenderPassDelegate {
    public:
        
        DelayFilterRenderPassDelegate(DelayFilterDefine* filter_define, MarsRenderer* renderer) : delay_filter_define_(filter_define), renderer_(renderer) {
            
        };
        
        void DidEndRenderPass(const RenderPass* render_pass, const RenderState& render_state) override {
            const auto& rp_color_attachment = render_pass->GetColorAttachment();
            if (rp_color_attachment) {
                std::shared_ptr<MarsTexture> source = rp_color_attachment->GetMarsTexture();
                renderer_->GetRendererExt()->CopyTexture(source, delay_filter_define_->last_source_texture_);
                delay_filter_define_->filter_params_[0] = 2.0f;
            } else {
                MLOGE("DelayFilterRenderPassDelegate RenderPass Color Attachment is null");
            }
        };
        
    private:
        DelayFilterDefine* delay_filter_define_ = nullptr;
        MarsRenderer* renderer_ = nullptr;
    };
    
    delegate_ = std::make_shared<DelayFilterRenderPassDelegate>(this, composition->renderer_);
}

DelayFilterDefine::~DelayFilterDefine() {
    DEBUG_MLOGD("DelayFilterDefine Destruct");
}

std::vector<std::shared_ptr<RenderPass>> DelayFilterDefine::GeneratePreRenderPasses(std::vector<std::shared_ptr<MarsMesh>> meshes) {
    std::vector<std::shared_ptr<RenderPass>> pre_render_pass;
    return pre_render_pass;
}

void DelayFilterDefine::GetVariableUniformValue(const std::string& key, float life, std::shared_ptr<UniformValue> value) {
    if (key == "uParams") {
        value->SetData(filter_params_, 0, 4);
    }
}

}
