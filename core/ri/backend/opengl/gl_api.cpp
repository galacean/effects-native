//
//  gl_api.cpp
//  shellTest
//
//  Created by Zongming Liu on 2021/11/10.
//

#include "gl_api.hpp"
#include "util/log_util.hpp"
#include "ri/gl.h"
#include "util/mars_thread.h"

// todo: 用宏切gl实现版本，避免写太多宏

namespace mn {

#ifdef INJECT_DEBUG_OBJECT

GLDebugObj* OpenGLApi::s_debug_obj_ = nullptr;

#endif

void OpenGLApi::ActiveTexture(uint32_t texture) {
    GL_CHECK(glActiveTexture(texture));
    DEBUG_MLOGD("----- ActiveTexture(0x%04X)", texture);
}

void OpenGLApi::AttachShader(uint32_t program, uint32_t shader) {
    GL_CHECK(glAttachShader(program, shader));
    DEBUG_MLOGD("----- AttachShader(%d,%d)", program, shader);
    DEBUG_AttachShader(program, shader);
}

void OpenGLApi::BindAttribLocation(uint32_t program, uint32_t index, const char *name) {
    GL_CHECK(glBindAttribLocation(program, index, name));
    DEBUG_MLOGD("----- BindAttribLocation(%d,%d,%s)", program, index, name);
}

void OpenGLApi::BindBuffer(uint32_t target, uint32_t buffer) {
    GL_CHECK(glBindBuffer(target, buffer));
    DEBUG_MLOGD("----- BindBuffer(0x%04X,%d)", target, buffer);
    DEBUG_BindBuffer(target, buffer);
}

void OpenGLApi::BindFramebuffer(uint32_t target, uint32_t framebuffer) {
    GL_CHECK(glBindFramebuffer(target, framebuffer));
    DEBUG_MLOGD("----- BindFramebuffer(0x%04X,%d)", target, framebuffer);
}

void OpenGLApi::BindRenderbuffer(uint32_t target, uint32_t renderbuffer) {
    GL_CHECK(glBindRenderbuffer(target, renderbuffer));
    DEBUG_MLOGD("----- BindRenderbuffer(0x%04X,%d)", target, renderbuffer);
}

void OpenGLApi::BindTexture(uint32_t target, uint32_t texture) {
    GL_CHECK(glBindTexture(target, texture));
    DEBUG_MLOGD("----- BindTexture(0x%04X,%d)", target, texture);
}

void OpenGLApi::BlendColor(float red, float green, float blue, float alpha) {
    GL_CHECK(glBlendColor(red, green, blue, alpha));
    DEBUG_MLOGD("----- BlendColor(%f,%f,%f,%f)", red, green, blue, alpha);
}

void OpenGLApi::BlendEquation(uint32_t mode) {
    GL_CHECK(glBlendEquation(mode));
    DEBUG_MLOGD("----- BlendEquation(0x%04X)", mode);
}

void OpenGLApi::BlendEquationSeparate(uint32_t mode_rgb, uint32_t mode_alpha) {
    GL_CHECK(glBlendEquationSeparate(mode_rgb, mode_alpha));
    DEBUG_MLOGD("----- BlendEquationSeparate(0x%04X,0x%04X)", mode_rgb, mode_alpha);
}

void OpenGLApi::BlendFunc(uint32_t sfactor, uint32_t dfactor) {
    GL_CHECK(glBlendFunc(sfactor, dfactor));
    DEBUG_MLOGD("----- BlendFunc(0x%04X,0x%04X)", sfactor, dfactor);
}

void
OpenGLApi::BlendFuncSeparate(uint32_t sfactorRGB,
                             uint32_t dfactorRGB,
                             uint32_t sfactorAlpha,
                             uint32_t dfactorAlpha) {
    GL_CHECK(glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha));
    DEBUG_MLOGD("----- BlendFuncSeparate(0x%04X,0x%04X,0x%04X,0x%04X)", sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
}

void OpenGLApi::BufferData(uint32_t target,
                           size_t size,
                           const void *data,
                           uint32_t usage) {
    GL_CHECK(glBufferData(target, size, data, usage));
    DEBUG_MLOGD("----- BufferData(0x%04X,%i,%p,0x%04X)", target, size, data, usage);
    DEBUG_BufferData(target, size, data, usage);
}

void OpenGLApi::BufferSubData(uint32_t target, intptr_t offset, size_t size, const void *data) {
    GL_CHECK(glBufferSubData(target, offset, size, data));
    DEBUG_MLOGD("----- BufferSubData %d %d %d %p", target, offset, size, data);
    DEBUG_BufferSubData(target, offset, size, data);
}

uint32_t OpenGLApi::CheckFramebufferStatus(uint32_t target) {
    GLenum ret;
    GL_CHECK(ret = glCheckFramebufferStatus(target));
    DEBUG_MLOGD("----- CheckFramebufferStatus 0x%04X = 0x%04X", target, ret);
    return ret;
}

void OpenGLApi::Clear(uint32_t mask) {
    if (MarsThread::GetThreadPaused()) {
        return;
    }
    GL_CHECK(glClear(mask));
    DEBUG_MLOGD("----- Clear(%d)", mask);
}

void OpenGLApi::ClearColor(float red, float green, float blue, float alpha) {
    GL_CHECK(glClearColor(red, green, blue, alpha));
    DEBUG_MLOGD("----- ClearColor(%f,%f,%f,%f)", red, green, blue, alpha);
}

void OpenGLApi::ClearDepthf(float d) {
    GL_CHECK(glClearDepthf(d));
    DEBUG_MLOGD("----- ClearDepthf(%f)", d);
}

void OpenGLApi::ClearStencil(int32_t s) {
    GL_CHECK(glClearStencil(s));
    DEBUG_MLOGD("----- ClearStencil(%d)", s);
}

void OpenGLApi::ColorMask(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    GL_CHECK(glColorMask(red, green, blue, alpha));
    DEBUG_MLOGD("----- ColorMask(%d,%d,%d,%d)", red, green, blue, alpha);
}

void OpenGLApi::CompileShader(uint32_t shader) {
    GL_CHECK(glCompileShader(shader));
    DEBUG_MLOGD("----- CompileShader %d", shader);
}

void OpenGLApi::CompressedTexImage2D(uint32_t target,
                                     int32_t level,
                                     uint32_t internalformat,
                                     int32_t width,
                                     int32_t height,
                                     int32_t border,
                                     int32_t imageSize,
                                     const void *data) {
    GL_CHECK(glCompressedTexImage2D(target,
                           level,
                           internalformat,
                           width,
                           height,
                           border,
                           imageSize,
                           data));
    DEBUG_MLOGD("----- CompressedTexImage2D %d %d", target, level);
}

void OpenGLApi::CompressedTexSubImage2D(uint32_t target,
                                        int32_t level,
                                        int32_t xoffset,
                                        int32_t yoffset,
                                        int32_t width,
                                        int32_t height,
                                        uint32_t format,
                                        int32_t imageSize,
                                        const void *data) {
    GL_CHECK(glCompressedTexSubImage2D(target,
                              level,
                              xoffset,
                              yoffset,
                              width,
                              height,
                              format,
                              imageSize,
                              data));
    DEBUG_MLOGD("----- CompressedTexSubImage2D %d %d", target, level);
}

void OpenGLApi::CopyTexImage2D(uint32_t target,
                               int32_t level,
                               uint32_t internalformat,
                               int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height,
                               int32_t border) {
    GL_CHECK(glCopyTexImage2D(target, level, internalformat, x, y, width, height, border));
    DEBUG_MLOGD("----- CopyTexImage2D %d %d", target, level);
}

void OpenGLApi::CopyTexSubImage2D(uint32_t target,
                                  int32_t level,
                                  int32_t xoffset,
                                  int32_t yoffset,
                                  int32_t x,
                                  int32_t y,
                                  int32_t width,
                                  int32_t height) {
    GL_CHECK(glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height));
    DEBUG_MLOGD("----- CopyTexSubImage2D %d %d", target, level);
}

