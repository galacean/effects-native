//
//  gl_render_buffer.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_render_buffer.hpp"
#include "util/log_util.hpp"
#include "ri/backend/opengl/gl_gpu_renderer.hpp"

namespace mn {

GLRenderBuffer::GLRenderBuffer(GLGPURenderer* renderer, RenderPassAttachmentStorageType storage_type, GLenum format, GLenum attachment) : renderer_(renderer), storage_type_(storage_type), internal_format_(format), format_(format), attachment_(attachment) {
    render_buffer_ = renderer_->CreateGLRenderBuffer();
    DEBUG_MLOGD("GLRenderBuffer Created %d", render_buffer_);
}

GLRenderBuffer::~GLRenderBuffer() {
    DEBUG_MLOGD("GLRenderBuffer Destruct");
    if (render_buffer_) {
        OpenGLApi::DeleteRenderbuffer(render_buffer_);
        render_buffer_ = 0;
    }
}

void GLRenderBuffer::SetSize(int width, int height) {
    GLState& state = this->renderer_->State();
    state.BindRenderBuffer(GL_RENDERBUFFER, render_buffer_);
    if (width > 0 && height > 0) {
        width_ = width;
        height_ = height;
        OpenGLApi::RenderbufferStorage(GL_RENDERBUFFER, internal_format_, width, height);
    } else {
        MLOGE("RenderBuffer Set Invalid Size");
    }
}

}
