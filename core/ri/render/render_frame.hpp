//
//  render_frame.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef render_frame_hpp
#define render_frame_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include <unordered_map>

#include "ri/render/render_pass.hpp"
#include "ri/render_base.hpp"
#include "ri/render/semantic_value.hpp"
#include "math/camera.hpp"

namespace mn {

class RenderState;

class MarsRenderer;

struct RenderFrameOptions {
    
    std::shared_ptr<RenderPassClearAction> clear_action;

    std::string name;
    
    std::shared_ptr<Camera> camera;
};

class RenderFrame {
    
public:

    RenderFrame(MarsRenderer *renderer, const RenderFrameOptions& options);
    
    virtual ~RenderFrame();
    
    void AddClearAction(std::shared_ptr<RenderPassClearAction> clear_action);
    
    void AddRenderPass(std::shared_ptr<RenderPass> render_pass);
    
    void RemoveRenderPass(std::shared_ptr<RenderPass> render_pass);
    
    void AddMeshToDefaultRendererPass(std::shared_ptr<MarsMesh> mesh);
    
    void AssignRenderer(MarsRenderer *renderer);

    void SetSemeticValue(const std::string& semantic_name, std::shared_ptr<SemanticValue> value);
    
    SemanticValue* GetSemeticValue(const std::string& name);
    
    void Render();
    
    std::vector<std::shared_ptr<RenderPass>>& GetRenderPasses() {
        return render_passes_;
    }
    
    std::shared_ptr<RenderPassClearAction>& GetRenderFrameClearAction() {
        return clear_action_;
    }
    
protected:
                        
    std::string name_;
    
    MarsRenderer *renderer_;
    
    std::shared_ptr<RenderPassClearAction> clear_action_;
    
    std::vector<std::shared_ptr<RenderPass>> render_passes_;
    
    // renderFrame: {"uModel": SemanticValue};  material: {"uModel": UniformValue}; 通过Key关联;
    std::unordered_map<std::string, std::shared_ptr<SemanticValue>> semantic_values_;
    
};

}

#endif /* render_frame_hpp */