uint32_t OpenGLApi::CreateBuffer() {
    uint32_t ret;
    GL_CHECK(glGenBuffers(1, &ret));
    DEBUG_MLOGD("----- CreateBuffer %d", ret);
    DEBUG_CreateBuffer(ret);
    return ret;
}

uint32_t OpenGLApi::CreateFramebuffer() {
    uint32_t ret;
    GL_CHECK(glGenFramebuffers(1, &ret));
    DEBUG_MLOGD("----- CreateFramebuffer=%d", ret);
    return ret;
}

uint32_t OpenGLApi::CreateProgram() {
    GLuint ret;
    GL_CHECK(ret = glCreateProgram());
    DEBUG_MLOGD("----- CreateProgram=%d", ret);
    DEBUG_CreateProgram(ret);
    return ret;
}

uint32_t OpenGLApi::CreateRenderbuffer() {
    uint32_t ret;
    GL_CHECK(glGenRenderbuffers(1, &ret));
    DEBUG_MLOGD("----- CreateRenderbuffer=%d", ret);
    return ret;
}

uint32_t OpenGLApi::CreateShader(uint32_t type) {
    GLuint ret;
    GL_CHECK(ret = glCreateShader(type));
    DEBUG_MLOGD("----- CreateShader(0x%04X)=%d", type, ret);
    return ret;
}

uint32_t OpenGLApi::CreateTexture() {
    uint32_t ret;
    GL_CHECK(glGenTextures(1, &ret));
    DEBUG_MLOGD("----- CreateTexture=%d", ret);
    return ret;
}

void OpenGLApi::CullFace(uint32_t mode) {
    GL_CHECK(glCullFace(mode));
    DEBUG_MLOGD("----- CullFace(0x%04X)", mode);
}

void OpenGLApi::DeleteBuffer(uint32_t buffer) {
    GL_CHECK(glDeleteBuffers(1, &buffer));
    DEBUG_MLOGD("----- DeleteBuffer(%d)", buffer);
}

