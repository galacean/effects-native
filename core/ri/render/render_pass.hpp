//
//  render_pass.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef render_pass_hpp
#define render_pass_hpp

#include <stdio.h>
#include <vector>

#include "ri/render/mars_mesh.hpp"
#include "math/camera.hpp"
#include "ri/render/render_pass_color_attachment.hpp"
#include "ri/render_base.hpp"

namespace mn {

class RenderPassDelegate {
  
public:
    
    virtual void WillBeginRenderPass(const RenderPass* render_pass, const RenderState& render_state) { };
    
    virtual void DidEndRenderPass(const RenderPass* render_pass, const RenderState& render_state) { };
    
    virtual void WillRenderMesh(std::shared_ptr<MarsMesh> mesh, const RenderState& render_state) { };
    
    virtual void DidRenderMesh(std::shared_ptr<MarsMesh> mesh, const RenderState& render_state) { };
    
};

class RenderPass {
    
public:
    
    RenderPass(MarsRenderer* renderer, int priority = 0);
    
    RenderPass(MarsRenderer* renderer, const std::string& name, int priority = 0);
    
    RenderPass(MarsRenderer* renderer, int viewport[4], const std::string& name, int priority = 0);
        
    ~RenderPass();
    
    void SetViewPortOptions(const RenderPassAttachmentOptions& options);
    
    void SetRenderPassAttachent(const RenderPassAttachmentOptions& attachment_options, CLEAR_ACTION_TYPE clear_action_type);
    
    void SetRenderPassDelegate(std::shared_ptr<RenderPassDelegate> delegate);
    
    void SetSemanticValues(const std::string& key, std::shared_ptr<SemanticValue> value);
    
    SemanticValue* GetSemanticValue(const std::string& name) {
        if (semantic_values_.find(name) != semantic_values_.end()) {
            return semantic_values_[name].get();
        }
        
        return nullptr;
    }
    
    void SetCamera(std::shared_ptr<Camera> camera);
    
    std::shared_ptr<Camera> GetCamera() {
        return camera_;
    }
        
    void ResetColorAttachments(std::vector<MarsTexture*> colors);
    
    void ResetAttachments(const RenderPassAttachmentOptions& attachment_option);
        
    void AddMesh(std::shared_ptr<MarsMesh> mesh);
    
    void RemoveMesh(std::shared_ptr<MarsMesh> mesh);
    
    void SetMeshes(const std::vector<std::shared_ptr<MarsMesh>> meshess);
    
    std::shared_ptr<RenderPassClearAction> ClearAction() {
        return clear_action_;
    }
    
    std::vector<std::shared_ptr<MarsMesh>>& Meshes() {
        return meshes_;
    }
    
    void GetStencilAttachment();
    
    void GetDepthAttachment();
    
    std::shared_ptr<RenderPassColorAttachment> GetColorAttachment() const;
        
    std::shared_ptr<GLFrameBuffer> GetFramebuffer() {
        return frame_buffer_;
    }
    
    std::shared_ptr<RenderPassDelegate> GetPassDelegate() {
        return render_pass_delegate_;
    }
    
    std::string& Name() {
        return name_;
    }
    
    int Priority() {
        return priority_;
    }
    
    void Bind();
    
private:
    void InnerResetAttachMent(std::vector<RenderPassColorAttachmentOptions> color_attachment_options, RenderPassAttachmentStorageType storage_type);
    
    void GetViewport(int* viewport);
    
    MarsTexture* GetDepthTexture();
    
    MarsTexture* GetStencilTexture();
    
    void AssignRenderer(MarsRenderer *renderer);
    
// todo: sematice value 内部使用;
//private:
public:
    
    int priority_;
    
    std::string name_;
    
    std::shared_ptr<Camera> camera_;
    
    std::shared_ptr<GLFrameBuffer> frame_buffer_;
    
    std::shared_ptr<RenderPassClearAction> clear_action_;
    
    MarsRenderer *renderer_ = nullptr;
    
    RenderPassMeshOrder mesh_order_ = RenderPassMeshOrder::ASCENDING;
    
    int viewport_[4] = { 0, 0, 0, 0 };
    
    float viewport_scale_ = 1;
    
    bool custom_viewport_ = false;
    
    std::vector<std::shared_ptr<RenderPassColorAttachment>> attachments_;
    
    MarsTexture* depth_texture_ = nullptr;
    
    MarsTexture* stencil_texture_ = nullptr;
    
    std::vector<std::shared_ptr<MarsMesh>> meshes_;
    
    std::unordered_map<std::string, std::shared_ptr<SemanticValue>> semantic_values_;
    
    std::shared_ptr<RenderPassDelegate> render_pass_delegate_;

};

}

#endif /* render_pass_hpp */
