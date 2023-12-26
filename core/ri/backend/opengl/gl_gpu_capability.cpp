//
//  gl_gpu_capability.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_gpu_capability.hpp"
#include "util/log_util.hpp"
#include "ri/backend/opengl/gl_api.hpp"
#include <string>
#include <list>

namespace mn {

GLGPUCapability::GLGPUCapability() {
    OpenGLApi::GetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_fragment_textures_);
    OpenGLApi::GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_vertex_uniforms_);
    OpenGLApi::GetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &max_vertex_textures_);
    OpenGLApi::GetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &max_fragment_uniforms_);
    OpenGLApi::GetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size_);
    
    const char* extensions = (const char*) OpenGLApi::GetString(GL_EXTENSIONS);
    if (extensions == nullptr) {
        uint32_t err = OpenGLApi::GetError();
        MLOGE("SetupExtensions failed, error: 0x%x", err);
        return;
    }
    size_t len = strlen(extensions);
    char *str = (char *) malloc(len + 1);
    std::list<std::string> temp;
    int idx = 0;
    for (int i = 0; i < len; i++) {
        if (extensions[i] == ' ') {
            if (idx > 0) {
                str[idx] = 0;
                temp.push_back(str);
            }
            idx = 0;
        } else {
            str[idx++] = extensions[i];
        }
    }
    if (idx > 0) {
        str[idx] = 0;
        temp.push_back(str);
    }
    for (auto gl_name : temp) {
        if (strcmp(gl_name.c_str(), "GL_KHR_texture_compression_astc_hdr") == 0 || strcmp(gl_name.c_str(), "GL_KHR_texture_compression_astc_ldr") == 0) {
            support_compressed_texture_ = true;
        }
        
        if (strcmp(gl_name.c_str(), "OES_texture_half_float") == 0) {
            half_float_texture_ = true;
        }
    }
    
    MLOGD("GPUCapability supportCompress %d, support halfFloat %d, maxVertexUniform %d, maxVertextTexture %d, maxFragUniform %d, maxFragTex %d", support_compressed_texture_, half_float_texture_, max_vertex_uniforms_, max_vertex_textures_, max_fragment_uniforms_, max_fragment_textures_);
    free(str);
}

GLGPUCapability::~GLGPUCapability() {
    MLOGD("GPU Capability Destruct");
}

int GLGPUCapability::GetMaxFragmentTextures() const {
    return max_fragment_textures_;
}

int GLGPUCapability::GetMaxFragmentUniforms() const {
    return max_fragment_uniforms_;
}

int GLGPUCapability::GetMaxVertexTextures() const {
    return max_vertex_textures_;
}

int GLGPUCapability::GetMaxVertexUniforms() const {
    return max_vertex_uniforms_;
}

int GLGPUCapability::GetMaxTextureSize() const {
    return max_texture_size_;
}

bool GLGPUCapability::SupportCompressedTexture() const {
    return support_compressed_texture_;
}

bool GLGPUCapability::SupportHalfFloatTexture() const {
    return half_float_texture_;
}

void GLGPUCapability::FrameBufferTexture2D(GLenum target, int index, int tex_target, uint32_t texture) const {
    
}

// todo: 10.2.90 多drawBuffer支持;
bool GLGPUCapability::DrawBuffers() {
    return false;
}

}
