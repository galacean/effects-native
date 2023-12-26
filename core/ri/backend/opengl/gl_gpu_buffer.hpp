//
//  gl_gpu_buffer.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef gl_gpu_buffer_hpp
#define gl_gpu_buffer_hpp

#include <stdio.h>

#include "ri/backend/opengl/gl_api.hpp"

namespace mn {

class GLGPURenderer;

class GLGPUBuffer {
    
public:

    GLGPUBuffer(GLGPURenderer* renderer, GLenum target, GLenum type, GLenum usage);
    
    ~GLGPUBuffer();
    
    void Bind();
    
    bool BufferData(size_t byte_length, const void* data);
    
    bool BufferSubData(size_t byte_offset, size_t byte_length, const void* data);

    void AssignRenderer(GLGPURenderer* renderer);
    
    GLenum Type() {
        return type_;
    }
    
    size_t ElementCount() {
        if (byte_per_element_ != 0) {
            return byte_length_ / byte_per_element_;
        }
        return 0;
    }
    
private:
    
    size_t byte_per_element_ = 0;
    
    GLenum target_ = 0;
    
    // FLOAT, INT, SHORT
    GLenum type_ = 0;
    
    // STATIC_DRAW, DYNAMIC_DRAW, STREAM_DRAW
    GLenum usage_ = 0;
    
    GLGPURenderer* renderer_ = nullptr;
    
    uint32_t buffer_ = 0;
    
    size_t byte_length_ = 0;
    
};

}

#endif /* gl_gpu_buffer_hpp */