void OpenGLApi::DeleteFramebuffer(uint32_t framebuffer) {
    GL_CHECK(glDeleteFramebuffers(1, &framebuffer));
    DEBUG_MLOGD("----- DeleteFramebuffer(%d)", framebuffer);
}

void OpenGLApi::DeleteProgram(uint32_t program) {
    GL_CHECK(glDeleteProgram(program));
    DEBUG_MLOGD("----- DeleteProgram(%d)", program);
}

void OpenGLApi::DeleteRenderbuffer(uint32_t renderbuffer) {
    GL_CHECK(glDeleteRenderbuffers(1, &renderbuffer));
    DEBUG_MLOGD("----- DeleteRenderbuffer(%d)", renderbuffer);
}

void OpenGLApi::DeleteShader(uint32_t shader) {
    GL_CHECK(glDeleteShader(shader));
    DEBUG_MLOGD("----- DeleteShader(%d)", shader);
}

void OpenGLApi::DeleteTexture(uint32_t texture) {
    GL_CHECK(glDeleteTextures(1, &texture));
    DEBUG_MLOGD("----- DeleteTexture(%d)", texture);
}

void OpenGLApi::DepthFunc(uint32_t func) {
    GL_CHECK(glDepthFunc(func));
    DEBUG_MLOGD("----- DepthFunc(0x%04X)", func);
}

void OpenGLApi::DepthMask(uint8_t flag) {
    GL_CHECK(glDepthMask(flag));
    DEBUG_MLOGD("----- DepthMask(0x%04X)", flag);
}

void OpenGLApi::DepthRangef(float near, float far) {
    GL_CHECK(glDepthRangef(near, far));
    DEBUG_MLOGD("----- DepthRangef(%f, %f)", near, far);
}

void OpenGLApi::DetachShader(uint32_t program, uint32_t shader) {
    GL_CHECK(glDetachShader(program, shader));
    DEBUG_MLOGD("----- DetachShader %ld, %ld", program, shader);
}

void OpenGLApi::Disable(uint32_t cap) {
    GL_CHECK(glDisable(cap));
    DEBUG_MLOGD("----- Disable(0x%04X)", cap);
    DEBUG_Disable(cap);
}

void OpenGLApi::DisableVertexAttribArray(uint32_t index) {
    GL_CHECK(glDisableVertexAttribArray(index));
    DEBUG_MLOGD("----- DisableVertexAttribArray(%d)", index);
    DEBUG_DisableVertexAttribArray(index);
}

void OpenGLApi::DrawArrays(uint32_t mode, int32_t first, int32_t count) {
    if (MarsThread::GetThreadPaused()) {
        return;
    }
    GL_CHECK(glDrawArrays(mode, first, count));
    DEBUG_MLOGD("----- DrawArrays(%d, %d, %d)", mode, first, count);
    DEBUG_DrawArrays(mode, first, count);
}

void OpenGLApi::DrawElements(uint32_t mode, int32_t count, uint32_t type, const void *indices) {
    if (MarsThread::GetThreadPaused()) {
        return;
    }
    DEBUG_MLOGD("----- DrawElements(0x%04X, %d, 0x%04X, %p)", mode, count, type, indices);
    GL_CHECK(glDrawElements(mode, count, type, indices));
    DEBUG_DrawElements(mode, count, type, indices);
}

void OpenGLApi::Enable(uint32_t cap) {
    GL_CHECK(glEnable(cap));
    DEBUG_MLOGD("----- Enable(0x%04X)", cap);
    DEBUG_Enable(cap);
}

void OpenGLApi::EnableVertexAttribArray(uint32_t index) {
    GL_CHECK(glEnableVertexAttribArray(index));
    DEBUG_MLOGD("----- EnableVertexAttribArray(%d)", index);
    DEBUG_EnableVertexAttribArray(index);
}

void OpenGLApi::Finish() {
    GL_CHECK(glFinish());
    DEBUG_MLOGD("----- Finish");
}

void OpenGLApi::Flush() {
    GL_CHECK(glFlush());
    DEBUG_MLOGD("----- Flush");
}

void OpenGLApi::FramebufferRenderbuffer(uint32_t target,
                                        uint32_t attachment,
                                        uint32_t renderbuffertarget,
                                        uint32_t renderbuffer) {
    GL_CHECK(glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer));
    DEBUG_MLOGD("----- FramebufferRenderbuffer 0x%04X 0x%04X 0x%04X 0x%04X", target, attachment, renderbuffertarget, renderbuffer);
}

void OpenGLApi::FramebufferTexture2D(uint32_t target,
                                     uint32_t attachment,
                                     uint32_t textarget,
                                     uint32_t texture,
                                     int32_t level) {
    GL_CHECK(glFramebufferTexture2D(target, attachment, textarget, texture, level));
    DEBUG_MLOGD("----- FramebufferTexture2D 0x%04X 0x%04X 0x%04X 0x%04X 0x%04X", target, attachment, textarget, texture, level);
}

void OpenGLApi::FrontFace(uint32_t mode) {
    GL_CHECK(glFrontFace(mode));
    DEBUG_MLOGD("----- FrontFace %d", mode);
}

