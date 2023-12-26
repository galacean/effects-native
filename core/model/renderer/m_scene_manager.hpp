//
//  m_scene_manager.hpp
//
//  Created by Zongming Liu on 2022/9/22.
//

#ifndef m_scene_manager_hpp
#define m_scene_manager_hpp

#include <stdio.h>
#include "model/renderer/m_entity.hpp"
#include "model/renderer/m_mesh.hpp"
#include "model/renderer/animation/m_animation.hpp"
#include "model/model_vfx_item.hpp"
#include "model/renderer/m_light.hpp"
#include "model/renderer/m_camera.hpp"
#include "model/renderer/m_skybox.hpp"

namespace mn {

struct MSceneState {
    int delta_sec;
    Mat4 view_matrix;
    Mat4 projection_matrix;
    Mat4 view_projection_matrix;
    
    std::shared_ptr<MMCamera> camera;
    Vec3 camera_position;
    
    // todo: for shadow;
    // Mat4 light_view_projection_matrix;
    
    std::vector<std::shared_ptr<MLight>> lights;
    std::shared_ptr<MSkyBox> sky_box;
};

struct MSceneOptions {
    MarsRenderer* renderer;
    std::string composition_name;
};

class MSceneManager {
    
public:

    MSceneManager();
    
    ~MSceneManager();
    
    void Initial(const MSceneOptions& option);
        
    void SetCamera(std::shared_ptr<Camera> camera);
    
    std::shared_ptr<Camera> GetCamera() {
        return camera_;
    }
    
    void Build();
    
    void UpdateDefaultCamera(std::shared_ptr<CameraParams> camera_params);
    
    void Tick(float delta_time);
    
    void OnUpdateRenderObject(RenderFrame* render_frame);

    std::shared_ptr<MEntity> CreateItem(std::shared_ptr<ModelVFXItem> vfx_item);
    
    std::shared_ptr<MMesh> CreateMesh(std::shared_ptr<MarsItemMesh> item_mesh, std::shared_ptr<ModelVFXItem> owner_item, std::shared_ptr<TreeVFXItem> vfx_item);
    
    std::shared_ptr<MSkyBox> CreateSkybox(std::shared_ptr<MarsItemSkybox> item_skybox, std::shared_ptr<ModelVFXItem> owner_item);
    
    std::shared_ptr<MLight> CreateLight(std::shared_ptr<MarsItemLight> item_light, std::shared_ptr<ModelVFXItem> owner_item);
    
    void CreateCamera(std::shared_ptr<MarsItemCamera> item_camera, std::shared_ptr<ModelVFXItem> owner_item);
        
private:
    
    void Clean();
    
    size_t GetLightCount() {
        return this->light_manager_->LightCount();
    }
    
    std::string name_;
    
    MarsRenderer* renderer_;
 
    std::vector<std::shared_ptr<MEntity>> item_list_;
    
    std::vector<std::shared_ptr<MMesh>> mesh_list_;
    
    std::vector<std::shared_ptr<MarsMesh>> render_mesh_set_;
    
    std::shared_ptr<Camera> camera_;
    
    // todo: lightManager/CameraManager/ShadowManager;
    std::shared_ptr<MAnimationManager> animation_manager_;
    
    std::shared_ptr<MLightManager> light_manager_;
    
    std::shared_ptr<MCameraManager> camera_manager_;
    
    // for skybox;
    std::shared_ptr<MSkyBox> sky_box_;
    
    std::shared_ptr<MarsTexture> brdf_lut_;
    
};

}

#endif /* m_scene_manager_hpp */
