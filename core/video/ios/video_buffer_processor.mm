//
//  video_buffer_processor.cpp
//  MarsNative
//
//  Created by changxing on 2023/10/7.
//  Copyright © 2023 Alipay. All rights reserved.
//

#include "video_buffer_processor.hpp"
#include "ri/backend/opengl/gl_api.hpp"
#include "util/log_util.hpp"
#include "util/mars_thread.h"
#import <OpenGLES/EAGL.h>

namespace mn {

// gles 2
static const GLchar* shader_frag_yuv_rg = (const GLchar*)"varying highp vec2 texCoordVarying;"
"precision mediump float;"
"uniform sampler2D SamplerY;"
"uniform sampler2D SamplerUV;"
"uniform mat3 colorConversionMatrix;"
"void main()"
"{"
"    mediump vec3 yuv;"
"    lowp vec3 rgb;"
"    yuv.x = (texture2D(SamplerY, texCoordVarying).r - (16.0/255.0));"
"    yuv.yz = (texture2D(SamplerUV, texCoordVarying).rg - vec2(0.5, 0.5));"
"    rgb = colorConversionMatrix * yuv;"
"    gl_FragColor = vec4(rgb, 1);"
"}";
static const GLchar* shader_frag_yuv_transparent_rg = (const GLchar*)"varying highp vec2 texCoordVarying;"
"precision mediump float;"
"uniform sampler2D SamplerY;"
"uniform sampler2D SamplerUV;"
"uniform mat3 colorConversionMatrix;"
"void main()"
"{"
"    mediump vec3 left_yuv;"
"    mediump vec3 right_yuv;"
"    lowp vec3 left_rgb;"
"    lowp vec3 right_rgb;"
"    vec2 left_texcoord  = texCoordVarying / vec2(2.0, 1.0);"
"    vec2 right_texcoord = texCoordVarying / vec2(2.0, 1.0) + vec2(0.5, 0.0);"
//   Subtract constants to map the video range start at 0
"    left_yuv.x = (texture2D(SamplerY, left_texcoord).r - (16.0/255.0));"
"    left_yuv.yz = (texture2D(SamplerUV, left_texcoord).rg - vec2(0.5, 0.5));"
"    left_rgb = colorConversionMatrix * left_yuv;"
"    right_yuv.x = (texture2D(SamplerY, right_texcoord).r - (16.0/255.0));"
"    right_yuv.yz = (texture2D(SamplerUV, right_texcoord).rg - vec2(0.5, 0.5));"
"    right_rgb = colorConversionMatrix * right_yuv;"
"    float right_alpha = 0.2126 * right_rgb.r + 0.7152 * right_rgb.g + 0.0722 * right_rgb.b;"
"    gl_FragColor = vec4(left_rgb, right_alpha);"
"}";

// gles 3
static const GLchar* shader_frag_yuv_ra = (const GLchar*)"varying highp vec2 texCoordVarying;"
"precision mediump float;"
"uniform sampler2D SamplerY;"
"uniform sampler2D SamplerUV;"
"uniform mat3 colorConversionMatrix;"
"void main()"
"{"
"    mediump vec3 yuv;"
"    lowp vec3 rgb;"
"    yuv.x = (texture2D(SamplerY, texCoordVarying).r - (16.0/255.0));"
"    yuv.yz = (texture2D(SamplerUV, texCoordVarying).ra - vec2(0.5, 0.5));"
"    rgb = colorConversionMatrix * yuv;"
"    gl_FragColor = vec4(rgb, 1);"
"}";
static const GLchar* shader_frag_yuv_transparent_ra = (const GLchar*)"varying highp vec2 texCoordVarying;"
"precision mediump float;"
"uniform sampler2D SamplerY;"
"uniform sampler2D SamplerUV;"
"uniform mat3 colorConversionMatrix;"
"void main()"
"{"
"    mediump vec3 left_yuv;"
"    mediump vec3 right_yuv;"
"    lowp vec3 left_rgb;"
"    lowp vec3 right_rgb;"
"    vec2 left_texcoord  = texCoordVarying / vec2(2.0, 1.0);"
"    vec2 right_texcoord = texCoordVarying / vec2(2.0, 1.0) + vec2(0.5, 0.0);"
//   Subtract constants to map the video range start at 0
"    left_yuv.x = (texture2D(SamplerY, left_texcoord).r - (16.0/255.0));"
"    left_yuv.yz = (texture2D(SamplerUV, left_texcoord).ra - vec2(0.5, 0.5));"
"    left_rgb = colorConversionMatrix * left_yuv;"
"    right_yuv.x = (texture2D(SamplerY, right_texcoord).r - (16.0/255.0));"
"    right_yuv.yz = (texture2D(SamplerUV, right_texcoord).ra - vec2(0.5, 0.5));"
"    right_rgb = colorConversionMatrix * right_yuv;"
"    float right_alpha = 0.2126 * right_rgb.r + 0.7152 * right_rgb.g + 0.0722 * right_rgb.b;"
"    gl_FragColor = vec4(left_rgb, right_alpha);"
"}";

static const GLchar* shader_vertex_yuv = (const GLchar*)"attribute vec4 position;"
"attribute vec2 texCoord;"
"varying vec2 texCoordVarying;"
"void main()"
"{"
"    gl_Position = position;"
"    texCoordVarying = texCoord;"
"}";

// BT.601, which is the standard for SDTV.
static const GLfloat kYUVColorConversion601[] = {
    1.164,  1.164, 1.164,
    0.0, -0.392, 2.017,
    1.596, -0.813,   0.0,
};

// BT.709, which is the standard for HDTV.
static const GLfloat kYUVColorConversion709[] = {
    1.164,  1.164, 1.164,
    0.0, -0.213, 2.112,
    1.793, -0.533,   0.0,
};

static int CreateShader(GLenum type, const GLchar* content) {
    int shader = OpenGLApi::CreateShader(type);
    OpenGLApi::ShaderSource(shader, 1, &content);
    OpenGLApi::CompileShader(shader);
    GLint status = 0;
    OpenGLApi::GetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        MLOGD("VideoBufferProcessor CreateShader fail %x %x", type, status);
        MLOGD("%s", OpenGLApi::GetShaderInfoLog(shader).c_str());
        OpenGLApi::DeleteShader(shader);
        return -1;
    }
    return shader;
}

