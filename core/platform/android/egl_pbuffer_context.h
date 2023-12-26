#ifndef MN_EGL_PBUFFER_CONTEXT_H_
#define MN_EGL_PBUFFER_CONTEXT_H_

#include "ri/gl.h"

namespace mn {

class EGLPBufferContext {
public:
    EGLPBufferContext();
    
    ~EGLPBufferContext();

    bool Init(void* shared_egl_context);

    bool MakeCurrent();

    bool IsValid();

    int GetGLESVersion();

private:
    void CreateSurface();

private:
    EGLDisplay egl_display_ = EGL_NO_DISPLAY;

    EGLContext egl_context_ = EGL_NO_CONTEXT;

    EGLSurface egl_surface_ = EGL_NO_SURFACE;

    EGLConfig egl_config_;

    int32_t color_size_;
    int32_t depth_size_;

    int gles_version_ = 0;
};

}

#endif
