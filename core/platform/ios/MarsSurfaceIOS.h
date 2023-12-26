//
//  mars_surface_ios.hpp
//
//  Created by Zongming Liu on 2021/11/8.
//

#ifndef mars_surface_ios_hpp
#define mars_surface_ios_hpp

#include <stdio.h>
#include "platform/egl/mars_surface.hpp"

namespace mn {

class SystemSurfaceHolder;

class MarsSurfaceIOS : public MarsSurface {
    
public:
    
    MarsSurfaceIOS();
    
    ~MarsSurfaceIOS();
    
    bool Create(const SystemSurface &system_surface, int32_t s_width, int32_t s_height) override;
    
    void OnSurfaceResize(int32_t width, int32_t height) override;

    bool OnSurfaceCreated(void*) override;
        
    void Destroy() override;
    
    int MakeCurrent() override;
    
    int SwapBuffer() override;
    
    int GetSurfaceWidth() override;
    
    int GetSurfaceHeight() override;
    
    int GetDefaultFramebufferId() override;
    
    bool IsSupportCompressedTexture() override;
    
    bool IsValid() override;
    
    int GetGLESVersion() override;
    
private:
    
    SystemSurfaceHolder* surface_ = nullptr;
    
};

}

#endif /* mars_surface_ios_hpp */
