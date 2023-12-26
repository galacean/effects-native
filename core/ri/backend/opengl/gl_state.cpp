//
//  gl_state.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "gl_state.hpp"
#include "util/log_util.hpp"

namespace mn {

GLState::GLState() {
    current_framebuffer_ = -1;
    
    current_render_buffer_ = -1;
}

GLState::~GLState() {
    DEBUG_MLOGD("GLState Destruct");
}

void GLState::Reset() {
    
}

void GLState::Enable(GLenum capability) {
    OpenGLApi::Enable(capability);
}

void GLState::Disable(GLenum capability) {
    OpenGLApi::Disable(capability);
}

void GLState::BindFrameBuffer(GLenum target, GLuint frameBuffer) {
    if (this->current_framebuffer_ != frameBuffer) {
        this->current_framebuffer_ = frameBuffer;
        OpenGLApi::BindFramebuffer(target, frameBuffer);
    } else {
        DEBUG_MLOGD("GLState BindFrameBuffer Duplicate %d", frameBuffer);
    }
}

void GLState::BindRenderBuffer(GLenum target, GLuint render_buffer) {
    if (this->current_render_buffer_ != render_buffer) {
        this->current_render_buffer_ = render_buffer;
        OpenGLApi::BindRenderbuffer(target, render_buffer);
    } else {
        DEBUG_MLOGD("GLState BindRenderBuffer Duplicate");
    }
}

void GLState::UseProgram(GLuint program) const {
    OpenGLApi::UseProgram(program);
}

void GLState::DepthFunc(GLenum func) {
    OpenGLApi::DepthFunc(func);
}

void GLState::DepthMask(GLboolean flag) {
    OpenGLApi::DepthMask(flag);
}

void GLState::Clear(GLbitfield mask) {
    OpenGLApi::Clear(mask);
}

void GLState::DepthRangef(GLfloat nearVal, GLfloat farVal) {
    OpenGLApi::DepthRangef(nearVal, farVal);
}

void GLState::StencilFunc(GLenum func, GLint ref, GLuint mask) {
    OpenGLApi::StencilFunc(func, ref, mask);
}

void GLState::StencilFuncSeparate(GLuint face, GLenum func, GLint ref, GLuint mask) {
    OpenGLApi::StencilFuncSeparate(face, func, ref, mask);
}

void GLState::StencilMask(GLuint mask) {
    OpenGLApi::StencilMask(mask);
}

void GLState::StencilMaskSeparate(GLuint face, GLuint mask) {
    OpenGLApi::StencilMaskSeparate(face, mask);
}

void GLState::StencilOp(GLenum sfail, GLenum dpfail, GLenum dppass) {
    OpenGLApi::StencilOp(sfail, dpfail, dppass);
}

void GLState::StencilOpSeparate(GLuint face, GLenum sfail, GLenum dpfail, GLenum dppass) {
    OpenGLApi::StencilOpSeparate(face, sfail, dpfail, dppass);
}

void GLState::ColorMask(GLboolean red,
                        GLboolean green,
                        GLboolean blue,
                        GLboolean alpha) {
    OpenGLApi::ColorMask(red, green, blue, alpha);
}

void GLState::CullFace(GLenum mode) {
    OpenGLApi::CullFace(mode);
}

void GLState::FrontFace(GLenum mode) {
    OpenGLApi::FrontFace(mode);
}

void GLState::BlendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    OpenGLApi::BlendColor(r, g, b, a);
}

void GLState::BlendFuncSeparate(GLenum srcRGB,
                       GLenum dstRGB,
                       GLenum srcAlpha,
                       GLenum dstAlpha) {
    OpenGLApi::BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void GLState::BlendEquationSeparate(GLenum modeRGB,
                           GLenum modeAlpha) {
    OpenGLApi::BlendEquationSeparate(modeRGB, modeAlpha);
}

// todo: 这几个方法应该放到state里么？
void GLState::PixelStorei(GLenum pname, GLint param) {
    
}

void GLState::ViewPort(int32_t x, int32_t y, int32_t w, int32_t h) {
    OpenGLApi::Viewport(x, y, w, h);
}

void GLState::ActiveTexture(GLenum texture) {
    OpenGLApi::ActiveTexture(texture);
}

void GLState::BindTexture(GLenum target, GLuint texture){
    OpenGLApi::BindTexture(target, texture);
}

void GLState::GetActiveTextureUnit() {
    
}

}
