//
// Created by changxing on 2023/10/7.
//

#include "video_buffer_processor.h"
#include "ri/backend/opengl/gl_api.hpp"
#include "util/log_util.hpp"
#include "platform/android/jni_bridge.h"

namespace mn {

static const GLchar* shader_frag = (const GLchar*)"#extension GL_OES_EGL_image_external : require\n"
    "varying highp vec2 texCoordVarying;"
    "precision mediump float;"
    "uniform samplerExternalOES Sampler0;"
    "void main()"
    "{"
    "    gl_FragColor = texture2D(Sampler0, texCoordVarying);"
    "}";

static const GLchar* shader_frag_transparent = (const GLchar*)"#extension GL_OES_EGL_image_external : require\n"
    "varying highp vec2 texCoordVarying;"
    "precision mediump float;"
    "uniform samplerExternalOES Sampler0;"
    "void main()"
    "{"
    "    vec2 left_texcoord  = texCoordVarying / vec2(2.0, 1.0);"
    "    vec2 right_texcoord = texCoordVarying / vec2(2.0, 1.0) + vec2(0.5, 0.0);"
    "    gl_FragColor = vec4(texture2D(Sampler0, left_texcoord).rgb, texture2D(Sampler0, right_texcoord).r);"
    "}";

static const GLchar* shader_vertex = (const GLchar*)"attribute vec4 position;"
    "attribute vec2 texCoord;"
    "varying vec2 texCoordVarying;"
    "void main()"
    "{"
    "    gl_Position = position;"
    "    texCoordVarying = texCoord;"
    "}";

static int CreateShader(GLenum type, const GLchar* content) {
    int shader = OpenGLApi::CreateShader(type);
    OpenGLApi::ShaderSource(shader, 1, &content);
    OpenGLApi::CompileShader(shader);
    GLint status = 0;
    OpenGLApi::GetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        std::string log = OpenGLApi::GetShaderInfoLog(shader);
        OpenGLApi::DeleteShader(shader);
        MLOGE("VideoBufferProcessor CreateShader fail %x %x", type, status);
        MLOGE("%s", log.c_str());
        return -1;
    }
    return shader;
}

static jclass s_global_clazz_ = nullptr;

VideoBufferProcessor::VideoBufferProcessor(bool transparent) {
    MLOGD("VideoBufferProcessor %p", this);
    transparent_ = transparent;
}

VideoBufferProcessor::~VideoBufferProcessor() {
    MLOGD("~VideoBufferProcessor %p", this);
    ReleaseSurface();
}

bool VideoBufferProcessor::RenderToFBO(int frame_buffer) {
    // 检查nativewindow和surface
    if (!native_window_ || !surface_texture_ref_) {
        MLOGE("VideoBufferProcessor RenderToFBO invalid %p %p", native_window_, surface_texture_ref_);
        return false;
    }
    // 触发updateTexture
    JNIEnv *env = JNIUtil::GetEnv();
    jclass clazz = JNIUtil::GetVideoSurfaceUtilClass();
    if (!clazz) {
        MLOGE("Video RenderToFBO no clazz");
        return false;
    }
    static jmethodID mid = env->GetStaticMethodID(clazz, "updateTexture", "(Ljava/lang/Object;)I");
    if (!mid) {
        MLOGE("Video RenderToFBO no mid");
        return false;
    }
    jint result = env->CallStaticIntMethod(clazz, mid, surface_texture_ref_);
    if (result == 0) {
        MLOGE("Video RenderToFBO updateTexture fail");
        return false;
    }
    if (result == 2) {
        // 无更新
        return true;
    }
    if (program_ < 0) { // gl环境异常
        return false;
    }
    // 记录旧的fbo
    int fbo = 0;
    OpenGLApi::GetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
    // 初始化渲染环境
    OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    OpenGLApi::ClearColor(0, 0, 0, 0);
    OpenGLApi::Clear(GL_COLOR_BUFFER_BIT);
    OpenGLApi::Viewport(0, 0, width_, height_);
    OpenGLApi::UseProgram(program_);
    if (!setup_) { // 创建相关gl对象
        Setup();
        if (program_ < 0) {
            MLOGE("VideoBufferProcessor Setup fail");
            return false;
        }
    }
    // 绘制到fbo
    OpenGLApi::ActiveTexture(GL_TEXTURE0);
    OpenGLApi::BindTexture(GL_TEXTURE_EXTERNAL_OES, video_texture_);
    OpenGLApi::BindBuffer(GL_ARRAY_BUFFER, gl_buffer_);
    OpenGLApi::EnableVertexAttribArray(attr_pos_);
    OpenGLApi::VertexAttribPointer(attr_pos_, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    OpenGLApi::EnableVertexAttribArray(attr_uv_);
    OpenGLApi::VertexAttribPointer(attr_uv_, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*) (8 * sizeof(GLfloat)));
    OpenGLApi::DrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    OpenGLApi::DisableVertexAttribArray(attr_pos_);
    OpenGLApi::DisableVertexAttribArray(attr_uv_);
    OpenGLApi::Flush();
    // 恢复fbo
    OpenGLApi::BindFramebuffer(GL_FRAMEBUFFER, fbo);
    return true;
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
    // 释放视频纹理
    if (video_texture_) {
        OpenGLApi::DeleteTexture(video_texture_);
        video_texture_ = 0;
    }
}