void OpenGLApi::GenerateMipmap(uint32_t target) {
    GL_CHECK(glGenerateMipmap(target));
    DEBUG_MLOGD("----- GenerateMipmap %d", target);
}

void OpenGLApi::GetActiveAttrib(uint32_t program,
                                uint32_t index,
                                int32_t bufSize,
                                int32_t *length,
                                int32_t *size,
                                uint32_t *type,
                                char *name) {
    GL_CHECK(glGetActiveAttrib(program, index, bufSize, length, size, type, name));
    DEBUG_MLOGD("----- GetActiveAttrib program:%d index:%d name:%s type:0x%04X", program, index, name, *type);
    DEBUG_GetActiveAttrib(program, index, *size, *type, name);
}

void OpenGLApi::GetActiveUniform(uint32_t program,
                                 uint32_t index,
                                 int32_t bufSize,
                                 int32_t *length,
                                 int32_t *size,
                                 uint32_t *type,
                                 char *name) {
    GL_CHECK(glGetActiveUniform(program, index, bufSize, length, size, type, name));
    DEBUG_MLOGD("----- GetActiveUniform progrom:%d index:%d name:%s type:0x%04X", program, index, name, *type);
    DEBUG_GetActiveUniform(program, index, *size, *type, name);
}

void OpenGLApi::GetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders) {
    GL_CHECK(glGetAttachedShaders(program, maxCount, count, shaders));
    DEBUG_MLOGD("----- GetAttachedShaders %d %d %d %d", program, maxCount, *count, *shaders);
}


int32_t OpenGLApi::GetAttribLocation(uint32_t program, const char *name) {
    int32_t ret;
    GL_CHECK(ret = glGetAttribLocation(program, name));
    DEBUG_MLOGD("----- GetAttribLocation %d %s %d", program, name, ret);
    DEBUG_GetAttribLocation(program, name, ret);
    return ret;
}

void OpenGLApi::GetBooleanv(uint32_t pname, uint8_t *data) {
    GL_CHECK(glGetBooleanv(pname, data));
    DEBUG_MLOGD("----- GetBooleanv");
}

void OpenGLApi::GetBufferParameteriv(GLenum target, GLenum pname, GLint *params) {
    GL_CHECK(glGetBufferParameteriv(target, pname, params));
    DEBUG_MLOGD("----- GetBufferParameteriv");
}

uint32_t OpenGLApi::GetError() {
    GLenum err = glGetError();
    DEBUG_MLOGD("----- GetError %x", err);
    return err;
}

void OpenGLApi::GetFloatv(uint32_t pname, float *data) {
    GL_CHECK(glGetFloatv(pname, data));
    DEBUG_MLOGD("----- GetFloatv(0x%04X)", pname);
}

void OpenGLApi::GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params) {
    GL_CHECK(glGetFramebufferAttachmentParameteriv(target, attachment, pname, params));
    DEBUG_MLOGD("----- GetFramebufferAttachmentParameteriv");
}

void OpenGLApi::GetIntegerv(uint32_t pname, int32_t *data) {
    GL_CHECK(glGetIntegerv(pname, data));
    DEBUG_MLOGD("----- GetIntegerv(0x%04X)", pname);
}

std::string OpenGLApi::GetProgramInfoLog(uint32_t program) {
    DEBUG_MLOGD("----- GetProgramInfoLog");
    GLchar str[512 + 1];
    GLsizei len = 0;
    GL_CHECK(glGetProgramInfoLog(program, 512, &len, str));
    if (len > 512) {
        len = 512;
    }
    str[len] = 0;
    std::string ret = str;
    return ret;
}

void OpenGLApi::GetProgramiv(uint32_t program, uint32_t pname, int32_t *params) {
    GL_CHECK(glGetProgramiv(program, pname, params));
    DEBUG_MLOGD("----- glGetProgramiv 0x%04X %d", pname, *params);
    DEBUG_GetProgramParameter(program, pname, params);
}

void OpenGLApi::GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params) {
    GL_CHECK(glGetRenderbufferParameteriv(target, pname, params));
    DEBUG_MLOGD("----- GetRenderbufferParameteriv");
}

std::string OpenGLApi::GetShaderInfoLog(uint32_t shader) {
    DEBUG_MLOGD("----- GetShaderInfoLog");
    GLchar str[512 + 1];
    GLsizei len = 0;
    GL_CHECK(glGetShaderInfoLog(shader, 512, &len, str));
    if (len > 512) {
        len = 512;
    }
    str[len] = 0;
    std::string ret = str;
    return ret;
}

void OpenGLApi::GetShaderPrecisionFormat(GLenum shaderType,
                                         GLenum precisionType,
                                         GLint *range,
                                         GLint *precision) {
    GL_CHECK(glGetShaderPrecisionFormat(shaderType, precisionType, range, precision));
    DEBUG_MLOGD("----- GetShaderPrecisionFormat");
}

