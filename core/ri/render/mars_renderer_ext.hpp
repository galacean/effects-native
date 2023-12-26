//
//  mars_renderer_ext.hpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef mars_renderer_ext_hpp
#define mars_renderer_ext_hpp

#include <stdio.h>
#include <string>
#include "ri/render/mars_texture.hpp"

namespace mn {

class RenderPass;

class MarsRenderer;

class MarsRendererExt  {
    
public:
    
    MarsRendererExt(MarsRenderer* renderer);
    
    void CopyTexture(std::shared_ptr<MarsTexture> source, std::shared_ptr<MarsTexture> dest);
    
//    void ResetColorAttachment(std::shared_ptr<RenderPass> )
    
private:
    
    void CreateCopyRenderPass();
    
    void Copy(std::shared_ptr<MarsTexture> source, std::shared_ptr<MarsTexture> dest);
    
    MarsRenderer* renderer_ = nullptr;
    
    std::shared_ptr<RenderPass> copy_render_pass_;
};


}

#endif /* mars_renderer_ext_hpp */
