//
//  m_mesh.hpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#ifndef m_mesh_hpp
#define m_mesh_hpp

#include <stdio.h>
#include <string>
#include "ri/render/mars_mesh.hpp"

#include "model/renderer/m_entity.hpp"
#include "model/renderer/m_geometry.hpp"
#include "model/renderer/material/m_material_base.hpp"
#include "model/renderer/animation/m_animation.hpp"
#include "model/renderer/animation/m_skin.hpp"
#include "model/model_vfx_item.hpp"
#include "model/renderer/m_skybox.hpp"

namespace mn {

class MSceneManager;

class MMesh;

struct MSceneState;

class MPrimitive {
    
public:

    MPrimitive(MarsRenderer* renderer);
    
    ~MPrimitive();
    
    void Create(std::shared_ptr<MarsPrimitiveOptions> options, MMesh* parent_mesh);
    
    void Build(size_t light_count, std::shared_ptr<MSkyBox> sky_box);
    
    void AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set);
    
    void UpdateUniformsForScene(const Mat4& world_matrix, const Mat4& normal_matrix, const MSceneState& scene_state);
    
private:
    
    void InnerUpdateUniformsByScene(const MSceneState& scene_state);
    
    void UpdateUniformsByAnimation(const Mat4& world_matrix, const Mat4& normal_matrix);
    
    void GetFeatureList(std::vector<std::string>& feature_list ,size_t light_count, std::shared_ptr<MSkyBox> sky_box);
    
    std::shared_ptr<MGeometry> geometry_;
    
    std::shared_ptr<MMaterialBase> material_;
    
    std::shared_ptr<MarsMaterial> mri_material_;
        
    std::shared_ptr<MarsMesh> mri_mesh_;

    std::string name_;

    int mri_priority_;
    
    std::shared_ptr<MSkin> skin_;
    
    std::shared_ptr<MAnimTexture> joint_matrix_texture_;
    
    std::shared_ptr<MAnimTexture> joint_normal_matrix_texture_;
    
    uint8_t* u_joint_matrix_data_;
    
    uint8_t* u_joint_normal_matrix_data_;
    
    MarsRenderer* renderer_;
    
};

class MMesh : public MEntity {
    
public:
    
    MMesh(MarsRenderer* renderer);
    
    ~MMesh();
    
    MEntityType GetType() override {
        return MEntityType::Mesh;
    }
        
    void Create(std::shared_ptr<MarsItemMesh> mesh_option, std::shared_ptr<ModelVFXItem> vfx_item, std::shared_ptr<TreeVFXItem> parent_item);
    
    void Build(size_t light_count, std::shared_ptr<MSkyBox> sky_box);
    
    void Tick(float delta_seconds) override;
    
    void UpdateParentItem(std::shared_ptr<TreeVFXItem> parent_item);
    
    void AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set) override;
    
    void UpdateUniformsForScene(const MSceneState& scene_state) override;
    
    std::shared_ptr<MSkin> Skin() {
        return skin_;
    }
    
    const std::string& Name() {
        return name_;
    }
    
    int Priority() {
        return priority_;
    }
    
private:
    
    const Mat4& WorldMatrix();
        
    void InnerUpdateUniformByScene(const MSceneState& scene_state);
    
    void UpdateUniformsForAnimation();
        
    std::vector<std::shared_ptr<MPrimitive>> primitivies_;
    
    std::weak_ptr<ModelVFXItem> owner_item_;
    
//    std::weak_ptr<TreeVFXItem> parent_item_;
    
    std::shared_ptr<MSkin> skin_;
    
    std::string name_;
    
    MarsRenderer* renderer_;
    
    int priority_ = 0;
        
    float angle_ = 0.0f;
        
};

}

#endif /* m_mesh_hpp */
