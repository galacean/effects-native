//
//  egl_surface.hpp
//
//  Created by Zongming Liu on 2021/10/22.
//

#ifndef mars_surface_hpp
#define mars_surface_hpp

#include <stdio.h>

namespace mn {

enum SystemSurfaceType {
    kSurfaceTypeUnkonwn,
    kSurfaceTypeGL
};

struct SystemSurface {
    
    void *surface = nullptr;
    
    SystemSurfaceType surface_type = kSurfaceTypeUnkonwn;
    
    virtual ~SystemSurface() {};
};

class MarsSurface {
    
public:
    
    virtual ~MarsSurface() {};
    
    virtual bool Create(const SystemSurface &system_surface, int32_t s_width, int32_t s_height) = 0;

    virtual bool OnSurfaceCreated(void* surface) = 0;
    
    virtual void OnSurfaceResize(int width, int height) = 0;
        
    virtual void Destroy() = 0;
    
    virtual int MakeCurrent() = 0;
    
    virtual int SwapBuffer() = 0;
    
    virtual int GetSurfaceWidth() = 0;
    
    virtual int GetSurfaceHeight() = 0;

    virtual int GetDefaultFramebufferId() = 0;

    virtual bool IsValid() = 0;
    
    virtual bool IsSupportCompressedTexture() = 0;

    virtual bool IsSurfaceless() { return false; }
    
    virtual int GetGLESVersion() = 0;
    
};

}
#endif /* mars_surface_hpp */
