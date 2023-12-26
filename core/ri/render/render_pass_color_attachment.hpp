//
//  render_pass_color_attachment.hpp
//
//  Created by Zongming Liu on 2022/4/8.
//

#ifndef render_pass_color_attachment_hpp
#define render_pass_color_attachment_hpp

#include <stdio.h>
#include "ri/render_base.hpp"
#include "ri/render/mars_texture.hpp"

namespace mn {

class RenderPassColorAttachment {
  
public:
    
    RenderPassColorAttachment(MarsRenderer* renderer , const RenderPassColorAttachmentOptions& color_attachment_option);
    
    RenderPassColorAttachment(MarsRenderer* renderer, std::shared_ptr<MarsTexture> texture);
    
    ~RenderPassColorAttachment();
    
    void AssignRenderer(MarsRenderer* renderer, const TextureOption& texture_option);
    
    int Width();
    
    int Height();
    
    std::shared_ptr<MarsTexture> GetMarsTexture() {
        return texture_;
    }
    
private:
    
    RenderPassAttachmentStorageType storage_type_;
    
    std::shared_ptr<MarsTexture> texture_;
        
    bool readable_ = false;
    
    bool external_texture_ = false;
    
};

}

#endif /* render_pass_color_attachment_hpp */
