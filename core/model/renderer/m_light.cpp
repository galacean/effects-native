//
//  m_light.cpp
//  PlayGroundIOS
//
//  Created by Zongming Liu on 2022/10/12.
//

#include "m_light.hpp"
#include "util/color_utils.hpp"

namespace mn {

MLight::MLight() {
    
}

MLight::~MLight() {
    
}

void MLight::Create(std::shared_ptr<MarsItemLight> light, std::shared_ptr<ModelVFXItem> owner_item) {
    direction_.Set(0, 0, -1);
    range_ = 0;
    outer_cone_angle_ = 0.0;
    inner_cone_angle_ = 0.0;
    padding_.Set(0, 0);
    owner_item_ = owner_item;
    visiable_ = true;
    auto options = light->options;
    
    color_.Set(options->color.m[0], options->color.m[1], options->color.m[2]);
    intensity_ = options->intensity;
    
    if (owner_item) {
        transform_.FromMarsTransform(owner_item->transform_);
    }

    if (options->type == "point") {
        type_ = MLightType::POINT;
        range_ = options->range;
    } else if (options->type == "spot") {
        type_ = MLightType::SPOT;
        range_ = options->range;
        outer_cone_angle_ = options->outer_cone_angle;
        inner_cone_angle_ = options->inner_cone_angle;
    } else if (options->type == "directional") {
        type_ = MLightType::DIRECTIONAL;
    } else {
        type_ = MLightType::AMBIENT;
    }
}

void MLight::Tick(float dt) {
    if (this->owner_item_.lock()) {
        auto owner_item = std::dynamic_pointer_cast<mn::VFXItem>(owner_item_.lock());
        this->transform_.FromMarsTransform(owner_item->transform_);
    }
}

const Vec3& MLight::GetWorldPosition() {
    return this->transform_.Translation();
}

// todo: position 未调用；
const Vec3& MLight::GetPosition() {
    return this->transform_.Translation();
}
    
const Vec3 MLight::WorldDireciton() {
    Vec3 result;
    MathUtil::MultiplyByPointAsVector(this->Matrix(), this->direction_, result);
    return result;
}

int MLight::GetLightType() {
    if (type_ == MLightType::DIRECTIONAL) {
        return 0;
    } else if (type_ == MLightType::POINT) {
        return 1;
    } else if (type_ == MLightType::SPOT) {
        return 2;
    } else {
        return 3;
    }
}

MLightManager::MLightManager() {
    
}

MLightManager::~MLightManager() {
    
}

void MLightManager::Tick(float dt) {
    for (size_t i=0; i<light_list_.size(); i++) {
        const auto& light = light_list_[i];
        light->Tick(dt);
    }
}

std::shared_ptr<MLight> MLightManager::InsertItem(std::shared_ptr<MarsItemLight> light_item, std::shared_ptr<ModelVFXItem> ower_item) {
    std::shared_ptr<MLight> light = std::make_shared<MLight>();
    light->Create(light_item, ower_item);
    light_list_.push_back(light);
    return light;
}

void MLightManager::InsertLight(std::shared_ptr<MLight> light) {
    light_list_.push_back(light);

}

void MLightManager::Remove(std::shared_ptr<MLight> light) {
    auto iter = std::find(light_list_.cbegin(), light_list_.cend(), light);
    if (iter != light_list_.cend()) {
        light_list_.erase(iter);
    }
}

}
