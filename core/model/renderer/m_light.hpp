//
//  m_light.hpp
//  PlayGroundIOS
//
//  Created by Zongming Liu on 2022/10/12.
//

#ifndef m_light_hpp
#define m_light_hpp

#include <stdio.h>
#include <vector>
#include "model/common.hpp"
#include "model/renderer/m_entity.hpp"
#include "model/model_vfx_item.hpp"

namespace mn {

enum MLightType {
    DIRECTIONAL,
    POINT,
    SPOT,
    AMBIENT
};

class MLight : public MEntity {
    
public:
    
    MLight();
    
    ~MLight();
    
    MEntityType GetType() override {
        return MEntityType::Light;
    }
    
    void Create(std::shared_ptr<MarsItemLight> light, std::shared_ptr<ModelVFXItem> owner_item);
    
    void Tick(float dt) override;
    
    bool IsPoint() {
        return type_ == MLightType::POINT;
    }
    
    bool IsDirection() {
        return type_ == MLightType::DIRECTIONAL;
    }
    
    bool IsSpot() {
        return type_ == MLightType::SPOT;
    }
    
    bool IsAmbient() {
        return type_ == MLightType::AMBIENT;
    }
    
    const Vec3& GetWorldPosition();
    
    const Vec3& GetPosition();
    
    const Vec3 WorldDireciton();
    
    float Intensity() {
        return intensity_;
    }
    
    float Range() {
        return range_;
    }
    
    float OuterConeAngle() {
        return outer_cone_angle_;
    }
    
    float InnerConeAngle() {
        return inner_cone_angle_;
    }
    
    const Vec3& Color() {
        return color_;
    }
    
    const Vec2& Padding() {
        return padding_;
    }
    
    int GetLightType();
        
private:
    
    MLightType type_;
    
    std::weak_ptr<ModelVFXItem> owner_item_;
    
    Vec3 direction_;
    
    float range_;
    
    Vec3 color_;
    
    float intensity_;
    
    float outer_cone_angle_;
    
    float inner_cone_angle_;
    
    Vec2 padding_;
    
};


class MLightManager {
    
public:
    
    MLightManager();
    
    ~MLightManager();
    
    void Tick(float dt);
    
    std::shared_ptr<MLight> InsertItem(std::shared_ptr<MarsItemLight> light, std::shared_ptr<ModelVFXItem> ower_item);
    
    void InsertLight(std::shared_ptr<MLight> light);
    
    void Remove(std::shared_ptr<MLight> light);
    
    size_t LightCount() {
        return light_list_.size();
    }
    
    const std::vector<std::shared_ptr<MLight>> GetLightList() {
        return light_list_;
    }
    
private:
    
    std::vector<std::shared_ptr<MLight>> light_list_;
    
};





}

#endif /* m_light_hpp */
