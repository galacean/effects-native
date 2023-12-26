#ifndef MN_MARS_SURFACE_ANDROID_H_
#define MN_MARS_SURFACE_ANDROID_H_

#include "platform/egl/mars_surface.hpp"

namespace mn {

class EGLWindowContext;

class MarsSurfaceAndroid : public MarsSurface {
public:
    MarsSurfaceAndroid(bool enable_surface_scale);

    ~MarsSurfaceAndroid();

    bool Create(const SystemSurface &system_surface, int32_t s_width, int32_t s_height) override;

    bool OnSurfaceCreated(void* surface) override;

    void OnSurfaceResize(int width, int height) override;
        
    void Destroy() override;
    
    int MakeCurrent() override;
    
    int SwapBuffer() override;
    
    int GetSurfaceWidth() override;
    
    int GetSurfaceHeight() override;

    int GetDefaultFramebufferId() override;

    bool IsValid() override;

    bool IsSupportCompressedTexture() override;

    bool IsSurfaceless() override;

    int GetGLESVersion() override;

private:
    EGLWindowContext* context_;
};

}

#endif