VideoBufferProcessor::VideoBufferProcessor(bool transparent) {
    MLOGD("VideoBufferProcessor %p", this);
    transparent_ = transparent;
}

VideoBufferProcessor::~VideoBufferProcessor() {
    MLOGD("~VideoBufferProcessor %p", this);
    if (cv_texture_cache_) {
        CFRelease(cv_texture_cache_);
        cv_texture_cache_ = nullptr;
    }
    if (pixel_buffer_) {
        CVPixelBufferRelease(pixel_buffer_);
        pixel_buffer_ = nullptr;
    }
}

void VideoBufferProcessor::ReleaseGLObject() {
    if (program_ != -1) {
        OpenGLApi::DeleteProgram(program_);
    }
    if (v_shader_ != -1) {
        OpenGLApi::DeleteShader(v_shader_);
    }
    if (f_shader_ != -1) {
        OpenGLApi::DeleteShader(f_shader_);
    }
    if (gl_buffer_) {
        OpenGLApi::DeleteBuffer(gl_buffer_);
    }
}

// 画到video texture的fbo上
bool VideoBufferProcessor::RenderToFBO(int frame_buffer) {
    CVPixelBufferRef pb;
    { // 尝试获取CVPixelBufferRef
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        pb = pixel_buffer_;
        if (!pb) {
            // todo: 没有来第一帧怎么办
            return true;
        }
        CVPixelBufferRetain(pb);
    }
    
    if (!setup_) {
        // 初始化gl
        Setup();
    }
    if (program_ == -1) {
        return false;
    }
    // 初始化fbo
    int current_fb = 0;
    OpenGLApi::GetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fb);
    OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    OpenGLApi::ClearColor(0, 0, 0, 0);
    OpenGLApi::Clear(GL_COLOR_BUFFER_BIT);
    OpenGLApi::UseProgram(program_);
    // 锁定buffer
    CVPixelBufferLockBaseAddress(pb, 0);
    size_t planeCount = CVPixelBufferGetPlaneCount(pb);
    if (planeCount != 2) {
        MLOGE("VideoBufferProcessor::RenderToFBO invalid planeCount %ld", planeCount);
        return false;
    }
    if (!conversion_) { // 判断使用哪种颜色转换矩阵
        CFTypeRef colorAttachments = CVBufferGetAttachment(pb, kCVImageBufferYCbCrMatrixKey, NULL);
        if (colorAttachments && CFStringCompare((CFStringRef)colorAttachments, kCVImageBufferYCbCrMatrix_ITU_R_601_4, 0) == kCFCompareEqualTo) {
            conversion_ = kYUVColorConversion601;
        } else {
            conversion_ = kYUVColorConversion709;
        }
        OpenGLApi::UniformMatrix3fv(u_matrix_, 1, GL_FALSE, conversion_);
    }
    CVReturn err;
    if (!cv_texture_cache_) { // 创建CVOpenGLESTextureCache
        err = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, [EAGLContext currentContext], NULL, &cv_texture_cache_);
        if (err != noErr) {
            MLOGE("VideoBufferProcessor::RenderToFBO Error at CVOpenGLESTextureCacheCreate %d", err);
            return false;
        }
    }
    // 获取视频宽高
    size_t width = CVPixelBufferGetWidth(pb);
    size_t height = CVPixelBufferGetHeight(pb);
    // 获取y纹理
    OpenGLApi::ActiveTexture(GL_TEXTURE0);
    GLint format = ([EAGLContext currentContext].API == kEAGLRenderingAPIOpenGLES3 ? GL_LUMINANCE : GL_RED_EXT);
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       cv_texture_cache_,
                                                       pb,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       format,     // y 使用RED component
                                                       (int) width,
                                                       (int) height,
                                                       format,
                                                       GL_UNSIGNED_BYTE,
                                                       0,
                                                       &y_tex_);
    if (err || CVOpenGLESTextureGetName(y_tex_) < 1) {
        MLOGE("VideoBufferProcessor::RenderToFBO Error at CVOpenGLESTextureCacheCreateTextureFromImage %d", err);
        return false;
    }
    OpenGLApi::BindTexture(CVOpenGLESTextureGetTarget(y_tex_), CVOpenGLESTextureGetName(y_tex_));
    OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    OpenGLApi::TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OpenGLApi::TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // 获取uv纹理
    GLint format2 = (format == GL_LUMINANCE ? GL_LUMINANCE_ALPHA : GL_RG_EXT);
    OpenGLApi::ActiveTexture(GL_TEXTURE1);
    err = CVOpenGLESTextureCacheCreateTextureFromImage(kCFAllocatorDefault,
                                                       cv_texture_cache_,
                                                       pb,
                                                       NULL,
                                                       GL_TEXTURE_2D,
                                                       format2,    // uv 使用R格式
                                                       (int) width / 2,
                                                       (int) height / 2,
                                                       format2,
                                                       GL_UNSIGNED_BYTE,
                                                       1,
                                                       &uv_tex_);
    // 解除锁定buffer
    CVPixelBufferUnlockBaseAddress(pb, 0);
    if (err || CVOpenGLESTextureGetName(uv_tex_) < 1) {
        MLOGE("VideoBufferProcessor::RenderToFBO Error at CVOpenGLESTextureCacheCreateTextureFromImage2 %d", err);
        return false;
    }
    if (!MarsThread::GetThreadPaused()) { // 在后台时不渲染视频，规避闪退：2698863439
        OpenGLApi::BindTexture(CVOpenGLESTextureGetTarget(uv_tex_), CVOpenGLESTextureGetName(uv_tex_));
        OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        OpenGLApi::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        OpenGLApi::TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        OpenGLApi::TexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //开始渲染
        OpenGLApi::Viewport(0, 0, (int) width, (int) height);
        OpenGLApi::BindBuffer(GL_ARRAY_BUFFER, gl_buffer_);
        OpenGLApi::EnableVertexAttribArray(attr_pos_);
        OpenGLApi::VertexAttribPointer(attr_pos_, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
        OpenGLApi::EnableVertexAttribArray(attr_uv_);
        OpenGLApi::VertexAttribPointer(attr_uv_, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (8 * sizeof(GLfloat)));
        OpenGLApi::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        OpenGLApi::DisableVertexAttribArray(attr_pos_);
        OpenGLApi::DisableVertexAttribArray(attr_uv_);
        OpenGLApi::Flush();
    }
    // 提交并释放缓存
    CVOpenGLESTextureCacheFlush(cv_texture_cache_, 0);
    if (y_tex_) {
        CFRelease(y_tex_);
        y_tex_ = nullptr;
    }
    if (uv_tex_) {
        CFRelease(uv_tex_);
        uv_tex_ = nullptr;
    }
    // 恢复fbo
    OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, current_fb);
    if (glGetError()) assert(0);
    // 释放buffer
    CVPixelBufferRelease(pb);
    return true;
}