void OpenGLApi::GetShaderSource(uint32_t shader,
                                int32_t bufSize,
                                int32_t *length,
                                char *source) {
    GL_CHECK(glGetShaderSource(shader, bufSize, length, source));
    DEBUG_MLOGD("----- GetShaderSource");
}

void OpenGLApi::GetShaderiv(uint32_t shader, uint32_t pname, int32_t *params) {
    GL_CHECK(glGetShaderiv(shader, pname, params));
    DEBUG_MLOGD("----- GetShaderiv(%u,%u,%p)", shader, pname, params);
}

const uint8_t *OpenGLApi::GetString(uint32_t name) {
    DEBUG_MLOGD("----- GetString(0x%04X)", name);
    return glGetString(name);
}

float OpenGLApi::GetTexParameterfv(GLenum target, GLenum pname) {
    float ret;
    GL_CHECK(glGetTexParameterfv(target, pname, &ret));
    DEBUG_MLOGD("----- GetTexParameterfv");
    return ret;
}

int32_t OpenGLApi::GetTexParameteriv(GLenum target, GLenum pname) {
    int32_t ret;
    GL_CHECK(glGetTexParameteriv(target, pname, &ret));
    DEBUG_MLOGD("----- GetTexParameteriv");
    return ret;
}

void OpenGLApi::GetUniformfv(GLuint program, GLint location, GLfloat *params) {
    GL_CHECK(glGetUniformfv(program, location, params));
    DEBUG_MLOGD("----- GetUniformfv %d %d %f", program, location, params);
}

void OpenGLApi::GetUniformiv(GLuint program, GLint location, GLint *params) {
    GL_CHECK(glGetUniformiv(program, location, params));
    DEBUG_MLOGD("----- GetUniformiv %d %d %d", program, location, params);
}

int32_t OpenGLApi::GetUniformLocation(uint32_t program, const char *name) {
    int32_t location;
    GL_CHECK(location = glGetUniformLocation(program, name));
    DEBUG_MLOGD("----- GetUniformLocation program: %d %s %d", program, name, location);
    DEBUG_GetUniformLocation(program, name, location);
    return location;
}

void OpenGLApi::GetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params) {
    GL_CHECK(glGetVertexAttribfv(index, pname, params));
    DEBUG_MLOGD("----- GetVertexAttribfv %d %x %f", index, pname, *params);
}

void OpenGLApi::GetVertexAttribiv(GLuint index, GLenum pname, GLint *params) {
    GL_CHECK(glGetVertexAttribiv(index, pname, params));
    DEBUG_MLOGD("----- GetVertexAttribiv %d %x %d", index, pname, *params);
}

void OpenGLApi::GetVertexAttribPointerv(GLuint index, GLenum pname, void **pointer) {
    GL_CHECK(glGetVertexAttribPointerv(index, pname, pointer));
    DEBUG_MLOGD("----- GetVertexAttribPointerv 0x%04X 0x%04X %p", index, pname, pointer);
}


void OpenGLApi::Hint(uint32_t target, uint32_t mode) {
    GL_CHECK(glHint(target, mode));
    DEBUG_MLOGD("----- Hint(0x%04X,%d)", target, mode);
}

uint8_t OpenGLApi::IsBuffer(uint32_t buffer) {
    DEBUG_MLOGD("----- IsBuffer");
    return glIsBuffer(buffer);
}

uint8_t OpenGLApi::IsEnabled(uint32_t cap) {
    DEBUG_MLOGD("----- IsEnabled");
    return glIsEnabled(cap);
}

uint8_t OpenGLApi::IsFramebuffer(uint32_t framebuffer) {
    DEBUG_MLOGD("----- IsFramebuffer");
    return glIsFramebuffer(framebuffer);
}

uint8_t OpenGLApi::IsProgram(uint32_t program) {
    DEBUG_MLOGD("----- IsProgram");
    return glIsProgram(program);
}

uint8_t OpenGLApi::IsRenderbuffer(uint32_t renderbuffer) {
    DEBUG_MLOGD("----- IsRenderbuffer");
    return glIsRenderbuffer(renderbuffer);
}

uint8_t OpenGLApi::IsShader(uint32_t shader) {
    DEBUG_MLOGD("----- IsShader");
    return glIsShader(shader);
}

uint8_t OpenGLApi::IsTexture(uint32_t texture) {
    DEBUG_MLOGD("----- IsTexture");
    return glIsTexture(texture);
}

void OpenGLApi::LineWidth(float width) {
    DEBUG_MLOGD("----- LineWidth(%f)", width);
    GL_CHECK(glLineWidth(width));
}

void OpenGLApi::LinkProgram(uint32_t program) {
    GL_CHECK(glLinkProgram(program));
    DEBUG_MLOGD("----- LinkProgram(%d)", program);
}

void OpenGLApi::PixelStorei(uint32_t pname, int32_t param) {
    GL_CHECK(glPixelStorei(pname, param));
    DEBUG_MLOGD("----- PixelStorei(0x%04X, 0x%04X)", pname, param);
}

void OpenGLApi::PolygonOffset(float factor, float units) {
    GL_CHECK(glPolygonOffset(factor, units));
    DEBUG_MLOGD("----- PolygonOffset(%f, %f)", factor, units);
}