ANativeWindow* VideoBufferProcessor::CreateSurface(int width, int height) {
    if (width < 1 || height < 1) { // 检查宽高
        MLOGE("Video invalid video size %dx%d", width, height);
        return nullptr;
    }
    width_ = width;
    height_ = height;
    // 创建SurfaceTexture使用的OES纹理
    video_texture_ = OpenGLApi::CreateTexture();
    OpenGLApi::BindTexture(GL_TEXTURE_EXTERNAL_OES, video_texture_);
    OpenGLApi::TexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    OpenGLApi::TexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    OpenGLApi::TexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    OpenGLApi::TexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // 获取jni
    jclass clazz = JNIUtil::GetVideoSurfaceUtilClass();
    if (!clazz) {
        MLOGE("Video CreateSurface no clazz");
        return nullptr;
    }
    JNIEnv* env = JNIUtil::GetEnv();
    static jmethodID mid = env->GetStaticMethodID(clazz, "createSurface", "(I)[Ljava/lang/Object;");
    if (!mid) {
        MLOGE("Video CreateSurface no mid");
        return nullptr;
    }
    // 创建Surface
    auto jarr = (jobjectArray) env->CallStaticObjectMethod(clazz, mid, video_texture_);
    if (!jarr) {
        MLOGE("Video CreateSurface no arr");
        return nullptr;
    }
    // 保存结果
    auto surface_texture = (jobject) env->GetObjectArrayElement(jarr, 0);
    auto surface = (jobject) env->GetObjectArrayElement(jarr, 1);
    surface_texture_ref_ = env->NewGlobalRef(surface_texture);
    // nativewindow会被mediaflow释放
    native_window_ = ANativeWindow_fromSurface(env, surface);
    MLOGE("Video NativeWindow %p %d %d", native_window_, width, height);
    // 修改nativewindow大小
    ANativeWindow_setBuffersGeometry(native_window_, width, height, AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);
    env->DeleteLocalRef(surface_texture);
    env->DeleteLocalRef(surface);
    env->DeleteLocalRef(jarr);
    return native_window_;
}

void VideoBufferProcessor::ReleaseSurface() {
    // 调用java层的release方法
    JNIEnv *env = JNIUtil::GetEnv();
    do {
        if (surface_texture_ref_) {
            jclass clazz = JNIUtil::GetVideoSurfaceUtilClass();
            if (!clazz) {
                MLOGE("Video ReleaseSurface no clazz");
                break;
            }
            static jmethodID mid = env->GetStaticMethodID(clazz, "releaseSurface", "(Ljava/lang/Object;)V");
            if (!mid) {
                MLOGE("Video ReleaseSurface no mid");
                break;
            }
            MLOGD("Video ReleaseSurface %p %p %p %p", env, clazz, mid, surface_texture_ref_);
            env->CallStaticVoidMethod(clazz, mid, surface_texture_ref_);
        }
    } while (false);
    // 释放全局引用
    if (surface_texture_ref_) {
        env->DeleteGlobalRef(surface_texture_ref_);
        surface_texture_ref_ = nullptr;
    }
}

void VideoBufferProcessor::Setup() {
    setup_ = true;
    program_ = -1;

    v_shader_ = CreateShader(GL_VERTEX_SHADER, shader_vertex);
    f_shader_ = CreateShader(GL_FRAGMENT_SHADER, transparent_ ? shader_frag_transparent : shader_frag);
    if (v_shader_ == -1 || f_shader_ == -1) {
        return;
    }
    program_ = OpenGLApi::CreateProgram();
    OpenGLApi::AttachShader(program_, v_shader_);
    OpenGLApi::AttachShader(program_, f_shader_);
    OpenGLApi::LinkProgram(program_);
    GLint status;
    OpenGLApi::GetProgramiv(program_, GL_LINK_STATUS, &status);
    if (status == 0) {
        OpenGLApi::DeleteShader(v_shader_);
        OpenGLApi::DeleteShader(f_shader_);
        OpenGLApi::DeleteProgram(program_);
        v_shader_ = -1;
        f_shader_ = -1;
        program_ = -1;
        MLOGE("VideoBufferProcessor create program fail");
        return;
    }

    gl_buffer_ = OpenGLApi::CreateBuffer();
    static GLfloat texVertices[] = {
            -1, -1,
            1, -1,
            -1, 1,
            1, 1,
            0, 1,
            1, 1,
            0, 0,
            1, 0,
    };
    OpenGLApi::BindBuffer(GL_ARRAY_BUFFER, gl_buffer_);
    OpenGLApi::BufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), texVertices, GL_STATIC_DRAW);

    OpenGLApi::UseProgram(program_);
    attr_pos_ = OpenGLApi::GetAttribLocation(program_, "position");
    attr_uv_ = OpenGLApi::GetAttribLocation(program_, "texCoord");
}

}
