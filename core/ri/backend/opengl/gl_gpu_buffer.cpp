//
//  gl_gpu_buffer.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_gpu_buffer.hpp"
#include "ri/backend/opengl/gl_gpu_renderer.hpp"
#include "ri/utils.hpp"

namespace mn {

GLGPUBuffer::GLGPUBuffer(GLGPURenderer *renderer, GLenum target, GLenum type, GLenum usage): renderer_(renderer), target_(target), usage_(usage), type_(type) {
    buffer_ = renderer_->CreateGLBuffer();
    byte_per_element_ = Utils::GetBytePerElementsByGLType(type);
}

GLGPUBuffer::~GLGPUBuffer() {
    if (buffer_) {
        OpenGLApi::DeleteBuffer(buffer_);
    } else {
        MLOGD("GLGPUBuffer Delete buffer_ is zero");
    }
}

void GLGPUBuffer::Bind() {
    OpenGLApi::BindBuffer(target_, buffer_);
}

bool GLGPUBuffer::BufferData(size_t byte_length, const void* data) {
    byte_length_ = byte_length;
    this->Bind();
    OpenGLApi::BufferData(target_, byte_length, data, usage_);
    GLenum err;
    if ((err = OpenGLApi::GetError()) != GL_NO_ERROR) {
        MLOGE("BufferData Error 0x%04X", err);
        return false;
    }
    return true;
}

bool GLGPUBuffer::BufferSubData(size_t byte_offset, size_t byte_length, const void* data) {
    this->Bind();
    size_t total_byte_length = byte_offset + byte_length;
    
    // While creating the new storage, any pre-existing data store is deleted.
    if (total_byte_length > byte_length_) {
        DEBUG_MLOGD("BufferSubData resize for outofrange byte_length_ %zu dataSize %zu", byte_length_, total_byte_length);
        byte_length_ = total_byte_length;
        OpenGLApi::BufferData(target_, byte_length_, nullptr, usage_);
    }
    OpenGLApi::BufferSubData(target_, byte_offset, byte_length, data);
    return true;

}

void GLGPUBuffer::AssignRenderer(GLGPURenderer *renderer) {
    
}

}
