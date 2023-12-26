//
//  cone.cpp
//
//  Created by Zongming Liu on 2022/3/28.
//

#include "cone.hpp"

namespace mn {

Cone::Cone() : Shape() {
    
}

Cone::Cone(float arc, ARC_MODE_TYPE arc_mode, float radius, float angle) : Shape(arc, arc_mode, radius) {
    angle_ = angle;
}

void Cone::Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) {
    float arc = GetArcAngle(arc_, arc_mode_, opt) * DEG2RAG;
    float x = std::cos(arc) * this->radius_;
    float y = std::sin(arc) * this->radius_;
    
    Vec3 position(x, y, 0);
    float l = std::tan(angle_ * DEG2RAG);
    Vec3 direction;
    MathUtil::Vec3Dot(direction, position, l);
    // todo: ???;
    direction.m[2] += 1;
    
    MathUtil::Vec3Dot(position, position, MathUtil::Random(0.0, 1.0));
    shape_data.position = position;

    MathUtil::Vec3Normolize(direction);
    shape_data.direction = direction;
}

}




