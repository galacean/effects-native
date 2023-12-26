//
//  m_entity.hpp
//
//  Created by Zongming Liu on 2022/9/22.
//

#ifndef m_entity_hpp
#define m_entity_hpp

#include <stdio.h>
#include <set>
#include "ri/render/mars_mesh.hpp"
#include "model/common.hpp"
#include "model/renderer/m_transform.hpp"

namespace mn {

struct MSceneState;

enum class MEntityType {
    Mesh = 1,
    Tree,
    Skybox,
    Light,
    Camera
};

class MEntity {
    
public:
    
    MEntity();
    
    virtual void Tick(float dt);
    
    void OnVisibleChange(bool visiable) {
        visiable_ = visiable;
    }
    
    virtual MEntityType GetType() = 0;
    
    virtual void AddToRenderObjectSet(std::vector<std::shared_ptr<MarsMesh>>& render_object_set) {
        
    }

    virtual void UpdateUniformsForScene(const MSceneState& scene_state) {
        
    }
    
    void SetTransform(const MTransfrom& transform);
        
    void SetTranslation(const Vec3& value);
    
    void SetRotation(const Quaternion& value);
    
    void SetRotation(const Vec3& value);

    void SetScale(const Vec3& value);
    
    const Vec3& Translation() {
        return this->transform_.Translation();
    }
    
    const Quaternion& Rotation() {
        return this->transform_.Rotation();
    }
    
    const Vec3& Scale() {
        return this->transform_.Scale();
    }
    
    Mat4& Matrix() {
        return this->transform_.Matrix();
    }
    
    void OnEntityRemoved() {
//        assert(0);
    }
    
    bool Visiable() {
        return visiable_;
    }
    
    void SetVisiable(bool val) {
        visiable_ = val;
    }
    
protected:
    
    bool visiable_;
    
    // todo: transform
    MTransfrom transform_;
    
};

}

#endif /* m_entity_hpp */