void OpenGLApi::ReadPixels(int32_t x,
                           int32_t y,
                           int32_t width,
                           int32_t height,
                           uint32_t format,
                           uint32_t type,
                           void *pixels) {
    GL_CHECK(glReadPixels(x, y, width, height, format, type, pixels));
    DEBUG_MLOGD("----- ReadPixels");
}

void OpenGLApi::RenderbufferStorage(uint32_t target,
                                    uint32_t internalformat,
                                    int width,
                                    int height) {
    GL_CHECK(glRenderbufferStorage(target, internalformat, width, height));
    DEBUG_MLOGD("----- RenderbufferStorage(0x%04X, 0x%04X, %d, %d)", target, internalformat, width, height);
}

void OpenGLApi::SampleCoverage(float value, uint8_t invert) {
    GL_CHECK(glSampleCoverage(value, invert));
    DEBUG_MLOGD("----- SampleCoverage:%f,%i", value, invert);
}

void OpenGLApi::Scissor(int32_t x, int32_t y, int32_t width, int32_t height) {
    GL_CHECK(glScissor(x, y, width, height));
    DEBUG_MLOGD("----- Scissor %d %d %d %d", x, y, width, height);
}

void OpenGLApi::ShaderSource(uint32_t shader, int32_t count, const char *const *string) {
    GL_CHECK(glShaderSource(shader, count, string, nullptr));
    DEBUG_MLOGD("----- ShaderSource %d %d %s", shader, count, *string);
    DEBUG_ShaderSource(shader, count, string);
}

void OpenGLApi::StencilFunc(uint32_t func, int32_t ref, uint32_t mask) {
    GL_CHECK(glStencilFunc(func, ref, mask));
    DEBUG_MLOGD("----- StencilFunc");
    DEBUG_StencilFunc(func, ref, mask);
}

void OpenGLApi::StencilFuncSeparate(uint32_t face, uint32_t func, int32_t ref, uint32_t mask) {
    GL_CHECK(glStencilFuncSeparate(face, func, ref, mask));
    DEBUG_MLOGD("----- StencilFuncSeparate");
    DEBUG_StencilFuncSeparate(face, func, ref, mask);
}

void OpenGLApi::StencilMask(uint32_t mask) {
    GL_CHECK(glStencilMask(mask));
    DEBUG_MLOGD("----- StencilMask(%i)", mask);
    DEBUG_StencilMask(mask);
}

void OpenGLApi::StencilMaskSeparate(uint32_t face, uint32_t mask) {
    GL_CHECK(glStencilMaskSeparate(face, mask));
    DEBUG_MLOGD("----- StencilMaskSeparate(%i,%i)", face, mask);
    DEBUG_StencilMaskSeparate(face, mask);
}

void OpenGLApi::StencilOp(uint32_t fail, uint32_t zfail, uint32_t zpass) {
    GL_CHECK(glStencilOp(fail, zfail, zpass));
    DEBUG_MLOGD("----- StencilOp(%i,%i,%i)", fail, zfail, zpass);
    DEBUG_StencilOp(fail, zfail, zpass);
}

void OpenGLApi::StencilOpSeparate(uint32_t face,
                                  uint32_t sfail,
                                  uint32_t dpfail,
                                  uint32_t dppass) {
    GL_CHECK(glStencilOpSeparate(face, sfail, dpfail, dppass));
    DEBUG_MLOGD("----- StencilOpSeparate(%i,%i,%i,%i)", face, sfail, dpfail, dppass);
    DEBUG_StencilOpSeparate(face, sfail, dpfail, dppass);
}

void OpenGLApi::TexImage2D(uint32_t target,
                           int32_t level,
                           int32_t internalformat,
                           int32_t width,
                           int32_t height,
                           int32_t border,
                           uint32_t format,
                           uint32_t type,
                           const void *pixels) {
    DEBUG_MLOGD("----- TexImage2D 0x%04X 0x%04X 0x%04X %d %d %d 0x%04X 0x%04X %p", target, level, internalformat, width, height, border, format, type, pixels);
    GL_CHECK(glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels));
}

void OpenGLApi::TexParameterf(uint32_t target, uint32_t pname, float param) {
    DEBUG_MLOGD("----- glTexParameterf");
    GL_CHECK(glTexParameterf(target, pname, param));
}

void OpenGLApi::TexParameteri(uint32_t target, uint32_t pname, int32_t param) {
    DEBUG_MLOGD("----- glTexParameteri %d %d %d", target, pname, param);
    GL_CHECK(glTexParameteri(target, pname, param));
}

void OpenGLApi::TexSubImage2D(uint32_t target,
                              int32_t level,
                              int32_t xoffset,
                              int32_t yoffset,
                              int32_t width,
                              int32_t height,
                              uint32_t format,
                              uint32_t type,
                              const void *pixels) {
    GL_CHECK(glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels));
    DEBUG_MLOGD("----- TexSubImage2D %d %d %d %d %d %d %d %d", target, level, xoffset, yoffset, width, height, format, type);
}

