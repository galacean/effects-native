//
//  render_pass.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "render_pass.hpp"
#include "util/log_util.hpp"
#include "ri/render/mars_renderer.hpp"
#include "ri/backend/opengl/gl_frame_buffer.hpp"

namespace mn {

RenderPass::RenderPass(MarsRenderer *renderer, int priority) : priority_(priority), name_("defalut_pass"), renderer_(renderer) {
    if (renderer_) {
        this->AssignRenderer(renderer_);
    }
}

RenderPass::RenderPass(MarsRenderer *renderer, const std::string& name , int priority) :priority_(priority), name_(name), renderer_(renderer) {
    if (renderer_) {
        this->AssignRenderer(renderer_);
    }
}

RenderPass::RenderPass(MarsRenderer* renderer, int viewport[4], const std::string& name, int priority) : priority_(priority), name_(name), renderer_(renderer) {
    custom_viewport_ = true;
    viewport_[0] = viewport[0];
    viewport_[1] = viewport[1];
    viewport_[2] = viewport[2];
    viewport_[3] = viewport[3];
    
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

RenderPass::~RenderPass() {
    MLOGD("Render Pass Destructor %s", name_.c_str());
}

void RenderPass::AssignRenderer(MarsRenderer *renderer) {
//    if (renderer) {
//        renderer_ = renderer;
//    }
}

void RenderPass::SetRenderPassAttachent(const RenderPassAttachmentOptions& attachment_options, CLEAR_ACTION_TYPE clear_action_type) {
    clear_action_ = std::make_shared<RenderPassClearAction>(clear_action_type);
    this->ResetAttachments(attachment_options);
}

void RenderPass::SetRenderPassDelegate(std::shared_ptr<RenderPassDelegate> delegate) {
    render_pass_delegate_ = delegate;
}

void RenderPass::SetSemanticValues(const std::string& key, std::shared_ptr<SemanticValue> value) {
    semantic_values_.insert({ key, value});
}

void RenderPass::SetCamera(std::shared_ptr<Camera> camera) {
    camera_ = camera;
}

void RenderPass::Bind() {
    if (this->frame_buffer_) {
        this->frame_buffer_->Bind();
    } else {
        GLState& state = renderer_->InnerRenderer()->State();
        int viewport[4] = { 0 };
        this->GetViewport(viewport);
        state.ViewPort(viewport[0], viewport[1], viewport[2], viewport[3]);
    }
}

void RenderPass::GetViewport(int* viewport) {
    if (custom_viewport_) {
        viewport[0] = viewport_[0];
        viewport[1] = viewport_[1];
        viewport[2] = viewport_[2];
        viewport[3] = viewport_[3];
    } else {
        int width = renderer_->GetSurfaceWidth();
        int height = renderer_->GetSurfaceHeight();
        viewport[0] = 0;
        viewport[1] = 0;
        viewport[2] = width * this->viewport_scale_;
        viewport[3] = height * this->viewport_scale_;
    }
}


// todo: 外部调用resetColorAttachments;
void RenderPass::ResetColorAttachments(std::vector<MarsTexture*> colors) {
    if (colors.size() == 0) {
        RenderPassAttachmentOptions attachment_option;
        this->ResetAttachments(attachment_option);
    }
    
    if (this->attachments_.size() == 0) {
        
    } else {
        
    }
}

void RenderPass::ResetAttachments(const RenderPassAttachmentOptions& attachment_option) {
    this->SetViewPortOptions(attachment_option);
    this->InnerResetAttachMent(attachment_option.color_attachment_options, attachment_option.depth_stencil_storage_type);
}

void RenderPass::SetViewPortOptions(const RenderPassAttachmentOptions& options) {
    if (options.is_custom_viewport) {
        viewport_[0] = options.viewport[0];
        viewport_[1] = options.viewport[1];
        viewport_[2] = options.viewport[2];
        viewport_[3] = options.viewport[3];

        custom_viewport_ = true;
        viewport_scale_ = options.viewport_scale;
        if (frame_buffer_) {
            frame_buffer_->Resize(options.viewport[0], options.viewport[1], options.viewport[2], options.viewport[3]);
        }
    } else {
        custom_viewport_ = false;
        viewport_scale_ = options.viewport_scale;
        if (frame_buffer_) {
            frame_buffer_->SetIsCustomViewport(false);
        }
    }
}

void RenderPass::InnerResetAttachMent(std::vector<RenderPassColorAttachmentOptions> color_attachment_options, RenderPassAttachmentStorageType storage_type) {
    if (color_attachment_options.size() > 0) {
        if (attachments_.size()) {
            attachments_.clear();
            frame_buffer_.reset();
        }

        size_t attachment_length = color_attachment_options.size();
        std::vector<GLTexture*> gl_color_attachments;
        for (size_t i=0; i<attachment_length; i++) {
            std::shared_ptr<RenderPassColorAttachment> color_attachment = std::make_shared<RenderPassColorAttachment>(renderer_, color_attachment_options[i]);
            
            this->attachments_.push_back(color_attachment);
            gl_color_attachments.push_back(color_attachment->GetMarsTexture()->GetInnerTexture());
        }
        
        int viewport[4] = { 0 };
        this->GetViewport(viewport);
        frame_buffer_ = std::make_shared<GLFrameBuffer>(renderer_->InnerRenderer(), gl_color_attachments, viewport, storage_type, viewport_scale_);
        frame_buffer_->Bind();
        frame_buffer_->UnBind();
    } else {
        attachments_.clear();
    }
}

void RenderPass::AddMesh(std::shared_ptr<MarsMesh> mesh) {
    for (int i = 0; i < meshes_.size(); i++) {
        if (meshes_[i] == mesh) {
            return;
        }
    }
    
    meshes_.push_back(mesh);
    int idx = meshes_.size() - 1;
    if (idx > 0) {
        if (mesh_order_ != RenderPassMeshOrder::ASCENDING) {
            while (meshes_[idx - 1]->priority_ < mesh->priority_) {
                meshes_[idx] = meshes_[idx - 1];
                meshes_[idx - 1] = mesh;
                idx--;
                if (idx == 0) {
                    break;
                }
            }
        } else {
            while (meshes_[idx - 1]->priority_ > mesh->priority_) {
                meshes_[idx] = meshes_[idx - 1];
                meshes_[idx - 1] = mesh;
                idx--;
                if (idx == 0) {
                    break;
                }
            }
        }
    }
}

void RenderPass::RemoveMesh(std::shared_ptr<MarsMesh> mesh) {
    for (auto iter = meshes_.begin(); iter != meshes_.end(); iter++) {
        if (*iter == mesh) {
            meshes_.erase(iter);
            return;
        }
    }
}

void RenderPass::SetMeshes(const std::vector<std::shared_ptr<MarsMesh>> meshes) {
    this->meshes_.clear();
    for (int i=0; i<meshes.size(); i++) {
        this->AddMesh(meshes[i]);
    }
}

void RenderPass::GetStencilAttachment() {
    assert(0);
}

void RenderPass::GetDepthAttachment() {
    assert(0);
}

std::shared_ptr<RenderPassColorAttachment> RenderPass::GetColorAttachment() const {
    if (attachments_.size() > 0) {
        return attachments_[0];
    }
    
    return nullptr;
}

}

