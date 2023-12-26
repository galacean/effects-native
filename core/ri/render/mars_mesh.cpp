//
//  mars_mesh.cpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#include "mars_mesh.hpp"
#include "util/log_util.hpp"
#include "ri/render/mars_renderer.hpp"

namespace mn {

MarsMesh::MarsMesh() : name_(""), priority_(0) , hide_(false), renderer_(nullptr) {
    world_matrix_ = Mat4::IDENTITY;
}

MarsMesh::MarsMesh(MarsRenderer *renderer, const std::string& name) : name_(name), priority_(0), hide_(false), renderer_(nullptr) {
    world_matrix_ = Mat4::IDENTITY;
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

MarsMesh::MarsMesh(MarsRenderer *renderer, const std::string& name, int priority, const Mat4& mat) : name_(name), priority_(priority), hide_(false), renderer_(nullptr) {
    world_matrix_ = mat;
    if (renderer) {
        this->AssignRenderer(renderer);
    }
}

MarsMesh::~MarsMesh() {
    DEBUG_MLOGD("MarsMesh Destructor");
}
   
void MarsMesh::AssignRenderer(MarsRenderer *renderer) {
    if (renderer) {
        renderer_ = renderer;
    }
}

void MarsMesh::SetWolrdMatrix(const Mat4& mat4) {
    world_matrix_ = mat4;
}

void MarsMesh::SetMaterial(const MaterialOptions& material_option) {
    material_ = std::make_shared<MarsMaterial>(renderer_, material_option);
}

void MarsMesh::SetMaterial(std::shared_ptr<MarsMaterial> material) {
    material_ = material;
}

void MarsMesh::SetGeometry(std::shared_ptr<MarsGeometry> geometry) {
    geometries_.push_back(geometry);
}

std::shared_ptr<MarsMaterial> MarsMesh::GetMaterial() {
    return material_;
}

// todo: ??意义??
std::shared_ptr<MarsGeometry> MarsMesh::GetGeometry() {
    if (geometries_.size() > 0) {
        return geometries_[0];
    } else {
        return nullptr;
    }
}

void MarsMesh::Destroy() {
    material_.reset();
}

}
