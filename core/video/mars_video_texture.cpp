//
//  mars_video_texture.cpp
//  MarsNative
//
//  Created by changxing on 2023/9/27.
//  Copyright © 2023 Alipay. All rights reserved.
//

#include "mars_video_texture.hpp"
#include "ri/backend/opengl/gl_api.hpp"

namespace mn {

MarsVideoTexture::MarsVideoTexture(MarsRenderer *renderer, const TextureOption& texture_option, VideoContext* vc)
        : MarsTexture(renderer, texture_option) {
    vc_ = vc;
    MLOGD("MarsVideoTexture %p %p", this, vc_);
}

MarsVideoTexture::~MarsVideoTexture() {
    MLOGD("~MarsVideoTexture %p %p", this, vc_);
    if (frame_buffer_ != -1) {
        OpenGLApi::DeleteFramebuffer(frame_buffer_);
        frame_buffer_ = -1;
    }
}

bool MarsVideoTexture::UpdateVideoFrame() {
    if (!vc_) {
        return false;
    }
    if (frame_buffer_ == -1) {
        frame_buffer_ = OpenGLApi::CreateFramebuffer();
        OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, frame_buffer_);
        OpenGLApi::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inner_texture_->GetTexture(), 0);
        if (OpenGLApi::CheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            MLOGE("MarsVideoTexture::UpdateVideoFrame CheckFramebufferStatus fail");
            return false;
        }
    }
    return vc_->RenderToFBO(frame_buffer_);
}

void MarsVideoTexture::SeekTo(int ms) {
    MLOGD("MarsVideoTexture::SeekTo %d", ms);
    vc_->SeekTo(ms);
}

void MarsVideoTexture::Pause() {
    if (paused_) {
        MLOGD("MarsVideoTexture::Pause, skip");
        return;
    }
    MLOGD("MarsVideoTexture::Pause");
    paused_ = true;
    vc_->Pause();
}

void MarsVideoTexture::Resume() {
    if (!paused_) {
        MLOGD("MarsVideoTexture::Resume, skip");
        return;
    }
    MLOGD("MarsVideoTexture::Resume");
    vc_->Resume();
    paused_ = false;
}

}
