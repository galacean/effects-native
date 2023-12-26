//
//  m_geometry.cpp
//
//  Created by Zongming Liu on 2022/9/16.
//

#include "m_geometry.hpp"

namespace mn {

MGeometry::MGeometry(std::shared_ptr<MarsGeometry> geometry) : geometry_(geometry) {
    attribute_names_ = geometry_->GetAttributeNames();
    MLOGD("MGeometry attribute names size:%ld", attribute_names_.size());
}

MGeometry::~MGeometry() {
    
}

void MGeometry::SetHide(bool hide) {
    
}

bool MGeometry::HasPosition() {
    return this->ExistAttributeName("a_Position");
}

bool MGeometry::HasNormals() {
    return this->ExistAttributeName("a_Normal");
}

bool MGeometry::HasTangents() {
    return this->ExistAttributeName("a_Tangent");
}

bool MGeometry::HasColor() {
    return this->ExistAttributeName("a_Color");
}

bool MGeometry::HasUVCoord(int index) {
    std::string uv("a_UV");
    uv.append(std::to_string(index));
    return this->ExistAttributeName(uv);
}

bool MGeometry::HasJoints() {
    return this->ExistAttributeName("a_Joint1");
}

bool MGeometry::HasWeight() {
    return this->ExistAttributeName("a_Weight1");
}

bool MGeometry::ExistAttributeName(const std::string& name) {
    return std::find(attribute_names_.cbegin(), attribute_names_.cend(), name) != attribute_names_.cend();
}

}
