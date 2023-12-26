//
//  render_pass_color_attachment.cpp
//
//  Created by Zongming Liu on 2022/4/8.
//

#include "render_pass_color_attachment.hpp"
#include "util/log_util.hpp"

namespace mn {

RenderPassColorAttachment::RenderPassColorAttachment(MarsRenderer* renderer, const RenderPassColorAttachmentOptions& color_attachment_option) {
    if (color_attachment_option.texture) {
        texture_ = color_attachment_option.texture;
        external_texture_ = true;
    } else if (color_attachment_option.use_texture_option) {
        external_texture_ = false;
        this->AssignRenderer(renderer, color_attachment_option.texture_option);
    } else {
        MLOGE("RenderPassColorAttachment construct failed");
    }
}

RenderPassColorAttachment::RenderPassColorAttachment(MarsRenderer* renderer, std::shared_ptr<MarsTexture> texture) {
    texture_ = texture;
    external_texture_ = true;
}

RenderPassColorAttachment::~RenderPassColorAttachment() {
    MLOGD("RenderPassColorAttachment Destruct");
}

void RenderPassColorAttachment::AssignRenderer(MarsRenderer* renderer, const TextureOption& texture_option) {
    if (!texture_) {
        this->texture_ = std::make_shared<MarsTexture>(renderer, texture_option);
    }
    
    if (texture_) {
        this->texture_->AssignRenderer(renderer);
    }
}

int RenderPassColorAttachment::Width() {
    if (texture_) {
        return texture_->Width();
    }
    
    return 0;
}

int RenderPassColorAttachment::Height() {
    if (texture_) {
        return texture_->Height();
    }
    
    return 0;
}

}


