//
//  m_entity.cpp
//
//  Created by Zongming Liu on 2022/9/22.
//

#include "m_entity.hpp"

namespace mn {

MEntity::MEntity() { }

void MEntity::Tick(float dt) {
    
}

void MEntity::SetTransform(const MTransfrom& transform) {
    this->transform_ = transform;
}

void MEntity::SetTranslation(const Vec3& value) {
    this->transform_.SetTranslation(value);
}

void MEntity::SetRotation(const Vec3& value) {
    this->transform_.SetRotation(value);
}

void MEntity::SetRotation(const Quaternion& value) {
    this->transform_.SetRotation(value);
}

void MEntity::SetScale(const Vec3& value) {
    this->transform_.SetScale(value);
}

}
