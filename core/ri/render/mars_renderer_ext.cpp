//
//  mars_renderer_ext.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "mars_renderer_ext.hpp"
#include "ri/render/render_pass.hpp"
#include "ri/render/render_frame_internal.hpp"

namespace mn {

MarsRendererExt::MarsRendererExt(MarsRenderer* renderer) : renderer_(renderer) {
    this->CreateCopyRenderPass();
}

void MarsRendererExt::CopyTexture(std::shared_ptr<MarsTexture> source, std::shared_ptr<MarsTexture> dest) {
    int width = dest->Width() != 0 ? dest->Width() : source->Width();
    int height = dest->Height() != 0 ? dest->Height() : source->Height();
    
    TextureRawData raw_data(width, height);
    dest->UpdateSource(raw_data);
    
    // Copy Texture;
    RenderPassAttachmentOptions attachment_option(0, 0, width, height);
    attachment_option.AddRenderPassColorAttachmentOption(dest);
    copy_render_pass_->SetRenderPassAttachent(attachment_option, CLEAR_ACTION_TYPE::CLEAR_DEFAULT);
    
    auto& meshes = copy_render_pass_->Meshes();
    if (meshes.size() > 0) {
        auto tex_u_value = meshes[0]->GetMaterial()->GetUniformValue("uTex");
        tex_u_value->SetTexture(source);
    }
    
    RenderState state;
    RenderFrameInternal::RenderRenderPass(renderer_, copy_render_pass_, state);
}

void MarsRendererExt::CreateCopyRenderPass() {
    std::string mesh_name = "mri-copy";
    copy_render_pass_ = std::make_shared<RenderPass>(renderer_, "mri-copy-rp");
    
    // todo: 测试下GL_TRIANGLE_STRIP;
    std::shared_ptr<MarsMesh> mesh = std::make_shared<MarsMesh>(renderer_, mesh_name);
//    auto geometry = std::make_shared<MarsGeometry>(renderer_, 4, 0, GL_TRIANGLE_STRIP);
//    float pos[8] = { -1, 1, -1, -1, 1, 1, 1, -1 };
//    AttributeWithData pos_data;
    
    std::shared_ptr<MarsGeometry> geometry = std::make_shared<MarsGeometry>(renderer_, 6, 0, GL_TRIANGLES);
    std::shared_ptr<mn::GPUBufferOption> index_buffer = std::make_shared<mn::GPUBufferOption>(mn::BufferDataType::UNSIGNED_SHORT);
    uint16_t index[6] = { 0, 1, 2, 2, 1, 3 };
    index_buffer->SetData((uint8_t *)index, 6 * sizeof(uint16_t));
    geometry->SetIndexInfo(index_buffer);
    
    float pos[] = { -1, 1, -1, -1, 1, 1, 1, -1 };
    AttributeWithData attr_pos(2, 2 * sizeof(float), 0);
    attr_pos.SetData((uint8_t *)pos, 8 * sizeof(float));
    geometry->SetAttributeInfo("aPos", attr_pos);
    mesh->SetGeometry(geometry);
    
    std::string copy_vertex = R"(
        precision highp float;
        attribute vec2 aPos;varying vec2 vTex;
        void main(){
            gl_Position = vec4(aPos,0.,1.0);
            vTex = (aPos + vec2(1.))/2.;
        }
    )";

    std::string copy_frag = R"(
        precision highp float;
        varying vec2 vTex;
        uniform sampler2D uTex;
        void main(){
            gl_FragColor = texture2D(uTex,vTex);
        }
    )";
    
    Shader copy_shader;
    copy_shader.fragment = copy_frag;
    copy_shader.vertex = copy_vertex;
    copy_shader.name = mesh_name;
    
    
    MaterialRenderStates status;
    status.depth_test = std::make_shared<MBool>(false);
    status.cull_face_enabled = std::make_shared<MBool>(false);
    status.stencil_test = std::make_shared<MBool>(false);
    
    auto material = std::make_shared<MarsMaterial>(renderer_, copy_shader, status, mesh_name);
    
    std::shared_ptr<UniformValue> u_tex_value = std::make_shared<UniformValue>(UniformDataType::SAMPLER_2D);
    material->SetUniformValue("uTex", u_tex_value);
    
    mesh->SetMaterial(material);
    copy_render_pass_->AddMesh(mesh);
}

}
