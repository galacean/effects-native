//
//  gl_frame_buffer.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_frame_buffer.hpp"
#include "util/log_util.hpp"
#include "ri/backend/opengl/gl_gpu_renderer.hpp"

namespace mn {

GLFrameBuffer::GLFrameBuffer(GLGPURenderer* renderer, std::vector<GLTexture*> color_attachments, int viewport[4], float viewport_scale) : renderer_(renderer), viewport_scale_(viewport_scale), depth_stencil_storage_type_(RenderPassAttachmentStorageType::NONE_TYPE) {
    
    viewport_[0] = viewport[0];
    viewport_[1] = viewport[1];
    viewport_[2] = viewport[2];
    viewport_[3] = viewport[3];
    
    color_textures_.clear();
    color_textures_ = color_attachments;
    this->CheckAttachment();
    MLOGD("GLFrameBuffer Construct viewport %d, %d, %d, %d", viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
}

GLFrameBuffer::GLFrameBuffer(GLGPURenderer* renderer, std::vector<GLTexture*> color_attachments, int viewport[4], RenderPassAttachmentStorageType depth_stencil_type, float viewport_scale) :  renderer_(renderer),  viewport_scale_(viewport_scale), depth_stencil_storage_type_(depth_stencil_type) {
    
    viewport_[0] = viewport[0];
    viewport_[1] = viewport[1];
    viewport_[2] = viewport[2];
    viewport_[3] = viewport[3];

    color_textures_.clear();
    color_textures_ = color_attachments;
    this->CheckAttachment();
    
    MLOGD("GLFrameBuffer Construct viewport %d, %d, %d, %d", viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
}

GLFrameBuffer::~GLFrameBuffer() {
    if (frame_buffer_) {
        OpenGLApi::DeleteFramebuffer(frame_buffer_);
        frame_buffer_ = 0;
    }
    
    if (depth_stencil_render_buffer_) {
        delete depth_stencil_render_buffer_;
    }
    
    if (depth_texture_) {
        delete depth_texture_;
    }
    
    if (stencil_texture_) {
        delete stencil_texture_;
    }
}

void GLFrameBuffer::CheckAttachment() {
    bool will_use_fbo = color_textures_.size() > 0;

    if (will_use_fbo) {
        frame_buffer_ = this->renderer_->CreateGLFrameBuffer();
    }
    
    if (color_textures_.size() > 1 && !renderer_->GPUCapability()->DrawBuffers()) {
        MLOGE("multiple color attachment not support");
        assert(false);
    }

    if (!will_use_fbo && depth_stencil_storage_type_ != RenderPassAttachmentStorageType::NONE_TYPE) {
        MLOGE("use depth stencil attachment without color attachments");
        assert(false);
    }
    
    const RenderPassAttachmentStorageType storage_type = depth_stencil_storage_type_;
    if (storage_type == RenderPassAttachmentStorageType::DEPTH_STENCIL_OPAQUE) {
        depth_stencil_render_buffer_ = new GLRenderBuffer(this->renderer_, storage_type, GL_DEPTH_STENCIL, GL_DEPTH_STENCIL_ATTACHMENT);
    } else if (storage_type == RenderPassAttachmentStorageType::DEPTH_16_OPAQUE) {
        depth_stencil_render_buffer_ = new GLRenderBuffer(this->renderer_, storage_type, GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);
    } else if (storage_type == RenderPassAttachmentStorageType::STENCIL_8_OPAQUE) {
        depth_stencil_render_buffer_ = new GLRenderBuffer(this->renderer_, storage_type, GL_STENCIL_INDEX8, GL_STENCIL_ATTACHMENT);
    } else if (storage_type == RenderPassAttachmentStorageType::DEPTH_16_TEXTURE) {
        // todo: GL3.0
        TextureOption framebuffer_option(0, 0, TextureSourceType::FRAMEBUFFER);
//        framebuffer_option.texture_info.format = GL_DEPTH_COMPONENT;
//        framebuffer_option.texture_info.internal_format = GL_DEPTH_COMPONENT;
//        framebuffer_option.texture_info.type = GL_UNSIGNED_SHORT;
//        depth_texture_ = new GLTexture(this->renderer_, framebuffer_option);
        assert(false);
    } else if (storage_type == RenderPassAttachmentStorageType::DEPTH_24_STENCIL_8_TEXTURE) {
//        TextureFrameBufferOption framebuffer_option(0, 0);
//        framebuffer_option.texture_info.format = GL_DEPTH_STENCIL;
//        framebuffer_option.texture_info.internal_format = GL_DEPTH24_STENCIL8;
//        framebuffer_option.texture_info.type = GL
//        depth_texture_ = new GLTexture(this->renderer_, frame_buffer_option);
        assert(false);
    } else {
        MLOGD("not support depth attachment type");
    }
}

void GLFrameBuffer::Bind() {
    if (this->frame_buffer_ > 0) {
        GLState& state = this->renderer_->State();
        state.BindFrameBuffer(GL_FRAMEBUFFER, frame_buffer_);
        state.ViewPort(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
        
        if (!is_ready_) {
            if (depth_stencil_render_buffer_) {
                depth_stencil_render_buffer_->SetSize(viewport_[2], viewport_[3]);
                OpenGLApi::FramebufferRenderbuffer(GL_FRAMEBUFFER, depth_stencil_render_buffer_->GetAttachment(), GL_RENDERBUFFER, depth_stencil_render_buffer_->RenderBuffer());
            } else if (depth_texture_) {
                TextureRawData raw_data(viewport_[2], viewport_[3]);
                depth_texture_->Update(raw_data);
                GLenum attachment = depth_texture_ && stencil_texture_ ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
                OpenGLApi::FramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, depth_texture_->GetTexture(), 0);
            }
            
            this->ResetColorTextures(this->color_textures_);
            GLenum status = OpenGLApi::CheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE) {
                GLenum err = OpenGLApi::GetError();
                MLOGE("BindGLFrameBuffer Failed 0x%04X", err);
                return ;
            }
            
            is_ready_ = true;
        }
    }
}

void GLFrameBuffer::ResetColorTextures(std::vector<GLTexture*> color_textures) {
    int width = this->viewport_[2];
    int height = this->viewport_[3];
    
    if (color_textures_.size() > 0) {
        this->color_textures_.clear();
        this->color_textures_ = color_textures;
    }
    
    for (int i=0; i<color_textures_.size(); i++) {
        // todo: 优化下TextureOption结构
        GLTexture* color_texture = color_textures_[i];
//        TextureOption texture_option(width, height, TextureSourceType::FRAMEBUFFER);
        TextureRawData raw_data(width, height);
        color_texture->Update(raw_data);
        
        GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
        OpenGLApi::FramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, color_texture->GetTexture(), 0);
    }
}

void GLFrameBuffer::UnBind() {
    // todo:在外层处理了；Bind To DefaultFrameBuffer;
}

void GLFrameBuffer::Resize(int32_t x, int32_t y, int32_t w, int32_t h) {
    
    if (viewport_[0] != x || viewport_[1] != y || viewport_[2] != w || viewport_[3] != h) {
        viewport_[0] = x;
        viewport_[1] = y;
        viewport_[2] = w;
        viewport_[3] = h;
        
        is_ready_ = false;
        this->Bind();
    }
}

}
