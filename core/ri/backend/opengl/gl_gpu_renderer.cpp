//
//  gl_gpu_renderer.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_gpu_renderer.hpp"
#include "util/log_util.hpp"

namespace mn {

GLGPURenderer::GLGPURenderer() : shader_library_(this), empty_texture_(nullptr) {
    TextureOption option(1, 1, TextureSourceType::EMPTY);
    uint8_t data[1] = { 255 };
    option.SetData(data);
    empty_texture_ = new GLTexture(this, option);
    gpu_capability_ = std::make_shared<GLGPUCapability>();
}

GLGPURenderer::~GLGPURenderer() {
    DEBUG_MLOGD("GLGPURenderer Destruct");
    if (empty_texture_) {
        delete empty_texture_;
    }
}

void GLGPURenderer::Resize() {
    // todo: 10.2.90 resize framebuffer;
}

GLuint GLGPURenderer::CreateGLRenderBuffer() {
    return OpenGLApi::CreateRenderbuffer();
}

GLuint GLGPURenderer::CreateGLFrameBuffer() {
    return OpenGLApi::CreateFramebuffer();
}

GLuint GLGPURenderer::CreateGLTexture() {
    return OpenGLApi::CreateTexture();
}

GLuint GLGPURenderer::CreateGLBuffer() {
    return OpenGLApi::CreateBuffer();
}

}
