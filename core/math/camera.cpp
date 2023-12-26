//
//  camera.cpp
//  PlayGroundIOS
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "camera.hpp"
#include "math/math_util.hpp"
#include "util/log_util.hpp"

namespace mn {

const static Vec3 s_tmp_scale_(1, 1, 1);

Camera::Camera(const char* name) {
    view_ = Mat4::IDENTITY;
    projection_ = Mat4::IDENTITY;
    view_projection_ = Mat4::IDENTITY;
    inv_view_ = Mat4::IDENTITY;
    params_.rotation = std::make_shared<Vec3>();
    name_ = name;
    dirty_ = true;
}

Camera::~Camera() {
    
}

const Mat4& Camera::View() {
    return view_;
}

const Mat4& Camera::Projection() {
    return projection_;
}

const Mat4& Camera::ViewProjection() {
    return view_projection_;
}

const Mat4& Camera::InverseView() {
    return inv_view_;
}

void Camera::GetQuat(Vec4& out) {
    if (params_.quat) {
        out.Set(*params_.quat.get());
        return;
    }
    if (params_.rotation) {
        MathUtil::QuatFromRotation(out, params_.rotation->m[0], params_.rotation->m[1], params_.rotation->m[2]);
        return;
    }
    out.Set(0, 0, 0, 1);
}

void Camera::UpdateMatrix() {
    if (dirty_) {
        MathUtil::Mat4Perspective(projection_, params_.fov, params_.aspect, params_.near, params_.far,
            params_.clip_mode == CAMERA_CLIP_MODE_VERTICAL);
        Vec4 quat;
        GetQuat(quat);
        MathUtil::Mat4FromRotationTranslationScale(inv_view_, quat, params_.position, s_tmp_scale_);
        MathUtil::Mat4Invert(view_, inv_view_);
        MathUtil::Mat4Multiply(view_projection_, projection_, view_);
        dirty_ = false;
    }
}

void Camera::SetParams(std::shared_ptr<CameraParams> params) {
    params_.near = params->near;
    params_.far = params->far;
    params_.fov = params->fov;
    params_.aspect = params->aspect;
    params_.position.Set(params->position);
    params_.clip_mode = params->clip_mode;
    if (params->rotation) {
        params_.rotation = std::make_shared<Vec3>(*params->rotation.get());
    } else {
        params_.rotation.reset();
    }
    if (params->quat) {
        params_.quat = std::make_shared<Vec4>(*params->quat.get());
    } else {
        params_.quat.reset();
    }
    dirty_ = true;
}

float Camera::GetAspect() const {
    return params_.aspect;
}

float Camera::GetPositionZ() const {
    return params_.position.m[2];
}

float Camera::GetFOV() const {
    return params_.fov;
}

}
