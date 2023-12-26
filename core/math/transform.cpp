//
//  transform.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/4/12.
//

#include "transform.h"
#include "math/math_util.hpp"
#include "util/log_util.hpp"
#include "util/util.hpp"

namespace mn {

Transform::Transform(TransformData* opt) : matrix_(Mat4::IDENTITY) {
    position_ = new Vec3();
    quat_ = new Vec4(0, 0, 0, 1);
    scale_ = new Vec3(1, 1, 1);
    parent_mat_ = new Mat4(Mat4::IDENTITY);
    world_mat_ = new Mat4(Mat4::IDENTITY);
    if (opt) {
        SetTransform(opt);
    }
}

Transform::~Transform() {
    MN_SAFE_DELETE(parent_mat_);
    MN_SAFE_DELETE(world_mat_);
    MN_SAFE_DELETE(world_scale_);
}

void Transform::SetTransform(TransformData* opt, bool reverse_euler) {
    if (opt->position_) {
        float* position = opt->position_->m;
        SetPosition(position[0], position[1], position[2]);
    }
    if (opt->quat_) {
        float* quat = opt->quat_->m;
        SetQuat(quat[0], quat[1], quat[2], quat[3]);
    } else if (((Transform*) opt)->rotation_) {
        float* rotation = ((Transform*) opt)->rotation_->m;
        float mul = reverse_euler ? -1 : 1;
        SetRotation(rotation[0] * mul, rotation[1] * mul, rotation[2] * mul);
    }
    if (opt->scale_) {
        float* scale = opt->scale_->m;
        SetScale(scale[0], scale[1], scale[2]);
    }
}

void Transform::SetPosition(float x, float y, float z) {
    this->Invalid();
    position_->Set(x, y, z);
}

void Transform::SetPosition(const Vec3 pos) {
    this->Invalid();
    position_->Set(pos);
}


void Transform::SetQuat(float x, float y, float z, float w) {
    this->Invalid();
    quat_->Set(x, y, z, w);
}


void Transform::SetQuat(const Vec4& quat) {
    this->Invalid();
    quat_->Set(quat);
}

void Transform::SetRotation(float x, float y, float z) {
    this->Invalid();
    MathUtil::QuatFromRotation(*quat_, x, y, z);
    MathUtil::QuatStar(*quat_, *quat_);
}

void Transform::SetRotation(const Vec3& rot) {
    this->Invalid();
    MathUtil::QuatFromRotation(*quat_, rot.m[0], rot.m[1], rot.m[2]);
    MathUtil::QuatStar(*quat_, *quat_);
}

void Transform::SetScale(float x, float y, float z) {
    this->Invalid();
    scale_->Set(x, y, z);
}

void Transform::SetScale(const Vec3& scale) {
    this->Invalid();
    scale_->Set(scale);
}

void Transform::Invalid() {
    dirty_ = true;
    MN_SAFE_DELETE(parent_mat_);
    
    for(size_t i=0; i<children_.size(); i++) {
        children_[i]->Invalid();
    }
}

void Transform::Mul(Transform* t) {
    Mat4& this_mat = GetMatrix();
    Mat4& t_mat = t->GetMatrix();
    Mat4 temp;
    MathUtil::Mat4Multiply(temp, this_mat, t_mat);
    FromMat4(temp);
}

void Transform::Update() {
    MathUtil::Mat4FromRotationTranslationScale(matrix_, *quat_, *position_, *scale_);
    dirty_ = false;
}

void Transform::FromMat4(const Mat4& m4, Vec3* scale) {
    dirty_ = true;
    if (scale) {
        MathUtil::GetMat4TR(m4, position_, quat_, scale);
        MathUtil::Vec3MulCombine(*scale_, scale, nullptr);
    } else {
        MathUtil::GetMat4TRS(m4, position_, quat_, scale_);
    }
}

void Transform::SetParentTransform(std::shared_ptr<Transform> t) {
    std::shared_ptr<Transform> parent_transform = pt_.lock();
    if (parent_transform) {
        Utils::VectorRemove(parent_transform->children_, shared_from_this());
    }
    
    pt_ = t;
    parent_transform = pt_.lock();
    
    if (parent_transform) {
        Utils::VectorAdd(parent_transform->children_, shared_from_this());
    }
    
    if (t.get() == this) {
        MLOGE("Transform set self to parent transform");
    }
    
    Invalid();
}

Mat4& Transform::GetMatrix() {
    if (dirty_) {
        Update();
    }
    return matrix_;
}

void Transform::GetWorldScale(Vec3& out) {
    std::shared_ptr<Transform> parent_transform = pt_.lock();
    if (parent_transform) {
        parent_transform->GetWorldScale(out);
        MathUtil::Vec3MulCombine(out, scale_, &out);
    } else {
        MathUtil::Vec3MulCombine(out, scale_, nullptr);
    }
}

void Transform::GetWorldMatrix(Mat4& mat4) {
    std::shared_ptr<Transform> parent = this->pt_.lock();
    if (parent && (!parent_mat_ || parent->Dirty())) {
        if (!parent_mat_) {
            parent_mat_ = new Mat4(Mat4::IDENTITY);
        }
        parent->GetWorldMatrix(*parent_mat_);
        if (!world_scale_) {
            world_scale_ = new Vec3();
        }
        GetWorldScale(*world_scale_);
        this->dirty_ = true;
    }
    
    if (parent_mat_) {
        if (this->dirty_) {
            MathUtil::Mat4Multiply(*world_mat_, *parent_mat_, GetMatrix());
        }
        
        mat4 = *world_mat_;
    } else {
        mat4 = GetMatrix();
    }
}

void Transform::GetWorldTRS(Vec3* translate, Vec4* quat, Vec3* scale) {
    Mat4 m;
    GetWorldMatrix(m);
    Vec3* scaling = world_scale_;
    if (!scaling) {
        scaling = scale_;
    }
    MathUtil::GetMat4TR(m, translate, quat, scaling);
    if (scale) {
        MathUtil::Vec3MulCombine(*scale, scaling, nullptr);
    }
}

void Transform::GetRotation(Vec3& out) {
    Vec4 q;
    MathUtil::QuatStar(q, *quat_);
    Mat3 m3;
    MathUtil::Mat3FromQuat(m3, q);
    MathUtil::RotationFromMat3(out, m3);
}

bool Transform::Dirty() {
    std::shared_ptr<Transform> parent = this->pt_.lock();
    if (parent && parent->Dirty()) {
        return true;
    }
    
    return this->dirty_;
}


}
