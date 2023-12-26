//
//  gl_api.hpp
//
//  Created by Zongming Liu on 2021/11/10.
//

#ifndef gl_api_hpp
#define gl_api_hpp

#include <stdio.h>
#include <string>
#include "ri/gl.h"
#include "ri/gl_api_debugger.h"

namespace mn {

struct OpenGLApi {
    
    static void ActiveTexture(uint32_t texture);

    static void AttachShader(uint32_t program, uint32_t shader);

    static void BindAttribLocation(uint32_t program, uint32_t index, const char *name);

    static void BindBuffer(uint32_t target, uint32_t buffer);

    static void BindFramebuffer(uint32_t target, uint32_t framebuffer);

    static void BindRenderbuffer(uint32_t target, uint32_t renderbuffer);

    static void BindTexture(uint32_t target, uint32_t texture);

    static void BlendColor(float red, float green, float blue, float alpha);

    static void BlendEquation(uint32_t mode);

    static void BlendEquationSeparate(uint32_t mode_rgb, uint32_t mode_alpha);

    static void BlendFunc(uint32_t sfactor, uint32_t dfactor);

    static void BlendFuncSeparate(uint32_t sfactorRGB,
                                  uint32_t dfactorRGB,
                                  uint32_t sfactorAlpha,
                                  uint32_t dfactorAlpha);

    static void BufferData(uint32_t target, size_t size, const void *data, uint32_t usage);

    static void BufferSubData(uint32_t target, intptr_t offset, size_t size, const void *data);

    static uint32_t CheckFramebufferStatus(uint32_t target);

    static void Clear(uint32_t mask);

    static void ClearColor(float red, float green, float blue, float alpha);

    static void ClearDepthf(float d);

    static void ClearStencil(int32_t s);

    static void ColorMask(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

    static void CompileShader(uint32_t shader);

    static void CompressedTexImage2D(uint32_t target,
                                     int32_t level,
                                     uint32_t internalformat,
                                     int32_t width,
                                     int32_t height,
                                     int32_t border,
                                     int32_t imageSize,
                                     const void *data);

    static void CompressedTexSubImage2D(uint32_t target,
                                        int32_t level,
                                        int32_t xoffset,
                                        int32_t yoffset,
                                        int32_t width,
                                        int32_t height,
                                        uint32_t format,
                                        int32_t imageSize,
                                        const void *data);

    static void CopyTexImage2D(uint32_t target,
                               int32_t level,
                               uint32_t internalformat,
                               int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height,
                               int32_t border);

    static void CopyTexSubImage2D(uint32_t target,
                                  int32_t level,
                                  int32_t xoffset,
                                  int32_t yoffset,
                                  int32_t x,
                                  int32_t y,
                                  int32_t width,
                                  int32_t height);

    static uint32_t CreateBuffer();

    static uint32_t CreateFramebuffer();

    static uint32_t CreateProgram();

    static uint32_t CreateRenderbuffer();

    static uint32_t CreateShader(uint32_t type);

    static uint32_t CreateTexture();

    static void CullFace(uint32_t mode);

    static void DeleteBuffer(uint32_t buffer);

    static void DeleteFramebuffer(uint32_t framebuffer);

    static void DeleteProgram(uint32_t program);

    static void DeleteRenderbuffer(uint32_t renderbuffer);

    static void DeleteShader(uint32_t shader);

    static void DeleteTexture(uint32_t texture);

    static void DepthFunc(uint32_t func);

    static void DepthMask(uint8_t flag);

    static void DepthRangef(float near, float far);

    static void DetachShader(uint32_t program, uint32_t shader);

    static void Disable(uint32_t cap);

    static void DisableVertexAttribArray(uint32_t index);

    static void DrawArrays(uint32_t mode, int32_t first, int32_t count);

    static void DrawElements(uint32_t mode, int32_t count, uint32_t type, const void *indices);

    static void Enable(uint32_t cap);

    static void EnableVertexAttribArray(uint32_t index);

    static void Finish();

    static void Flush();

    static void FramebufferRenderbuffer(uint32_t target,
                                        uint32_t attachment,
                                        uint32_t renderbuffertarget,
                                        uint32_t renderbuffer);

    static void FramebufferTexture2D(uint32_t target,
                                     uint32_t attachment,
                                     uint32_t textarget,
                                     uint32_t texture,
                                     int32_t level);

    static void FrontFace(uint32_t mode);

    static void GenerateMipmap(uint32_t target);

    static void GetActiveAttrib(uint32_t program,
                                uint32_t index,
                                int32_t bufSize,
                                int32_t *length,
                                int32_t *size,
                                uint32_t *type,
                                char *name);

    static void GetActiveUniform(uint32_t program,
                                 uint32_t index,
                                 int32_t bufSize,
                                 int32_t *length,
                                 int32_t *size,
                                 uint32_t *type,
                                 char *name);

    static void GetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);

