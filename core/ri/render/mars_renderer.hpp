//
//  mars_renderer.hpp
//
//  Created by Zongming Liu on 2022/4/7.
//

#ifndef mars_renderer_hpp
#define mars_renderer_hpp

#include <stdio.h>
#include <vector>
#include "ri/backend/opengl/gl_gpu_renderer.hpp"
#include "ri/render/render_frame.hpp"
#include "platform/egl/mars_surface.hpp"
#include "ri/render/mars_renderer_ext.hpp"
#include "ri/backend/opengl/gl_gpu_capability.hpp"


namespace mn {

class MarsRenderer {
  
public:
    
    MarsRenderer();
    
    MarsRenderer(mn::MarsSurface *surface);
    
    ~MarsRenderer();
    
    GLGPURenderer *InnerRenderer() {
        return renderer_;
    }
    
    RenderFrame *CreateRenderFrame(const std::string& name);
    
    void Resize();
    
    int GetSurfaceWidth() {
        return surface_width_;
    }
    
    int GetSurfaceHeight() {
        return surface_height_;
    }
    
    void MakeCurrent();
    
    void SwapBuffer();
    
    void BindSystemFramebuffer();
    
    MarsRendererExt* GetRendererExt() {
        return renderer_ext_;
    }
    
    std::shared_ptr<GLGPUCapability> GPUCapability();

private:

    GLGPURenderer *renderer_;
    
    mn::MarsSurface* surface_;
    
    MarsRendererExt* renderer_ext_ ;
    
    std::vector<RenderFrame *> render_frames_;
    
    int surface_width_;
    
    int surface_height_;

};

}

#endif /* mars_renderer_hpp */
