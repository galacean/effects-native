//
//  gl_texture.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//
// todo: texture cube;

#ifndef ri_gl_texture_hpp
#define ri_gl_texture_hpp

#include <stdio.h>
#include "ri/gl.h"
#include "ri/render_base.hpp"

namespace mn {

class GLGPURenderer;

class GLTexture {
    
public:
    
    GLTexture(GLGPURenderer *renderer, const TextureOption& texture_option);
    
    ~GLTexture();
        
    void Update(const TextureRawData& source_option);
    
    void UpdateMipMap(const TextureMipmapRawData& mip_map_option);
    
    void OffloadData();
    
    int GetWidth() {
        return width_;
    };
    
    int GetHeight() {
        return height_;
    };
    
    GLuint GetTexture() {
        return texture_;
    }
    
    GLenum Target() {
        return texture_info_.target;
    }

    bool GetPremultiplyAlpha() const {
        return premultiply_alpha_;
    }
        
    GLTextureInfo& GetGLTextureInfo() {
        return texture_info_;
    }

    void SetTextureFilters();
    
    void Bind();
    
private:
    
    void GetCompressTextureOptions(const TextureRawData& texture_option);
    
    bool CheckKTXValid(uint8_t *data);
    
        
    void ResizeImage();
        
    GLGPURenderer *renderer_ = nullptr;
    
    uint32_t texture_ = 0;
    
    int width_ = 0;
    
    int height_ = 0;
    
    TextureSourceType source_type_;
    
    GLTextureInfo texture_info_;
        
    bool use_mipmap_ = false;

    bool premultiply_alpha_ = false;
    
};

}

#endif /* gl_texture_hpp */
