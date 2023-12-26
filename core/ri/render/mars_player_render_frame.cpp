#include "mars_player_render_frame.h"
#include "mars_renderer.hpp"
#include "ri/material.h"
#include "filters/filter_define.hpp"
#include <algorithm>
#include "util/util.hpp"

namespace mn {

static int RenderPassPriorityNormal = 1000;

const std::string DEF_NAME = "_mars_default_";
const std::string MARS_COPY_MESH_NAME = "mars-internal-copy";
const std::string COPY_MESH_SHADER_ID = "mars-internal-copy-mesh";

MarsPlayerRenderFrame::MarsPlayerRenderFrame(MarsRenderer* renderer, const RenderFrameOptions& opt) : RenderFrame(renderer, opt) {
    empty_texture_ = MaterialUtil::CreateEmptyTextureContainer(renderer);
    camera_ = opt.camera;
    class CameraSemanticValue : public SemanticValue {
    public:
        CameraSemanticValue(std::shared_ptr<Camera> camera, int type) : camera_(camera), type_(type) {
        }
        
        void GetSemantiveValue(const RenderState& state, std::shared_ptr<UniformValue> value) override {
            switch (type_) {
                case 1:
                    value->SetData(camera_->View());
                    break;
                
                case 2:
                    value->SetData(camera_->InverseView());
                    break;
                    
                case 3:
                    value->SetData(camera_->Projection());
                    break;
                    
                case 4:
                    value->SetData(camera_->ViewProjection());
                    break;
                    
                default:
                    break;
            }
        }
        
    private:
        std::shared_ptr<Camera> camera_;
        int type_;
    };
    
    // todo: sprite和particle不一样，重构下semantics的逻辑
    SetSemeticValue("uViewP", std::make_shared<CameraSemanticValue>(camera_, 1));
    SetSemeticValue("uView", std::make_shared<CameraSemanticValue>(camera_, 2));
//    SetSemeticValue("PROJECTION", std::make_shared<CameraSemanticValue>(camera_, 3));
    SetSemeticValue("uViewProjection", std::make_shared<CameraSemanticValue>(camera_, 4));
    
    
    class StateSemanticValue : public SemanticValue {
    public:
        void GetSemantiveValue(const RenderState& state, std::shared_ptr<UniformValue> value) override {
            value->SetData(state.current_mesh->WorldMatrix());
        }
    };
    SetSemeticValue("uModel", std::make_shared<StateSemanticValue>());
    
    class PrePassSemanticValue : public SemanticValue {
    public:
        PrePassSemanticValue(MarsPlayerRenderFrame* player_render_frame, int type): player_render_frame_(player_render_frame), type_(type) {
            
        }
        void GetSemantiveValue(const RenderState& state, std::shared_ptr<UniformValue> value) override {
            DEBUG_MLOGD("PrePassSemantic Value type is %d", type_);
            if (player_render_frame_ != nullptr) {
                switch (type_) {
                    case 1: {
                        if (value->data_type == SAMPLER_2D) {
                            auto rp = player_render_frame_->FindPreviousRenderPass(state.current_render_pass);
                            if (rp && rp->GetColorAttachment()) {
                                value->SetTexture(rp->GetColorAttachment()->GetMarsTexture());
                            } else {
                                MLOGE("GetPrePassSemantiveValue ColorAttatchment is null %d", type_);
                                value->SetTexture(player_render_frame_->transparent_texture_);
                            }
                        } else {
                            MLOGE("GetPrePassSemantiveValue Failed type %d", type_);
                        }
                        break;
                    }
                    case 2: {
                        if (value->data_type == FLOAT_VEC2) {
                            auto rp = player_render_frame_->FindPreviousRenderPass(state.current_render_pass);
                            if (rp && rp->GetColorAttachment()) {
                                auto texture = rp->GetColorAttachment()->GetMarsTexture();
                                float size[2] = { (float)texture->Width(), (float)texture->Height() };
                                value->SetData(size, 0, 2);
                            } else {
                                MLOGE("GetPrePassSemantiveValue ColorAttatchment is null %d", type_);
                            }
                        } else {
                            MLOGE("GetPrePassSemantiveValue Failed type %d", type_);
                        }
                        break;
                    }
                    case 3: {
                        if (value->data_type == SAMPLER_2D) {
                            auto rp = player_render_frame_->FindPreviousDefaultRenderPass(state.current_render_pass);
                            if (rp && rp->GetColorAttachment()) {
                                value->SetTexture(rp->GetColorAttachment()->GetMarsTexture());
                            } else {
                                MLOGE("GetPrePassSemantiveValue ColorAttatchment is null %d", type_);
                                value->SetTexture(player_render_frame_->transparent_texture_);
                            }
                        } else {
                            MLOGE("GetPrePassSemantiveValue Failed type %d", type_);
                        }
                        break;
                    }
                    case 4: {
                        if (value->data_type == FLOAT_VEC2) {
                            auto rp = player_render_frame_->FindPreviousDefaultRenderPass(state.current_render_pass);
                            if (rp && rp->GetColorAttachment()) {
                                auto texture = rp->GetColorAttachment()->GetMarsTexture();
                                float size[2] = { (float)texture->Width(), (float)texture->Height() };
                                value->SetData(size, 0, 2);
                            } else {
                                MLOGE("GetPrePassSemantiveValue ColorAttatchment is null %d", type_);
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            } else {
                MLOGE("PrePassSemantic Value Get failed");
            }
        }
        
    private:
        MarsPlayerRenderFrame* player_render_frame_ = nullptr;
        int type_;
    };
    
    SetSemeticValue(SEMANTIC_PRE_COLOR_ATTACHMENT_0, std::make_shared<PrePassSemanticValue>(this, 1));
    SetSemeticValue(SEMANTIC_PRE_COLOR_ATTACHMENT_SIZE_0, std::make_shared<PrePassSemanticValue>(this, 2));
    SetSemeticValue(SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_0, std::make_shared<PrePassSemanticValue>(this, 3));
    SetSemeticValue(SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_SIZE_0, std::make_shared<PrePassSemanticValue>(this, 4));
    
    auto render_pass = std::make_shared<RenderPass>(renderer, DEF_NAME, RenderPassPriorityNormal);
    render_pass->camera_ = camera_;
    render_pass->mesh_order_ = RenderPassMeshOrder::ASCENDING;
    AddRenderPass(render_pass);
    
    TextureOption transparent_option(1, 1, TextureSourceType::DATA);
    uint8_t transparent_data[4] = { 0, 0, 0, 0 };
    transparent_option.SetData(transparent_data);
    this->transparent_texture_ = std::make_shared<MarsTexture>(renderer_, transparent_option);
    
    auto first_rp = this->render_passes_[0];
    this->default_render_passes_.push_back(first_rp);
    
    Shader copy_shader;
    this->CreateCopyShader(copy_shader);
    this->renderer_->InnerRenderer()->ShaderLibrary().CompileShader(COPY_MESH_SHADER_ID);
}

MarsPlayerRenderFrame::~MarsPlayerRenderFrame() {
    if (resource_) {
        delete resource_;
    }
}

void MarsPlayerRenderFrame::AddMeshToDefaultRenderPass(std::shared_ptr<MarsMesh> mesh) {
    render_passes_[0]->AddMesh(mesh);
}

void MarsPlayerRenderFrame::RemoveMeshToDefaultRenderPass(std::shared_ptr<MarsMesh> mesh) {
    render_passes_[0]->RemoveMesh(mesh);
}

void MarsPlayerRenderFrame::OnRendererResize() {
    MLOGD("MarsPlayerRenderFrame::OnRendererResize renderer_ %p, width %d, height %d", renderer_, renderer_->GetSurfaceWidth(), renderer_->GetSurfaceHeight());
//    mn::RenderPassAttachmentOptions attachment_options;
//    attachment_options.SetViewport(0, 0, renderer_->GetSurfaceWidth(), renderer_->GetSurfaceHeight());
//    render_passes_[0]->SetViewPortOptions(attachment_options);
}


void MarsPlayerRenderFrame::CreateResource() {
    if (!this->resource_) {
        int width = renderer_->GetSurfaceWidth();
        int height = renderer_->GetSurfaceHeight();
        resource_ = new MarsPlayerRenderFrameResource();
        TextureOption tex_a_opt(width, height, TextureSourceType::FRAMEBUFFER);
        std::shared_ptr<MarsTexture> texture_a = std::make_shared<MarsTexture>(renderer_, tex_a_opt);
        resource_->color_a = texture_a;
        
        TextureOption tex_b_opt(width, height, TextureSourceType::FRAMEBUFFER);
        std::shared_ptr<MarsTexture> texture_b = std::make_shared<MarsTexture>(renderer_, tex_b_opt);
        resource_->color_b = texture_b;
        
        std::shared_ptr<RenderPass> res_render_pass = std::make_shared<RenderPass>(renderer_, "resource_pass");
        RenderPassAttachmentOptions res_attachment_option;
        res_attachment_option.SetDepthStencilAttachmentType(RenderPassAttachmentStorageType::DEPTH_STENCIL_OPAQUE);
        res_attachment_option.AddRenderPassColorAttachmentOption(texture_a);
        res_render_pass->SetRenderPassAttachent(res_attachment_option, CLEAR_ACTION_TYPE::CLEAR_DEFAULT);
        resource_->res_rp = res_render_pass;
        
        std::shared_ptr<RenderPass> final_copy_pass = std::make_shared<RenderPass>(renderer_, "mars-final-copy", RenderPassPriorityNormal + 600);
        RenderPassAttachmentOptions final_copy_attachment_option;
        final_copy_pass->SetCamera(camera_);
        std::shared_ptr<MarsMesh> copy_mesh = this->CreateCopyMesh();
        final_copy_pass->AddMesh(copy_mesh);
        
        resource_->final_copy_rp = final_copy_pass;
    }
}

// todo: 10.2.90 status状态;
std::shared_ptr<MarsMesh> MarsPlayerRenderFrame::CreateCopyMesh() {
    std::shared_ptr<MarsMesh> copy_mesh = std::make_shared<MarsMesh>(renderer_, MARS_COPY_MESH_NAME);
    std::shared_ptr<MaterialRenderStates> render_state = std::make_shared<MaterialRenderStates>();
    
    MaterialOptions material_option(render_state);
    material_option.name = MARS_COPY_MESH_NAME;
    this->CreateCopyShader(material_option.shader);
    
    std::shared_ptr<MarsMaterial> material = std::make_shared<MarsMaterial>(renderer_, material_option);
    
//    material->SetUniformValue("uDepth", std::make_shared<UniformValue>(SAMPLER_2D));
    material->SetUniformValue("uFilterSource", std::make_shared<UniformValue>(SAMPLER_2D, true, SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_0));
    material->SetUniformValue("uFilterSourceSize", std::make_shared<UniformValue>(SAMPLER_2D, true, SEMANTIC_MAIN_PRE_COLOR_ATTACHMENT_SIZE_0));
    
    copy_mesh->SetMaterial(material);
    std::shared_ptr<MarsGeometry> geometry = std::make_shared<MarsGeometry>(renderer_, 6, 0, GL_TRIANGLES);
    std::shared_ptr<mn::GPUBufferOption> index_buffer = std::make_shared<mn::GPUBufferOption>(mn::BufferDataType::UNSIGNED_SHORT);
    uint16_t index[6] = { 0, 1, 2, 2, 1, 3 };
    index_buffer->SetData((uint8_t *)index, 6 * sizeof(uint16_t));
    geometry->SetIndexInfo(index_buffer);
    
    float pos[] = { -1, 1, -1, -1, 1, 1, 1, -1 };
    AttributeWithData attr_pos(2, 2 * sizeof(float), 0);
    attr_pos.SetData((uint8_t *)pos, 8 * sizeof(float));
    geometry->SetAttributeInfo("aPos", attr_pos);
    copy_mesh->SetGeometry(geometry);
    
    return copy_mesh;
}

void MarsPlayerRenderFrame::CreateCopyShader(Shader& shader) {
    const std::string vertex_shader = R"(
        precision highp float;
        attribute vec2 aPos;
        varying vec2 vTex;
    
        void main() {
            gl_Position = vec4(aPos,0.,1.0);
            vTex = (aPos + vec2(1.0))/2.;
        }
    )";
    
    // todo: 10.3.10 sourceSize 优化下；线性插值；
    const std::string frag_shader = R"(
        precision mediump float;
        varying vec2 vTex;
        
        uniform vec2 uFilterSourceSize;
        uniform sampler2D uFilterSource;
    
        void main() {
            gl_FragColor = texture2D(uFilterSource, vTex);
        }
    )";
    
    shader.vertex = vertex_shader;
    shader.fragment = frag_shader;
    shader.cache_id = COPY_MESH_SHADER_ID;
    shader.name = COPY_MESH_SHADER_ID;
}

std::shared_ptr<RenderPass> MarsPlayerRenderFrame::SplitDefaultRenderPassByMesh(std::shared_ptr<MarsMesh> mesh, FilterDefine* filter_define) {
    int pass_index = this->FindMeshRenderPassIndex(mesh);
    if (pass_index == -1) {
        MLOGE("Split Default RenderPass Mesh Failed");
        return nullptr;
    }
    
    this->CreateResource();
    auto render_pass = this->render_passes_[pass_index];
    auto meshes = render_pass->Meshes();
    int mesh_index = 0;
    for (int i=0; i<meshes.size(); i++) {
        if (mesh == meshes[i]) {
            mesh_index = i;
            break;
        }
    }
    auto mesh_0 = std::vector<std::shared_ptr<MarsMesh>>(meshes.begin(), meshes.begin() + mesh_index);
    auto mesh_1 = std::vector<std::shared_ptr<MarsMesh>>(meshes.begin() + mesh_index, meshes.end());
    
    int def_index = Utils::VectorFindIndex(default_render_passes_, render_pass);
    if (def_index == -1) {
        MLOGE("not find render pass");
        return nullptr;
    }

    std::shared_ptr<RenderPass> last_def_pr;
    if (Utils::VectorInOfRange(default_render_passes_, def_index - 1)) {
        last_def_pr = default_render_passes_.at(def_index -1);
    }
    
    Utils::VectorRemove(default_render_passes_, render_pass);
    std::shared_ptr<RenderPass> render_pass_0 = std::make_shared<RenderPass>(renderer_, DEF_NAME + std::to_string(def_index), render_pass->Priority());
    RenderPassAttachmentOptions attachment_options_0;
    attachment_options_0.AddRenderPassColorAttachmentOption(this->GetPRAttachments(last_def_pr));
    render_pass_0->SetRenderPassAttachent(attachment_options_0, CLEAR_ACTION_TYPE::CLEAR_DEFAULT);
    render_pass_0->SetMeshes(mesh_0);
    
    this->render_passes_[pass_index] = render_pass_0;
    
    mesh_1.insert(mesh_1.begin(), this->CreateCopyMesh());
    std::vector<std::shared_ptr<RenderPass>> pre_render_passes = filter_define->GeneratePreRenderPasses(mesh_1);
    this->render_passes_.insert(this->render_passes_.begin() + pass_index + 1, pre_render_passes.begin(), pre_render_passes.end());

    if (Utils::VectorFindIndex(this->render_passes_, this->resource_->final_copy_rp) == -1) {
        this->render_passes_.push_back(this->resource_->final_copy_rp);
    }

    std::shared_ptr<RenderPass> render_pass_1 = std::make_shared<RenderPass>(renderer_, DEF_NAME + std::to_string(def_index+1), render_pass->Priority() + 1);
    RenderPassAttachmentOptions attachment_options_1;
    attachment_options_1.AddRenderPassColorAttachmentOption(this->GetPRAttachments(render_pass_0));
    render_pass_1->SetRenderPassAttachent(attachment_options_1, CLEAR_ACTION_TYPE::CLEAR_DEFAULT);
    render_pass_1->SetMeshes(mesh_1);
//
    this->render_passes_.insert(this->render_passes_.begin() + pass_index + 1 + pre_render_passes.size(), render_pass_1);
    Utils::VectorAdd(this->default_render_passes_, render_pass_0);
    Utils::VectorAdd(this->default_render_passes_, render_pass_1);
    
    // todo: info map;
    return render_pass_1;
}

int MarsPlayerRenderFrame::FindMeshRenderPassIndex(std::shared_ptr<MarsMesh> mesh) {
    for (int i=0; i<render_passes_.size(); i++) {
        auto render_pass = render_passes_.at(i);
        if (render_pass->Name().rfind(DEF_NAME, 0) == 0) {
            auto meshes = render_pass->Meshes();
            for (int j=0; j<meshes.size(); j++) {
                if (mesh == meshes.at(j)) {
                    return i;
                }
            }
        }
    }
    
    return -1;
}

std::shared_ptr<RenderPass> MarsPlayerRenderFrame::FindPreviousDefaultRenderPass(const RenderPass* current_pass) {
    RenderPass* final_copy_pass = this->resource_->final_copy_rp.get();
    if (current_pass == final_copy_pass) {
        size_t default_pass_len = default_render_passes_.size();
        if (default_pass_len >= 1) {
            return default_render_passes_[default_pass_len - 1];
        } else {
            return nullptr;
        }
    }
    
    auto default_pass = this->FindPreviousRenderPass(default_render_passes_, current_pass);
    if (!default_pass) {
        int rp_index = this->FindRenderPassIndex(this->render_passes_, current_pass);
        for (int i=rp_index-1; i>=0; i--) {
            auto tmp_render_pass = this->render_passes_[i];
            if (std::find(default_render_passes_.cbegin(), default_render_passes_.cend(), tmp_render_pass) != default_render_passes_.cend()) {
                return tmp_render_pass;
            }
        }
        MLOGE("FindPreviousDefaultRenderPass not find");
        return nullptr;
    } else {
        return default_pass;
    }
}

std::shared_ptr<RenderPass> MarsPlayerRenderFrame::FindPreviousRenderPass(const RenderPass* current_pass) {
    return this->FindPreviousRenderPass(this->render_passes_, current_pass);
}

std::shared_ptr<RenderPass> MarsPlayerRenderFrame::FindPreviousRenderPass(std::vector<std::shared_ptr<RenderPass>> passes, const RenderPass* current_pass) {
    for (int i=0; i<passes.size(); i++) {
        if (passes[i].get() == current_pass) {
            if (i >= 1) {
                return passes[i-1];
            } else {
                return nullptr;
            }
        }
    }
    return nullptr;
}

int MarsPlayerRenderFrame::FindRenderPassIndex(std::vector<std::shared_ptr<RenderPass>> passes, const RenderPass* render_pass) {
    for (int i=0; i<passes.size(); i++) {
        if (passes[i].get() == render_pass) {
            return i;
        }
    }
    return -1;
}

RenderPassColorAttachmentOptions MarsPlayerRenderFrame::GetPRAttachments(std::shared_ptr<RenderPass> pre_render_pass) {
    if (!pre_render_pass) {
        auto tex_a = this->resource_->color_a;
        RenderPassColorAttachmentOptions color_attachment_option(tex_a);
        return color_attachment_option;
    }
    
    auto pre_color_attachment = pre_render_pass->GetColorAttachment()->GetMarsTexture();
    if (pre_color_attachment == this->resource_->color_a) {
        RenderPassColorAttachmentOptions color_b(this->resource_->color_b);
        return color_b;
    } else {
        RenderPassColorAttachmentOptions color_a(this->resource_->color_a);
        return color_a;
    }
    
    // todo: 10.2.90 attachment options;
}

}
