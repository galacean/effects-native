//
//  mars_renderer.cpp
//
//  Created by Zongming Liu on 2022/4/7.
//

#include "mars_renderer.hpp"
#include "util/mars_thread.h"

namespace mn {

MarsRenderer::MarsRenderer() : surface_(nullptr), renderer_(nullptr), renderer_ext_(nullptr), surface_width_(0), surface_height_(0) {
    
}

MarsRenderer::MarsRenderer(mn::MarsSurface *surface) :surface_(surface), renderer_(nullptr), renderer_ext_(nullptr), surface_width_(0), surface_height_(0) {
    renderer_ = new GLGPURenderer();
    if (surface) {
        surface_width_ = surface->GetSurfaceWidth();
        surface_height_ = surface->GetSurfaceHeight();
    }
    
    renderer_ext_ = new MarsRendererExt(this);
    MLOGD("MarsRenderer::MarsRenderer %p %d %d", this, surface_width_, surface_height_);
}

MarsRenderer::~MarsRenderer() {
    for (auto& frame : render_frames_) {
        if (frame) {
            delete frame;
        }
    }
    
    if (renderer_) {
        delete renderer_;
    }
    
    if (renderer_ext_) {
        delete renderer_ext_;
    }
    
    // todo: mars_surface_的销毁问题;
}

// todo: Surface Resize; GLRender resize viewport?? and framebuffer viewport;
void MarsRenderer::Resize() {
    if (surface_) {
        surface_width_ = surface_->GetSurfaceWidth();
        surface_height_ = surface_->GetSurfaceHeight();
        MLOGD("MarsRenderer::Resize: surface_width_ %d surface_height_ %d", surface_width_, surface_height_);
    }
}

void MarsRenderer::MakeCurrent() {
    if (surface_) {
        surface_->MakeCurrent();
    }
}

void MarsRenderer::SwapBuffer() {
    if (surface_ && (!MarsThread::GetThreadPaused())) {
        surface_->SwapBuffer();
    }
}

// todo: options
RenderFrame* MarsRenderer::CreateRenderFrame(const std::string& name) {
    RenderFrameOptions options;
    options.name = name;
    RenderFrame *frame = new RenderFrame(this, options);
    render_frames_.push_back(frame);
    return frame;
}

std::shared_ptr<GLGPUCapability> MarsRenderer::GPUCapability() {
    if (renderer_) {
        return renderer_->GPUCapability();
    }
    
    return nullptr;
}

void MarsRenderer::BindSystemFramebuffer() {
    // todo: hack system framebuffer iOS;
    OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, surface_->GetDefaultFramebufferId());
}

}
