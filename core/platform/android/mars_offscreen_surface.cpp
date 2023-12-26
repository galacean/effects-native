#include "mars_offscreen_surface.h"
#include "egl_pbuffer_context.h"
#include "util/log_util.hpp"
#include "ri/backend/opengl/gl_api.hpp"

namespace mn {

MarsOffscreenSurface::MarsOffscreenSurface() {
    context_ = new EGLPBufferContext;
    MLOGD("MarsOffscreenSurface context %p", context_);
}

MarsOffscreenSurface::~MarsOffscreenSurface() {
    if (context_) {
        context_->MakeCurrent();
        if (framebuffer_) {
            OpenGLApi::DeleteFramebuffer(framebuffer_);
            framebuffer_ = 0;
        }
        if (color_attachment_) {
            OpenGLApi::DeleteTexture(color_attachment_);
            color_attachment_ = 0;
        }
        if (depth_stencil_attachment_) {
            OpenGLApi::DeleteRenderbuffer(depth_stencil_attachment_);
            depth_stencil_attachment_ = 0;
        }

        delete context_;
        context_ = nullptr;
    }
    MLOGD("MarsOffscreenSurface::Destruct");
}

bool MarsOffscreenSurface::Create(const SystemSurface &system_surface, int32_t s_width, int32_t s_height) {
    bool result = context_->Init(system_surface.surface);
    if (!result) {
        return false;
    }
    width_ = s_width;
    height_ = s_height;
    context_->MakeCurrent();
    result = CreateFBO();
    MLOGD("MarsOffscreenSurface::Create %d %p %u %d %d", result, context_, framebuffer_, width_, height_);
    return result;
}

int MarsOffscreenSurface::MakeCurrent() {
    return context_->MakeCurrent();
}

int MarsOffscreenSurface::SwapBuffer() {
    MakeCurrent();
    OpenGLApi::Finish();
    return 1;
}

bool MarsOffscreenSurface::IsValid() {
    return context_ && context_->IsValid();
}

bool MarsOffscreenSurface::CreateFBO() {
    framebuffer_ = OpenGLApi::CreateFramebuffer();
    if (framebuffer_ == 0) {
        MLOGE("CreateFBO create framebuffer fail");
        return false;
    }
    OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    color_attachment_ = OpenGLApi::CreateTexture();
    if (color_attachment_ == 0) {
        MLOGE("CreateFBO create color attachment fail");
        return false;
    }
    OpenGLApi::BindTexture(GL_TEXTURE_2D, color_attachment_);
    OpenGLApi::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment_, 0);
    OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    OpenGLApi::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    if (OpenGLApi::GetError()) {
        MLOGE("CreateFBO init color attachment fail");
        return false;
    }
    depth_stencil_attachment_ = OpenGLApi::CreateRenderbuffer();
    if (depth_stencil_attachment_ == 0) {
        MLOGE("CreateFBO create depth stencil attachment fail");
        return false;
    }
    OpenGLApi::BindRenderbuffer(GL_RENDERBUFFER, depth_stencil_attachment_);
    OpenGLApi::RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, width_, height_);
    OpenGLApi::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_attachment_);
    OpenGLApi::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stencil_attachment_);
    if (OpenGLApi::GetError()) {
        MLOGE("CreateFBO init depth stencil attachment fail");
        return false;
    }
    uint32_t status = OpenGLApi::CheckFramebufferStatus(GL_FRAMEBUFFER);
    return (status == GL_FRAMEBUFFER_COMPLETE);
}

int MarsOffscreenSurface::GetGLESVersion() {
    if (context_) {
        return context_->GetGLESVersion();
    }
    return 0;
}

}