    static int32_t GetAttribLocation(uint32_t program, const char *name);

    static void GetBooleanv(uint32_t pname, uint8_t *data);

    static void GetBufferParameteriv(GLenum target, GLenum pname, GLint *params);

    static uint32_t GetError();

    static void GetFloatv(uint32_t pname, float *data);

    static void GetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params);

    static void GetIntegerv(uint32_t pname, int32_t *data);

    static std::string GetProgramInfoLog(uint32_t program);

    static void GetProgramiv(uint32_t program, uint32_t pname, int32_t *params);

    static void GetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params);

    static std::string GetShaderInfoLog(uint32_t shader);

    static void GetShaderPrecisionFormat(GLenum shaderType,
                                         GLenum precisionType,
                                         GLint *range,
                                         GLint *precision);

    static void GetShaderSource(uint32_t shader,
                                int32_t bufSize,
                                int32_t *length,
                                char *source);

    static void GetShaderiv(uint32_t shader, uint32_t pname, int32_t *params);

    static const uint8_t *GetString(uint32_t name);

    static float GetTexParameterfv(GLenum target, GLenum pname);

    static int32_t GetTexParameteriv(GLenum target, GLenum pname);

    static void GetUniformfv(GLuint program, GLint location, GLfloat *params);

    static void GetUniformiv(GLuint program, GLint location, GLint *params);

    static int32_t GetUniformLocation(uint32_t program, const char *name);

    static void GetVertexAttribfv(GLuint index, GLenum pname, GLfloat *params);

    static void GetVertexAttribiv(GLuint index, GLenum pname, GLint *params);

    static void GetVertexAttribPointerv(GLuint index, GLenum pname, void **pointer);

    static void Hint(uint32_t target, uint32_t mode);

    static uint8_t IsBuffer(uint32_t buffer);

    static uint8_t IsEnabled(uint32_t cap);

    static uint8_t IsFramebuffer(uint32_t framebuffer);

    static uint8_t IsProgram(uint32_t program);

    static uint8_t IsRenderbuffer(uint32_t renderbuffer);

    static uint8_t IsShader(uint32_t shader);

    static uint8_t IsTexture(uint32_t texture);

    static void LineWidth(float width);

    static void LinkProgram(uint32_t program);

    static void PixelStorei(uint32_t pname, int32_t param);

    static void PolygonOffset(float factor, float units);

    static void ReadPixels(int32_t x,
                           int32_t y,
                           int32_t width,
                           int32_t height,
                           uint32_t format,
                           uint32_t type,
                           void *pixels);

    static void RenderbufferStorage(uint32_t target,
                                    uint32_t internalformat,
                                    int width,
                                    int height);

    static void SampleCoverage(float value, uint8_t invert);

    static void Scissor(int32_t x, int32_t y, int32_t width, int32_t height);

    static void ShaderSource(uint32_t shader, int32_t count, const char *const *string);

    static void StencilFunc(uint32_t func, int32_t ref, uint32_t mask);

    static void StencilFuncSeparate(uint32_t face, uint32_t func, int32_t ref, uint32_t mask);

    static void StencilMask(uint32_t mask);

    static void StencilMaskSeparate(uint32_t face, uint32_t mask);

    static void StencilOp(uint32_t fail, uint32_t zfail, uint32_t zpass);

    static void StencilOpSeparate(uint32_t face,
                                  uint32_t sfail,
                                  uint32_t dpfail,
                                  uint32_t dppass);

    static void TexImage2D(uint32_t target,
                           int32_t level,
                           int32_t internalformat,
                           int32_t width,
                           int32_t height,
                           int32_t border,
                           uint32_t format,
                           uint32_t type,
                           const void *pixels);

    static void TexParameterf(uint32_t target, uint32_t pname, float param);

    static void TexParameteri(uint32_t target, uint32_t pname, int32_t param);

    static void TexSubImage2D(uint32_t target,
                              int32_t level,
                              int32_t xoffset,
                              int32_t yoffset,
                              int32_t width,
                              int32_t height,
                              uint32_t format,
                              uint32_t type,
                              const void *pixels);

    static void Uniform1f(int32_t location, float v0);

    static void Uniform1fv(int32_t location, int32_t count, const float *value);

    static void Uniform1i(int32_t location, int32_t v0);

    static void Uniform1iv(int32_t location, int32_t count, const int32_t *value);

    static void Uniform2f(int32_t location, float v0, float v1);

    static void Uniform2fv(int32_t location, int32_t count, const float *value);

    static void Uniform2i(int32_t location, int32_t v0, int32_t v1);

    static void Uniform2iv(int32_t location, int32_t count, const int32_t *value);

    static void Uniform3f(int32_t location, float v0, float v1, float v2);

    static void Uniform3fv(int32_t location, int32_t count, const float *value);

    static void Uniform3i(int32_t location, int32_t v0, int32_t v1, int32_t v2);

    static void Uniform3iv(int32_t location, int32_t count, const int32_t *value);

    static void Uniform4f(int32_t location, float v0, float v1, float v2, float v3);

    static void Uniform4fv(int32_t location, int32_t count, const float *value);

    static void Uniform4i(int32_t location, int32_t v0, int32_t v1, int32_t v2, int32_t v3);

    static void Uniform4iv(int32_t location, int32_t count, const int32_t *value);

    static void UniformMatrix2fv(int32_t location,
                                 int32_t count,
                                 uint8_t transpose,
                                 const float *value);

    static void UniformMatrix3fv(int32_t location,
                                 int32_t count,
                                 uint8_t transpose,
                                 const float *value);

    static void UniformMatrix4fv(int32_t location,
                                 int32_t count,
                                 uint8_t transpose,
                                 const float *value);

    static void UseProgram(uint32_t program);

    static void ValidateProgram(uint32_t program);

    static void VertexAttrib1f(uint32_t index, float x);

    static void VertexAttrib1fv(uint32_t index, const float *v);

    static void VertexAttrib2f(uint32_t index, float x, float y);

    static void VertexAttrib2fv(uint32_t index, const float *v);

    static void VertexAttrib3f(uint32_t index, float x, float y, float z);

    static void VertexAttrib3fv(uint32_t index, const float *v);

    static void VertexAttrib4f(uint32_t index, float x, float y, float z, float w);

    static void VertexAttrib4fv(uint32_t index, const float *v);

    static void VertexAttribPointer(uint32_t index,
                                    int32_t size,
                                    uint32_t type,
                                    uint8_t normalized,
                                    int32_t stride,
                                    const void *pointer);

    static void Viewport(int32_t x, int32_t y, int32_t width, int32_t height);

#ifdef INJECT_DEBUG_OBJECT

    static GLDebugObj* s_debug_obj_;

#endif
    
};

}

#endif /* gl_api_hpp */
