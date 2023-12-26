#include "egl_window_context.h"
#include <android/native_window.h>
#include "util/log_util.hpp"

namespace mn {

EGLWindowContext::EGLWindowContext(bool enable_surface_scale) {
    MLOGD("EGLWindowContext::%s enableSurfaceScale: %d", __FUNCTION__, enable_surface_scale);
    enable_surface_scale_ = enable_surface_scale;
}

EGLWindowContext::~EGLWindowContext() {
    MLOGD("EGLWindowContext::%s", __FUNCTION__);
    if (egl_context_ != EGL_NO_CONTEXT) {
        eglDestroyContext(egl_display_, egl_context_);
    }
    if (egl_surface_ != EGL_NO_SURFACE) {
        eglDestroySurface(egl_display_, egl_surface_);
    }

    if (native_window_) {
        ANativeWindow_release((ANativeWindow*)native_window_);
    }
}

bool EGLWindowContext::Init(void* window) {
    MLOGD("EGLWindowContext::%s", __FUNCTION__);
    if (egl_display_ != EGL_NO_DISPLAY) {
        if (egl_context_ == EGL_NO_CONTEXT) {
            MLOGE("egl context invalid");
            return false;
        }
        MLOGD("recreate egl surface");
        CreateSurface(window);
        return (egl_surface_ != EGL_NO_SURFACE);
    }
    egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(egl_display_, 0, 0);

    const EGLint attribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_ALPHA_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE};
    
    color_size_ = 8;
    depth_size_ = 24;
    EGLint num_configs;
    eglChooseConfig(egl_display_, attribs, &egl_config_, 1, &num_configs);

    if (!num_configs) {
        // Fall back to 16bit depth buffer
        const EGLint attribsLowBit[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_ALPHA_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, 8,
            EGL_NONE};
        eglChooseConfig(egl_display_, attribsLowBit, &egl_config_, 1, &num_configs);
        depth_size_ = 16;
    }
    if (!num_configs) {
        MLOGE("egl choose config failed");
        return false;
    }

    CreateSurface(window);

    EGLint target_version = 3;
    EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
        target_version,  // Request opengl ES Version
        EGL_NONE};
    bool is3 = true;
    egl_context_ = eglCreateContext(egl_display_, egl_config_, NULL, context_attribs);
    if (egl_context_ == EGL_NO_CONTEXT) {
        MLOGD("downgrade to gles2.0");
        is3 = false;
        target_version = 2;
        context_attribs[1] = target_version;
        egl_context_ = eglCreateContext(egl_display_, egl_config_, NULL, context_attribs);
    }

    if (egl_context_ == EGL_NO_CONTEXT) {
        EGLint err = eglGetError();
        MLOGE("egl context is null. %d", err);
        return false;
    }
    gles_version_ = target_version;

    return MakeCurrent();
}

bool EGLWindowContext::IsValid() {
    return egl_surface_ != EGL_NO_SURFACE && egl_context_ != EGL_NO_CONTEXT;
}

void EGLWindowContext::Resize(int width, int height) {
    // ANativeWindow自带宽高信息，无需再传入
    Init(native_window_);
    MLOGD("EGLWindowContext::Resize %d %d", width_, height_);
}

bool EGLWindowContext::MakeCurrent() {
    EGLBoolean result = eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_);
    if (!result) {
        EGLint error = eglGetError();
        MLOGE("egl make current error: %d", error);
        return false;
    }
    return true;
}

bool EGLWindowContext::SwapBuffers() {
    // todo: 检查上下文
    EGLBoolean b = eglSwapBuffers(egl_display_, egl_surface_);
    return b == EGL_SUCCESS;
}

int EGLWindowContext::GetWidth() {
    return width_;
}

int EGLWindowContext::GetHeight() {
    return height_;
}

void EGLWindowContext::CreateSurface(void* window) {
    if (egl_surface_ != EGL_NO_SURFACE) {
        if (!window && use_pbuffer_) {
            return;
        }
        eglDestroySurface(egl_display_, egl_surface_);
        egl_surface_ = EGL_NO_SURFACE;
    }

    EGLSurface surface = EGL_NO_SURFACE;
    if (window == nullptr) {
        EGLint pbufAttribs[] =
                {
                        EGL_WIDTH, width_,
                        EGL_HEIGHT, height_,
                        EGL_LARGEST_PBUFFER, EGL_TRUE,
                        EGL_NONE
                };
        surface = eglCreatePbufferSurface(egl_display_, egl_config_, pbufAttribs);
        use_pbuffer_ = true;
    } else {
        if (enable_surface_scale_) {
            int32_t width = ANativeWindow_getWidth((ANativeWindow *)window) * 0.5;
            int32_t height = ANativeWindow_getHeight((ANativeWindow *)window) * 0.5;
            ANativeWindow_setBuffersGeometry((ANativeWindow *)window, width, height, AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM);
        }

        surface = eglCreateWindowSurface(egl_display_, egl_config_, (ANativeWindow*)window, NULL);
        use_pbuffer_ = false;
        native_window_ = window;
    }
    
    if (surface == EGL_NO_SURFACE) {
        EGLint err = eglGetError();
        MLOGE("egl surface is null. %d", err);
        return;
    }
    eglQuerySurface(egl_display_, surface, EGL_WIDTH, &width_);
    eglQuerySurface(egl_display_, surface, EGL_HEIGHT, &height_);
    MLOGD("EGLWindowContext::%s %p %d %d", __FUNCTION__, this, width_, height_);

    egl_surface_ = surface;
}

int EGLWindowContext::GetGLESVersion() {
    return gles_version_;
}

}
