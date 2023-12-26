#include "egl_pbuffer_context.h"
#include "util/log_util.hpp"

namespace mn {

EGLPBufferContext::EGLPBufferContext() {
    MLOGD("EGLPBufferContext::%s", __FUNCTION__);
}

EGLPBufferContext::~EGLPBufferContext() {
    MLOGD("EGLPBufferContext::%s", __FUNCTION__);
    if (egl_context_ != EGL_NO_CONTEXT) {
        eglDestroyContext(egl_display_, egl_context_);
    }
    if (egl_surface_ != EGL_NO_SURFACE) {
        eglDestroySurface(egl_display_, egl_surface_);
    }
}

bool EGLPBufferContext::Init(void* shared_egl_context) {
    MLOGD("EGLWindowContext::%s", __FUNCTION__);
    if (egl_display_ != EGL_NO_DISPLAY) {
        return (egl_surface_ != EGL_NO_SURFACE);
    }
    egl_display_ = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(egl_display_, 0, 0);

    const EGLint attribs[] = {EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
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
            EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
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

    CreateSurface();

    EGLint target_version = 3;
    EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION,
        target_version,  // Request opengl ES Version
        EGL_NONE};
    bool is3 = true;
    egl_context_ = eglCreateContext(egl_display_, egl_config_, shared_egl_context, context_attribs);
    if (egl_context_ == EGL_NO_CONTEXT) {
        MLOGD("downgrade to gles2.0");
        is3 = false;
        target_version = 2;
        context_attribs[1] = target_version;
        egl_context_ = eglCreateContext(egl_display_, egl_config_, shared_egl_context, context_attribs);
    }

    if (egl_context_ == EGL_NO_CONTEXT) {
        EGLint err = eglGetError();
        MLOGE("egl context is null. %d", err);
        return false;
    }
    gles_version_ = target_version;

    return MakeCurrent();
}

bool EGLPBufferContext::MakeCurrent() {
    EGLBoolean result = eglMakeCurrent(egl_display_, egl_surface_, egl_surface_, egl_context_);
    if (!result) {
        EGLint error = eglGetError();
        MLOGE("egl make current error: %d", error);
        return false;
    }
    return true;
}

bool EGLPBufferContext::IsValid() {
    return egl_surface_ != EGL_NO_SURFACE && egl_context_ != EGL_NO_CONTEXT;
}

void EGLPBufferContext::CreateSurface() {
    if (egl_surface_ != EGL_NO_SURFACE) {
        return;
    }
    EGLint attrs[] =
            {
                    EGL_WIDTH, 1,
                    EGL_HEIGHT, 1,
                    EGL_NONE
            };
    egl_surface_ = eglCreatePbufferSurface(egl_display_, egl_config_, attrs);
    MLOGD("EGLPBufferContext::%s %p", __FUNCTION__, this);
}

int EGLPBufferContext::GetGLESVersion() {
    return gles_version_;
}

}
