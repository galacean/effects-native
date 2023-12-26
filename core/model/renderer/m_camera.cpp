//
//  m_camera.cpp
//
//  Created by Zongming Liu on 2022/10/12.
//

#include <algorithm>
#include "m_camera.hpp"
#include "math/math_util.hpp"

namespace mn {

MMCamera::MMCamera() {
    
}

MMCamera::~MMCamera() {
    
}

void MMCamera::Create(std::shared_ptr<MarsItemCamera> camera_option, int width, int height, std::shared_ptr<ModelVFXItem> owner_item) {
    this->owner_item_ = owner_item;
    this->reversed_ = true;
    auto options = camera_option->content.options;
    this->near_ = options->near;
    this->far_ = options->far;
    this->aspect_ = options->aspect;
    this->Update();
}

void MMCamera::Update() {
    auto owner_item = this->owner_item_.lock();
    if (owner_item) {
        // todo: owner_item;
    }
    
    MathUtil::Mat4Perspective(projection_matrix_, fovy_, aspect_, near_, far_, this->reversed_);
    view_matrix_ = transform_.Matrix();
    view_matrix_.Inverse();
}

void MMCamera::SetEye(const Vec3& pos) {
    transform_.SetTranslation(pos);
}

const Vec3& MMCamera::GetEye() {
    return transform_.Translation();
}

void MMCamera::GetNewProjectionMatrix(Mat4& out) {
    MathUtil::Mat4Perspective(out, std::min(fovy_ * 1.4, 140.0), aspect_ , near_, far_, this->reversed_);
}

MCameraManager::MCameraManager() {
    
}

MCameraManager::~MCameraManager() {
    
}

void MCameraManager::Initial(int width, int height) {
    default_camera_ = std::make_shared<MMCamera>();
    default_camera_->aspect_ = (float) width / height;
    default_camera_->near_ = 0.1;
    default_camera_->far_ = 1000.0;
    default_camera_->fovy_ = 60;
    default_camera_->reversed_ = true;
    Vec3 eye_pos(0, 0, 8);
    default_camera_->SetEye(eye_pos);
    default_camera_->Update();
}

std::shared_ptr<MMCamera> MCameraManager::Insert(std::shared_ptr<MarsItemCamera> camera, std::shared_ptr<ModelVFXItem> owner_item) {
    std::shared_ptr<MMCamera> m_camera = std::make_shared<MMCamera>();
    m_camera->Create(camera, width_, height_, owner_item);
    this->camera_list_.push_back(m_camera);
    return m_camera;
}

void MCameraManager::Remove(std::shared_ptr<MMCamera> camera) {
    for (auto iter = camera_list_.cbegin(); iter != camera_list_.cend(); ) {
        if (*iter == camera) {
            camera_list_.erase(iter);
            break;
        } else {
            ++iter;
        }
    }
}

void MCameraManager::UpdateDefaultCamera(std::shared_ptr<CameraParams> camera_params) {
    this->default_camera_->aspect_ = camera_params->aspect;
    this->default_camera_->near_ = camera_params->near;
    this->default_camera_->far_ = camera_params->far;
    this->default_camera_->fovy_ = camera_params->fov;
    
    if (camera_params->rotation) {
        this->default_camera_->SetRotation(*camera_params->rotation.get());
    }
    this->default_camera_->SetTranslation(camera_params->position);
    this->default_camera_->Update();
}



}