void OpenGLApi::Uniform1f(int32_t location, float v0) {
    GL_CHECK(glUniform1f(location, v0));
    DEBUG_MLOGD("----- Uniform1f location: %d value:%f", location, v0);
    DEBUG_Uniform1f(location, v0);
}

void OpenGLApi::Uniform1fv(int32_t location, int32_t count, const float *value) {
    GL_CHECK(glUniform1fv(location, count, value));
    DEBUG_MLOGD("----- Uniform1fv location: %d count: %d value: %f", location, count, value[0]);
    DEBUG_Uniform1fv(location, count, value);
}

void OpenGLApi::Uniform1i(int32_t location, int32_t v0) {
    GL_CHECK(glUniform1i(location, v0));
    DEBUG_MLOGD("----- Uniform1i location: %d value: %d", location, v0);
    DEBUG_Uniform1i(location, v0);
}

void OpenGLApi::Uniform1iv(int32_t location, int32_t count, const int32_t *value) {
    GL_CHECK(glUniform1iv(location, count, value));
    DEBUG_MLOGD("----- Uniform1iv location:%d count:%d value:%d", location, count, value[0]);
    DEBUG_Uniform1iv(location, count, value);
}

void OpenGLApi::Uniform2f(int32_t location, float v0, float v1) {
    GL_CHECK(glUniform2f(location, v0, v1));
    DEBUG_MLOGD("----- Uniform2f location: %d value: %f value:%f", location, v0, v1);
    DEBUG_Uniform2f(location, v0, v1);
}

void OpenGLApi::Uniform2fv(int32_t location, int32_t count, const float *value) {
    GL_CHECK(glUniform2fv(location, count, value));
    DEBUG_MLOGD("----- Uniform2fv location:%d count:%d value[0]:%f", location, count, value[0]);
    DEBUG_Uniform2fv(location, count, value);
}

void OpenGLApi::Uniform2i(int32_t location, int32_t v0, int32_t v1) {
    GL_CHECK(glUniform2i(location, v0, v1));
    DEBUG_MLOGD("----- Uniform2i location:%d value0:%d value1:%d", location, v0, v1);
    DEBUG_Uniform2i(location, v0, v1);
}

void OpenGLApi::Uniform2iv(int32_t location, int32_t count, const int32_t *value) {
    GL_CHECK(glUniform2iv(location, count, value));
    DEBUG_MLOGD("----- Uniform2iv location:%d count:%d value[0]:%d", location, count, value[0]);
    DEBUG_Uniform2iv(location, count, value);
}

void OpenGLApi::Uniform3f(int32_t location, float v0, float v1, float v2) {
    GL_CHECK(glUniform3f(location, v0, v1, v2));
    DEBUG_MLOGD("----- Uniform3f location:%d value0:%f value1:%f value2:%f", location, v0, v1, v2);
    DEBUG_Uniform3f(location, v0, v1, v2);
}

void OpenGLApi::Uniform3fv(int32_t location, int32_t count, const float *value) {
    GL_CHECK(glUniform3fv(location, count, value));
    DEBUG_MLOGD("----- Uniform3fv location:%d count:%d values:%f, %f, %f", location, count, value[0], value[1], value[2]);
    DEBUG_Uniform3fv(location, count, value);
}

void OpenGLApi::Uniform3i(int32_t location, int32_t v0, int32_t v1, int32_t v2) {
    GL_CHECK(glUniform3i(location, v0, v1, v2));
    DEBUG_MLOGD("----- Uniform3i location:%d value[0]:%d value[1]:%d value[2]:%d", location, v0, v1, v2);
    DEBUG_Uniform3i(location, v0, v1, v2);
}

void OpenGLApi::Uniform3iv(int32_t location, int32_t count, const int32_t *value) {
    GL_CHECK(glUniform3iv(location, count, value));
    DEBUG_MLOGD("----- Uniform3iv location:%d count:%d values:%d, %d, %d", location, count, value[0], value[1], value[2]);
    DEBUG_Uniform3iv(location, count, value);
}

void OpenGLApi::Uniform4f(int32_t location, float v0, float v1, float v2, float v3) {
    GL_CHECK(glUniform4f(location, v0, v1, v2, v3));
    DEBUG_MLOGD("----- Uniform4f location:%d value0:%f value1:%0.2f value2:%0.2f value3:%0.2f", location, v0, v1, v2, v3);
    DEBUG_Uniform4f(location, v0, v1, v2, v3);
}

void OpenGLApi::Uniform4fv(int32_t location, int32_t count, const float *value) {
    GL_CHECK(glUniform4fv(location, count, value));
    DEBUG_MLOGD("----- Uniform4fv location:%d count:%d values:%0.2f, %0.2f, %0.2f, %0.2f", location, count, value[0], value[1], value[2], value[3]);
    DEBUG_Uniform4fv(location, count, value);
}

void OpenGLApi::Uniform4i(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3) {
    GL_CHECK(glUniform4i(location, v0, v1, v2, v3));
    DEBUG_MLOGD("----- Uniform4i location:%d value0:%d value1:%d value2:%d value3:%d", location, v0, v1, v2, v3);
    DEBUG_Uniform4i(location, v0, v1, v2, v3);
}

