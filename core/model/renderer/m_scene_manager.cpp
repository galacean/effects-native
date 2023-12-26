//
//  m_scene_manager.cpp
//
//  Created by Zongming Liu on 2022/9/22.
//

#include "m_scene_manager.hpp"
#include "math/math_util.hpp"

namespace mn {

MSceneManager::MSceneManager() {
    
}

MSceneManager::~MSceneManager() {
    
}

void MSceneManager::Initial(const MSceneOptions& option) {
    this->Clean();
    renderer_ = option.renderer;
    name_ = option.composition_name;
    
    int width = renderer_->GetSurfaceWidth();
    int height = renderer_->GetSurfaceHeight();
    
    camera_manager_->Initial(width, height);
}

void MSceneManager::SetCamera(std::shared_ptr<Camera> camera) {
    camera_ = camera;
}

std::shared_ptr<MEntity> MSceneManager::CreateItem(std::shared_ptr<ModelVFXItem> vfx_item) {
    std::shared_ptr<MarsItemModelBase> mars_item = vfx_item->options_;
    int type = mars_item->type;
    if (type == ITEM_CONTENT_TYPE_MODEL_MESH) {
        std::shared_ptr<TreeVFXItem> parent_item;
        if (vfx_item->parent && vfx_item->parent->GetType().compare("tree") == 0) {
            parent_item = std::static_pointer_cast<TreeVFXItem>(vfx_item->parent);
        }
        return this->CreateMesh(std::static_pointer_cast<MarsItemMesh>(mars_item), vfx_item, parent_item);
    } else if (type == ITEM_CONTENT_TYPE_MODEL_LIGHT) {
        std::shared_ptr<MarsItemLight> light_option = std::static_pointer_cast<MarsItemLight>(mars_item);
        return this->CreateLight(light_option, vfx_item);
//    } else if (type == "camera") {
//        assert(0);
//        std::shared_ptr<MarsItemCamera> camera_option = std::static_pointer_cast<MarsItemCamera>(item_options);
//        this->CreateCamera(camera_option, nullptr);
    } else if (type == ITEM_CONTENT_TYPE_MODEL_SKYBOX) {
        std::shared_ptr<MarsItemSkybox> skybox_option = std::static_pointer_cast<MarsItemSkybox>(mars_item);
        return this->CreateSkybox(skybox_option, vfx_item);
    }
    
    assert(0);
    return nullptr;
}

void MSceneManager::Build() {
    for (auto mesh: mesh_list_) {
        mesh->Build(this->GetLightCount(), sky_box_);
    }
}

void MSceneManager::UpdateDefaultCamera(std::shared_ptr<CameraParams> camera_params) {
    // todo: update camera;
    camera_manager_->UpdateDefaultCamera(camera_params);
}

void MSceneManager::Tick(float delta_time) {
    MSceneState scene_state;
    
    const auto& default_camera = this->camera_manager_->DefaultCamera();
    
    const Mat4& project_matrix = default_camera->GetProjectionMatrix();
    const Mat4& view_matrix = default_camera->GetViewMatrix();
    
    Mat4 view_projection_matrix;
    MathUtil::Mat4Multiply(view_projection_matrix, project_matrix, view_matrix);
    
    scene_state.view_projection_matrix = view_projection_matrix;
    scene_state.view_matrix = view_matrix;
    scene_state.camera = default_camera;
    scene_state.delta_sec = delta_time;
    scene_state.camera_position = default_camera->GetEye();
    scene_state.sky_box = sky_box_;
    
    scene_state.lights = this->light_manager_->GetLightList();
    this->light_manager_->Tick(delta_time);
    this->render_mesh_set_.clear();
    for (auto item : item_list_) {
        item->Tick(delta_time);
        item->UpdateUniformsForScene(scene_state);
        item->AddToRenderObjectSet(this->render_mesh_set_);
    }
}

void MSceneManager::OnUpdateRenderObject(RenderFrame* render_frame) {
    // todo: shadow
    for (auto mesh : render_mesh_set_) {
        render_frame->AddMeshToDefaultRendererPass(mesh);
    }
}

std::shared_ptr<MMesh> MSceneManager::CreateMesh(std::shared_ptr<MarsItemMesh> item_mesh, std::shared_ptr<ModelVFXItem> owner_item, std::shared_ptr<TreeVFXItem> vfx_item) {
    std::shared_ptr<MMesh> mesh = std::make_shared<MMesh>(renderer_);
    mesh->Create(item_mesh, owner_item, vfx_item);
    this->item_list_.push_back(mesh);
    this->mesh_list_.push_back(mesh);
    return mesh;
}

std::shared_ptr<MSkyBox> MSceneManager::CreateSkybox(std::shared_ptr<MarsItemSkybox> item_skybox, std::shared_ptr<ModelVFXItem> owner_item) {
    // scene manager对应一个sky box;
    std::shared_ptr<MarsTexture> brdf_lut = MSkyBox::CreateBrdfLutTexture(renderer_);
    sky_box_ = std::make_shared<MSkyBox>(renderer_);
    sky_box_->Create(item_skybox, brdf_lut, owner_item);
    return sky_box_;
}

std::shared_ptr<MLight> MSceneManager::CreateLight(std::shared_ptr<MarsItemLight> item_light, std::shared_ptr<ModelVFXItem> owner_item) {
    auto light = this->light_manager_->InsertItem(item_light, owner_item);
    this->item_list_.push_back(light);
    return light;
    
}

void MSceneManager::CreateCamera(std::shared_ptr<MarsItemCamera> item_camera, std::shared_ptr<ModelVFXItem> owner_item) {
    camera_manager_->Insert(item_camera, owner_item);
}

void MSceneManager::Clean() {
    item_list_.clear();
    mesh_list_.clear();
    render_mesh_set_.clear();
    
    animation_manager_ = std::make_shared<MAnimationManager>();
    light_manager_ = std::make_shared<MLightManager>();
    camera_manager_ = std::make_shared<MCameraManager>();
}

}