void VideoBufferProcessor::Setup() {
    setup_ = true;
    // 创建shader
    v_shader_ = CreateShader(GL_VERTEX_SHADER, shader_vertex_yuv);
    if ([EAGLContext currentContext].API == kEAGLRenderingAPIOpenGLES3) {
        f_shader_ = CreateShader(
                GL_FRAGMENT_SHADER,
                transparent_ ? shader_frag_yuv_transparent_ra : shader_frag_yuv_ra
        );
    } else {
        f_shader_ = CreateShader(
                GL_FRAGMENT_SHADER,
                transparent_ ? shader_frag_yuv_transparent_rg : shader_frag_yuv_rg
        );
    }
    if (v_shader_ == -1 || f_shader_ == -1) {
        return;
    }
    // 创建program
    program_ = OpenGLApi::CreateProgram();
    OpenGLApi::AttachShader(program_, v_shader_);
    OpenGLApi::AttachShader(program_, f_shader_);
    OpenGLApi::LinkProgram(program_);
    GLint status;
    OpenGLApi::GetProgramiv(program_, GL_LINK_STATUS, &status);
    if (status == 0) {
        // shader编译失败
        OpenGLApi::DeleteShader(v_shader_);
        OpenGLApi::DeleteShader(f_shader_);
        OpenGLApi::DeleteProgram(program_);
        v_shader_ = -1;
        f_shader_ = -1;
        program_ = -1;
        MLOGE("VideoBufferProcessor create program fail");
        return;
    }
    // 初始化buffer
    gl_buffer_ = OpenGLApi::CreateBuffer();
    static GLfloat texVertices[] = {
        -1, -1,
        1,  -1,
        -1, 1,
        1,  1,
        0, 1,
        1, 1,
        0, 0,
        1, 0,
    };
    OpenGLApi::BindBuffer(GL_ARRAY_BUFFER, gl_buffer_);
    OpenGLApi::BufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), texVertices, GL_STATIC_DRAW);
    // 获取shader绑定
    OpenGLApi::UseProgram(program_);
    attr_pos_ = OpenGLApi::GetAttribLocation(program_, "position");
    attr_uv_ = OpenGLApi::GetAttribLocation(program_, "texCoord");
    u_sample_y_ = OpenGLApi::GetUniformLocation(program_, "SamplerY");
    u_sample_uv_ = OpenGLApi::GetUniformLocation(program_, "SamplerUV");
    u_matrix_ = OpenGLApi::GetUniformLocation(program_, "colorConversionMatrix");
    
    OpenGLApi::Uniform1i(u_sample_y_, 0);        //  texture Unit0
    OpenGLApi::Uniform1i(u_sample_uv_, 1);       //  texture Unit1
}

void VideoBufferProcessor::OnSurfaceDataInternal(void* data, int64_t intValue) {
    CVPixelBufferRef buffer = (CVPixelBufferRef)data;
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    if (pixel_buffer_ != buffer) {
        if (pixel_buffer_) {
            CVPixelBufferRelease(pixel_buffer_);
        }
        pixel_buffer_ = buffer;
        CVPixelBufferRetain(pixel_buffer_);
    }
}

long VideoBufferProcessor::OnSurfaceData(int option, void* view, void* data, int64_t intValue) {
    if (!view || !data) {
        MLOGE("Video OnSurfaceData invalid %d %p %p %lld", option, view, data, intValue);
        return 0;
    }
//        MLOGD("Video OnSurfaceData %d %p %p %lld", option, view, data, intValue);
    if (option == 6) { // ME_ONE_FRAME_RENDERED
        ((VideoBufferProcessor*) view)->OnSurfaceDataInternal(data, intValue);
    }
    return 0;
}

}
