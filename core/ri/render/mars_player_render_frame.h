#ifndef MN_RI_RENDER_MARS_PLAYER_RENDER_FRAME_H_
#define MN_RI_RENDER_MARS_PLAYER_RENDER_FRAME_H_

#include "render_frame.hpp"

namespace mn {

struct MarsPlayerRenderFrameResource {
    
    RenderPassAttachmentStorageType storage_type;
    
    std::shared_ptr<MarsTexture> color_a;
    
    std::shared_ptr<MarsTexture> color_b;
    
    std::shared_ptr<RenderPass> final_copy_rp;
    
    std::shared_ptr<RenderPass> res_rp;
    
};

class FilterDefine;

class MarsPlayerRenderFrame : public RenderFrame {

public:
    
    MarsPlayerRenderFrame(MarsRenderer* renderer, const RenderFrameOptions& opt);
    
    ~MarsPlayerRenderFrame();

    void AddMeshToDefaultRenderPass(std::shared_ptr<MarsMesh> mesh);

    void RemoveMeshToDefaultRenderPass(std::shared_ptr<MarsMesh> mesh);

    void OnRendererResize();
    
    std::shared_ptr<RenderPass> SplitDefaultRenderPassByMesh(std::shared_ptr<MarsMesh> mesh, FilterDefine* filter_define);
    
    std::shared_ptr<RenderPass> FindPreviousRenderPass(const RenderPass* render_pass);

public:
    
    std::shared_ptr<MarsTexture> empty_texture_;
    
private:
    
    void CreateResource();
    
    std::shared_ptr<MarsMesh> CreateCopyMesh();
    
    void CreateCopyShader(Shader& shader);
    
    RenderPassColorAttachmentOptions GetPRAttachments(std::shared_ptr<RenderPass> pre_render_pass);
    
    int FindMeshRenderPassIndex(std::shared_ptr<MarsMesh> mesh);
    
    int FindRenderPassIndex(std::vector<std::shared_ptr<RenderPass>> passes, const RenderPass* render_pass);
    
    std::shared_ptr<RenderPass> FindPreviousDefaultRenderPass(const RenderPass* render_pass);
        
    std::shared_ptr<RenderPass> FindPreviousRenderPass(std::vector<std::shared_ptr<RenderPass>> passes, const RenderPass* current_pass);
    
    std::vector<std::shared_ptr<RenderPass>> default_render_passes_;

    MarsPlayerRenderFrameResource* resource_ = nullptr;
    
    std::shared_ptr<Camera> camera_;
    
    std::shared_ptr<MarsTexture> transparent_texture_;
    
};

}

#endif
