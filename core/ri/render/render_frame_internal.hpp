//
//  render_frame_internal.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef render_frame_internal_hpp
#define render_frame_internal_hpp

#include <stdio.h>
#include "ri/render/mars_renderer.hpp"

namespace mn {

class RenderFrameInternal {
    
public:
    
    static void Render(MarsRenderer* renderer, RenderFrame* render_frame);
        
    static void RenderRenderPass(MarsRenderer* renderer, std::shared_ptr<RenderPass> render_pass, RenderState& state);
    
private:
    
    static void EndRenderPass(MarsRenderer* renderer, std::shared_ptr<RenderPass> render_pass);
    
    static void Clear(MarsRenderer* renderer, const RenderPassClearAction* clear_action);
        
    static void RenderMeshesByCamera(MarsRenderer* renderer, std::shared_ptr<Camera> camera, RenderState& state);
    
    static void SetupRenderPass(MarsRenderer* renderer, std::shared_ptr<RenderPass> render_pass);

};

}

#endif /* render_frame_internal_hpp */
