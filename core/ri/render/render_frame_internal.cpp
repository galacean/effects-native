//
//  render_frame_internal.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "render_frame_internal.hpp"


namespace mn {

void RenderFrameInternal::Render(MarsRenderer* renderer, RenderFrame* render_frame) {
    renderer->MakeCurrent();
    Clear(renderer, render_frame->GetRenderFrameClearAction().get());
    
    RenderState render_state;
    render_state.current_render_frame = render_frame;
    
    const auto& render_passes = render_frame->GetRenderPasses();
    DEBUG_MLOGD("RenderFrame Renderer render_passes size: %d", render_passes.size());
    for (auto& render_pass : render_passes) {
        render_state.current_render_pass = render_pass.get();
        auto delegate = render_pass->GetPassDelegate();
        if (delegate) {
            delegate->WillBeginRenderPass(render_pass.get(), render_state);
        }
        
        DEBUG_MLOGD("RenderFrame Render pass name: %s", render_pass->Name().c_str());
        RenderRenderPass(renderer, render_pass, render_state);
        if (delegate) {
            delegate->DidEndRenderPass(render_pass.get(), render_state);
        }
    }
}

void RenderFrameInternal::RenderRenderPass(MarsRenderer* renderer, std::shared_ptr<RenderPass> render_pass, RenderState& state) {
    state.current_render_pass = render_pass.get();
    SetupRenderPass(renderer, render_pass);
    RenderMeshesByCamera(renderer, render_pass->camera_, state);
    EndRenderPass(renderer, render_pass);
}

void RenderFrameInternal::EndRenderPass(MarsRenderer* renderer, std::shared_ptr<RenderPass> render_pass) {
    // todo: 10.3.0 clear_store_action;
    //    if (render_pass->GetFramebuffer()) {
    //        render_pass->GetFramebuffer()->UnBind();
    //    } else {
    //        this->renderer_->BindSystemFramebuffer();
    //    }
    renderer->BindSystemFramebuffer();
}

void RenderFrameInternal::Clear(MarsRenderer* renderer, const RenderPassClearAction* action) {
    uint32_t bit = 0;
    if (action->color_action) {
        OpenGLApi::ClearColor(action->clear_color[0], action->clear_color[1], action->clear_color[2], action->clear_color[3]);
        OpenGLApi::ColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        bit = GL_COLOR_BUFFER_BIT;
    }
    if (action->stencil_action) {
        OpenGLApi::StencilMask(0xFF);
        OpenGLApi::ClearStencil(action->clear_stencil);
        bit |= GL_STENCIL_BUFFER_BIT;
    }
    if (action->depth_action) {
        OpenGLApi::DepthMask(GL_TRUE);
        OpenGLApi::ClearDepthf(action->clear_depth);
        bit |= GL_DEPTH_BUFFER_BIT;
    }
    if (bit) {
        OpenGLApi::Clear(bit);
    }
}
    
void RenderFrameInternal::RenderMeshesByCamera(MarsRenderer* renderer, std::shared_ptr<Camera> camera, RenderState& state) {
    state.current_camera = camera.get();
    auto& meshes = state.current_render_pass->Meshes();
    auto delegate = state.current_render_pass->GetPassDelegate();
    DEBUG_MLOGD("RenderFrame Render renderPass meshes size: %d", meshes.size());
    for (auto& mesh : meshes) {
        state.current_mesh = mesh.get();
        DEBUG_MLOGD("RenderFrame Render meshe name: %s", mesh->Name().c_str());
        
        if (delegate) {
            delegate->WillRenderMesh(mesh, state);
        }
        
        mesh->GetMaterial()->AssignRenderer(renderer);
        GLMaterial* material = mesh->GetMaterial()->GetGLMaterial();
        if (material) {
            GLProgram* program = material->GetProgram();
            if (program) {
                material->SetUpStates();
                program->SetupUniforms(state);
                auto& geometries = mesh->GetGeometries();
                for (auto& geo : geometries) {
                    geo->AssignRenderer(renderer);
                    geo->Flush();
                    program->SetupAttributes(geo->InnerGeometry());
                    geo->Draw();
                }
            } else {
                MLOGE("Render Failed for program is null");
            }
        }
        
        if (delegate) {
            delegate->DidRenderMesh(mesh, state);
        }
    }
}

void RenderFrameInternal::SetupRenderPass(MarsRenderer* renderer, std::shared_ptr<RenderPass> render_pass) {
    //    render_pass->AssignRenderer(renderer);
    render_pass->Bind();
    if (render_pass->ClearAction()) {
        Clear(renderer, render_pass->ClearAction().get());
    }
}

}
