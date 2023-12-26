//
//  gl_gpu_renderer.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_gpu_renderer_hpp
#define gl_gpu_renderer_hpp

#include <stdio.h>
#include <vector>
#include <string>

#include "ri/backend/opengl/gl_gpu_capability.hpp"
#include "ri/backend/opengl/gl_frame_buffer.hpp"
#include "ri/backend/opengl/gl_render_buffer.hpp"
#include "ri/backend/opengl/gl_gpu_buffer.hpp"
#include "ri/backend/opengl/gl_state.hpp"
#include "ri/backend/opengl/gl_shader_library.hpp"
#include "ri/backend/opengl/gl_texture.hpp"

#include "util/log_util.hpp"
#include "ri/backend/opengl/gl_api.hpp"

namespace mn {

class GLGPURenderer {
    
public:
    
    GLGPURenderer();
    
    ~GLGPURenderer();
    
    void Resize();
    
    GLuint CreateGLRenderBuffer();
    
    GLuint CreateGLFrameBuffer();
    
    GLuint CreateGLTexture();
    
    GLuint CreateGLBuffer();
    
    GLState& State() {
        return state_;
    }
    
    GLShaderLibrary& ShaderLibrary() {
        return shader_library_;
    }
    
    std::shared_ptr<GLGPUCapability> GPUCapability() {
        return gpu_capability_;
    }
    
    GLTexture *EmptyTexture() {
        return empty_texture_;
    }

private:
    
    std::shared_ptr<GLGPUCapability> gpu_capability_;
    
    GLShaderLibrary shader_library_;
    
    GLState state_;
    
    GLTexture *empty_texture_;
    
    std::vector<GLuint> framebuffers_;
    
    
    
    int width_;
    
    int height_;
    
};

}

#endif /* gl_gpu_renderer_hpp */
