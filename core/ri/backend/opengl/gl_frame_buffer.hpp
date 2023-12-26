//
//  gl_frame_buffer.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_frame_buffer_hpp
#define gl_frame_buffer_hpp

#include <stdio.h>
#include <vector>
#include "ri/gl.h"
#include "ri/render_base.hpp"
#include "ri/backend/opengl/gl_render_buffer.hpp"
#include "ri/backend/opengl/gl_texture.hpp"

namespace mn {

class GLGPURenderer;

class GLFrameBuffer {
    
public:
        
    GLFrameBuffer(GLGPURenderer* renderer, std::vector<GLTexture*> color_attachments, int viewport[4], float viewport_scale = 1);
    
    GLFrameBuffer(GLGPURenderer* renderer, std::vector<GLTexture*> color_attachments, int viewport[4], RenderPassAttachmentStorageType depth_stencil_type, float viewport_scale = 1);
    
    ~GLFrameBuffer();
    
    void Bind();
    
    void ResetColorTextures(std::vector<GLTexture*> color_textures);
    
    void UnBind();
    
    void Resize(int x, int y, int w, int h);
    
    void SetIsCustomViewport(bool value) {
        is_custom_viewport_ = value;
    }
    
    bool isCustomViewPort() {
        return is_custom_viewport_;
    }
    
private:
    
    void CheckAttachment();
        
    GLGPURenderer* renderer_;
    
    GLuint frame_buffer_ = 0;
    
    RenderPassAttachmentStorageType depth_stencil_storage_type_ ;
        
    std::vector<GLTexture*> color_textures_;
    
    GLRenderBuffer* depth_stencil_render_buffer_ = nullptr;

    GLTexture* depth_texture_ = nullptr;
    
    GLTexture* stencil_texture_ = nullptr;
    
    float viewport_scale_ = 1;
    
    bool is_custom_viewport_ = false;
    
    int viewport_[4] = { 0 };
    
    bool is_ready_ = false;
    
    // todo: 后面做
    // bool external_storage_;
    
};

}

#endif /* gl_frame_buffer_hpp */
