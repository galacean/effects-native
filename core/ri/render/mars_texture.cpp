//
//  mars_texture.cpp
//
//  Created by Zongming Liu on 2022/4/7.
//

#include "mars_texture.hpp"
#include "ri/render_base.hpp"
#include "ri/render/mars_renderer.hpp"

namespace mn {

MarsTexture::MarsTexture(MarsRenderer *renderer, const TextureOption& texture_option) :
                        renderer_(renderer), width_(texture_option.raw_data.width), height_(texture_option.raw_data.height) {
    if (renderer) {
        inner_texture_ = new GLTexture(renderer_->InnerRenderer(), texture_option);
        this->AssignRenderer(renderer);
    }
}

MarsTexture::~MarsTexture() {
    if (inner_texture_) {
        delete inner_texture_;
    }
}

void MarsTexture::AssignRenderer(MarsRenderer *renderer) {
    if (!renderer_) {
        renderer_ = renderer;
    }
}

// todo??
//    void uploadCurrentVideoFrame();

void MarsTexture::UpdateSource(const TextureRawData& source_option) {
    this->inner_texture_->Update(source_option);
}

void MarsTexture::OffloadData() {
    
}

void MarsTexture::ReloadDataAsync() {
    
}

}