void OpenGLApi::Uniform4iv(int32_t location, int32_t count, const int32_t *value) {
    GL_CHECK(glUniform4iv(location, count, value));
    DEBUG_MLOGD("----- Uniform4iv location:%d count:%d value[0]:%d", location, count, value[0]);
    DEBUG_Uniform4iv(location, count, value);
}

void OpenGLApi::UniformMatrix2fv(int32_t location,
                                 int32_t count,
                                 uint8_t transpose,
                                 const float *value) {
    GL_CHECK(glUniformMatrix2fv(location, count, transpose, value));
    DEBUG_MLOGD("----- UniformMatrix2fv %d %d %d %f", location, count, transpose, value[0]);
    DEBUG_UniformMatrix2fv(location, count, transpose, value);
}

void OpenGLApi::UniformMatrix3fv(int32_t location,
                                 int32_t count,
                                 uint8_t transpose,
                                 const float *value) {
    GL_CHECK(glUniformMatrix3fv(location, count, transpose, value));
    DEBUG_MLOGD("----- UniformMatrix3fv %d %d %d %f", location, count, transpose, value[0]);
    DEBUG_UniformMatrix3fv(location, count, transpose, value);
}

void OpenGLApi::UniformMatrix4fv(int32_t location,
                                 int32_t count,
                                 uint8_t transpose,
                                 const float *value) {
    GL_CHECK(glUniformMatrix4fv(location, count, transpose, value));
    DEBUG_MLOGD("----- UniformMatrix4fv location:%d count:%d transpose:%d value[0]:%f", location, count, transpose, value[0]);
    DEBUG_DATA("UniformMatrix4fv  ", (uint8_t *)value, 16 * sizeof(float) * count, sizeof(float));
    DEBUG_UniformMatrix4fv(location, count, transpose, value);
}

void OpenGLApi::UseProgram(uint32_t program) {
    GL_CHECK(glUseProgram(program));
    DEBUG_MLOGD("----- UseProgram %d", program);
    DEBUG_UseProgram(program);
}

void OpenGLApi::ValidateProgram(uint32_t program) {
    GL_CHECK(glValidateProgram(program));
    DEBUG_MLOGD("----- ValidateProgram");
}

void OpenGLApi::VertexAttrib1f(uint32_t index, float x) {
    GL_CHECK(glVertexAttrib1f(index, x));
    DEBUG_MLOGD("----- VertexAttrib1f");
}

void OpenGLApi::VertexAttrib1fv(uint32_t index, const float *v) {
    GL_CHECK(glVertexAttrib1fv(index, v));
    DEBUG_MLOGD("----- VertexAttrib1fv");
}

void OpenGLApi::VertexAttrib2f(uint32_t index, float x, float y) {
    GL_CHECK(glVertexAttrib2f(index, x, y));
    DEBUG_MLOGD("----- VertexAttrib2f");
}

void OpenGLApi::VertexAttrib2fv(uint32_t index, const float *v) {
    GL_CHECK(glVertexAttrib2fv(index, v));
    DEBUG_MLOGD("----- VertexAttrib2fv");
}

void OpenGLApi::VertexAttrib3f(uint32_t index, float x, float y, float z) {
    GL_CHECK(glVertexAttrib3f(index, x, y, z));
    DEBUG_MLOGD("----- VertexAttrib3f");
}

void OpenGLApi::VertexAttrib3fv(uint32_t index, const float *v) {
    GL_CHECK(glVertexAttrib3fv(index, v));
    DEBUG_MLOGD("----- VertexAttrib3fv(%u,%p)", index, v);
}

void OpenGLApi::VertexAttrib4f(uint32_t index, float x, float y, float z, float w) {
    GL_CHECK(glVertexAttrib4f(index, x, y, z, w));
    DEBUG_MLOGD("----- VertexAttrib4f(%u,%f,%f,%f,%f)", index, x, y, z, w);
}

void OpenGLApi::VertexAttrib4fv(uint32_t index, const float *v) {
    GL_CHECK(glVertexAttrib4fv(index, v));
    DEBUG_MLOGD("----- VertexAttrib4fv(%i,%p)", index, v);
}

void OpenGLApi::VertexAttribPointer(uint32_t index,
                                    int32_t size,
                                    uint32_t type,
                                    uint8_t normalized,
                                    int32_t stride,
                                    const void *pointer) {
    GL_CHECK(glVertexAttribPointer(index, size, type, normalized, stride, pointer));
    DEBUG_MLOGD("----- VertexAttribPointer(%d,%d,0x%04X,%d)", index, size, type, stride);
    DEBUG_VertexAttribPointer(index, size, type, normalized, stride, pointer);
}

void OpenGLApi::Viewport(int32_t x, int32_t y, int32_t width, int32_t height) {
    GL_CHECK(glViewport(x, y, width, height));
    DEBUG_MLOGD("----- Viewport(%d,%d,%d,%d)", x, y, width, height);
}


}
