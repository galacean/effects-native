#ifndef MN_EGL_WINDOW_CONTEXT_H_
#define MN_EGL_WINDOW_CONTEXT_H_

#include "ri/gl.h"

namespace mn {

class EGLWindowContext {
public:
    EGLWindowContext(bool disable_surface_scale);

    ~EGLWindowContext();

    bool Init(void* window);

    void Resize(int width, int height);

    bool MakeCurrent();

    bool SwapBuffers();

    int GetWidth();

    int GetHeight();

    bool IsValid();

    bool UsingPBuffer() const {
        return use_pbuffer_;
    }

    int GetGLESVersion();

private:
    void CreateSurface(void* window);

private:
    bool no_egl_terminate_ = true;

    EGLDisplay egl_display_ = EGL_NO_DISPLAY;

    EGLContext egl_context_ = EGL_NO_CONTEXT;

    EGLSurface egl_surface_ = EGL_NO_SURFACE;

    EGLConfig egl_config_;

    int32_t color_size_;
    int32_t depth_size_;

    int width_ = 1;
    int height_ = 1;

    bool use_pbuffer_ = false;

    int gles_version_ = 0;

    void* native_window_ = nullptr;

    bool enable_surface_scale_ = false;

};

}

#endif
