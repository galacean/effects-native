//
//  mars_texture.hpp
//
//  Created by Zongming Liu on 2022/4/7.
//

#ifndef mars_texture_hpp
#define mars_texture_hpp

#include <stdio.h>
#include <string>

#include "ri/backend/opengl/gl_texture.hpp"

namespace mn {

class MarsRenderer;

class TextureOption;

class MarsTexture {
    
public:
    
    MarsTexture(MarsRenderer *renderer, const TextureOption& texture_option);
    
    virtual ~MarsTexture();
    
    void AssignRenderer(MarsRenderer *renderer);
    
    // todo??
//    void uploadCurrentVideoFrame();
    
    GLTexture *GetInnerTexture() {

        return inner_texture_;
    }
    
    void UpdateSource(const TextureRawData& source_option);
    
    void OffloadData();
    
    void ReloadDataAsync();
    
    int Width() {
        return width_;
    }
    
    int Height() {
        return height_;
    }

    virtual bool IsVideo() const {
        return false;
    }
    
protected:
    
    int width_ = 0;
    
    int height_ = 0;
    
    MarsRenderer *renderer_ = nullptr;
    
    std::string name_;
    
    GLTexture *inner_texture_ = nullptr;
    
};

}



#endif /* mars_texture_hpp */
