//
//  render_frame.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "render_frame.hpp"
#include "util/log_util.hpp"
#include "ri/render/mars_renderer.hpp"
#include "ri/render/render_pass.hpp"
#include "ri/render/render_frame_internal.hpp"

namespace mn {

RenderFrame::RenderFrame(MarsRenderer *renderer, const RenderFrameOptions& options) : name_(options.name), renderer_(nullptr) {
    MLOGD("RenderFrame Construct %p", renderer);
    clear_action_ = options.clear_action;
    if (!clear_action_) {
        clear_action_ = std::make_shared<RenderPassClearAction>();
    }
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

RenderFrame::~RenderFrame() {
    MLOGD("RenderFrame Destruct %s", name_.c_str());
}

void RenderFrame::AddRenderPass(std::shared_ptr<RenderPass> render_pass) {
    render_passes_.push_back(render_pass);
}

// todo: Hack不应该在这一层处理这个逻辑;
void RenderFrame::AddMeshToDefaultRendererPass(std::shared_ptr<MarsMesh> mesh) {
    if (render_passes_.size() > 0) {
        render_passes_[0]->AddMesh(mesh);
    } else {
        MLOGD("add mesh to default renderPass failed");
    }
}

void RenderFrame::AddClearAction(std::shared_ptr<RenderPassClearAction> clear_action) {
    clear_action_ = clear_action;
}

void RenderFrame::RemoveRenderPass(std::shared_ptr<RenderPass> render_pass) {
    //
}

void RenderFrame::AssignRenderer(MarsRenderer *renderer) {
    if (renderer) {
        this->renderer_ = renderer;
    }
}

void RenderFrame::SetSemeticValue(const std::string& semantic_name, std::shared_ptr<SemanticValue> value) {
    semantic_values_.insert({ semantic_name, value });
}

// todo: semantic names;
SemanticValue* RenderFrame::GetSemeticValue(const std::string& name) {
    // todo: 优化
    if (semantic_values_.find(name) != semantic_values_.end()) {
        return semantic_values_[name].get();
    }
    
    return nullptr;
}

void RenderFrame::Render() {
    RenderFrameInternal::Render(renderer_, this);
}

}
