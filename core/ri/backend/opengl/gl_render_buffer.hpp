//
//  gl_render_buffer.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_render_buffer_hpp
#define gl_render_buffer_hpp

#include <stdio.h>
#include "ri/gl.h"
#include "ri/render_base.hpp"

namespace mn {

class GLGPURenderer;

class GLRenderBuffer {
    
public:
        
    GLRenderBuffer(GLGPURenderer* renderer, RenderPassAttachmentStorageType storage_type, GLenum format, GLenum attachment);
    
    ~GLRenderBuffer();
    
    void SetSize(int width, int height);
    
    GLenum GetAttachment() {
        return attachment_;
    }
    
    GLuint RenderBuffer() {
        return render_buffer_;
    }
    
private:
    
    GLGPURenderer* renderer_ = nullptr;
    
    GLuint render_buffer_ = 0;
    
    RenderPassAttachmentStorageType storage_type_ = RenderPassAttachmentStorageType::NONE_TYPE;
    
    GLenum attachment_;
    
    GLenum internal_format_;
    
    GLenum format_;
    
    int width_ = 0;
    
    int height_ = 0;

};

}

#endif /* gl_render_buffer_hpp */
