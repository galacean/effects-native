//
//  m_transform.cpp
//  PlayGround
//
//  Created by Zongming Liu on 2022/10/9.
//

#include "m_transform.hpp"
#include "math/math_util.hpp"

namespace mn {

MTransfrom::MTransfrom() : rotation_(0, 0, 0, 1), scale_(1, 1, 1), translation_(0, 0, 0) {
    
}

MTransfrom::~MTransfrom() {
    
}

void MTransfrom::SetTranslation(const Vec3& value) {
    translation_ = value;
}

const Vec3& MTransfrom::Translation() {
    return translation_;
}

void MTransfrom::SetScale(const Vec3& value) {
    scale_ = value;
}

const Vec3& MTransfrom::Scale() {
    return scale_;
}

void MTransfrom::SetRotation(const Quaternion& value) {
    rotation_ = value;
}

void MTransfrom::SetRotation(const Vec3& value) {
    Euler euler(value.m[0], value.m[1], value.m[2]);
    rotation_.SetFromEuler(euler);
}

const Quaternion& MTransfrom::Rotation() {
    return rotation_;
}

Mat4& MTransfrom::Matrix() {
    MathUtil::Mat4Compose(mat_, translation_, rotation_, scale_);
    return mat_;
}

void MTransfrom::SetMatrix(const Mat4& mat) {
    MathUtil::Mat4Decompose(mat, translation_, rotation_, scale_);
    mat_ = mat;
}

void MTransfrom::FromMarsTransform(std::shared_ptr<Transform> transform) {
    transform->GetWorldMatrix(mat_);
    MathUtil::Mat4Decompose(mat_, translation_, rotation_, scale_);
}

}
