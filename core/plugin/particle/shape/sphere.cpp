//
//  sphere.cpp
//
//  Created by Zongming Liu on 2022/1/20.
//

#include "sphere.hpp"
#include "math/math_util.hpp"


namespace mn {

Sphere::Sphere(): Shape(0, ARC_MODE_RANDOM, 0) {
    
}

Sphere::Sphere(float arc, ARC_MODE_TYPE arc_mode, float radius) : Shape(arc, arc_mode, radius) {
    
}

float Sphere::GetHorizontalAngle() {
    return MathUtil::Random(-90.0, 90.0);
}

void Sphere::Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) {
    float rz = GetArcAngle(arc_, arc_mode_, opt) * DEG2RAG;
    float rh = this->GetHorizontalAngle() * DEG2RAG;
    
    Vec3 position(std::cos(rh), 0, std::sin(rh));
    Mat3 rot_z_mat;
    MathUtil::Mat3FromRotationZ(rot_z_mat, rz);
    MathUtil::Vec3MulMat3(position, position, rot_z_mat);

    shape_data.direction.Set(position);

    size_t len = position.Length();
    for (size_t i=0; i<len; i++) {
        shape_data.position.m[i] = position.m[i] * radius_;
    }
}


HemiSpere::HemiSpere() : Sphere() {
    
}

HemiSpere::HemiSpere(float arc, ARC_MODE_TYPE arc_mode, float radius) : Sphere(arc, arc_mode, radius) {
    
}

float HemiSpere::GetHorizontalAngle() {
    return MathUtil::Random(0.0, 90.0);
}


}
