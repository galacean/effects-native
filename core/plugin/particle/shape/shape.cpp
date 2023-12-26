//
//  shape.cpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#include <cmath>
#include "shape.hpp"
#include "plugin/particle/shape/sphere.hpp"
#include "plugin/particle/shape/donut.hpp"
#include "plugin/particle/shape/cone.hpp"
#include "util/log_util.hpp"


namespace mn {

std::shared_ptr<Shape> Shape::CreateShape(ParticleShapeOption* shape_option) {
    if (shape_option) {
        std::string shape_type = shape_option->shape;
        ARC_MODE_TYPE arc_mode = CreateArcModeType(shape_option->arc_mode);
        // todo: 字符串直接 == 太危险
        if (shape_type == "None") {
            return std::make_shared<ShapeNone>();
        } else if (shape_type == "Sphere") {
            return std::make_shared<Sphere>(shape_option->arc, arc_mode, shape_option->radius);
        } else if (shape_type == "Donut") {
            return std::make_shared<Donut>(shape_option->arc, arc_mode, shape_option->radius);
        } else if (shape_type == "Hemisphere") {
            return std::make_shared<HemiSpere>(shape_option->arc, arc_mode, shape_option->radius);
        } else if (shape_type == "Cone") {
            return std::make_shared<Cone>(shape_option->arc, arc_mode, shape_option->radius, shape_option->angle);
        } else if (shape_type == "Circle") {
            return std::make_shared<Circle>(shape_option->arc, arc_mode, shape_option->radius);
        } else if (shape_type == "Rectangle") {
            return std::make_shared<RectAngle>(shape_option->arc, arc_mode, shape_option->radius, shape_option->width, shape_option->height);
        } else if (shape_type == "RectangleEdge") {
            return std::make_shared<RectAngleEdge>(shape_option->arc, arc_mode, shape_option->radius, shape_option->width, shape_option->height);
        } else if (shape_type == "Edge") {
            return std::make_shared<Edge>(shape_option->arc, arc_mode, shape_option->radius, shape_option->width);
        } else  {
            MLOGE("create shape type not support");
            assert(0);
        }
        return nullptr;
    } else {
        return nullptr;
    }
}

void Circle::Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) {
    float arc = GetArcAngle(arc_, arc_mode_, opt) * DEG2RAG;
    shape_data.direction.Set(std::cos(arc), std::sin(arc), 0);
    Vec3 position;
    MathUtil::Vec3Dot(shape_data.position, shape_data.direction, this->radius_);

}

void RectAngle::Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) {
    float x = MathUtil::Random(-this->w_, this->w_);
    float y = MathUtil::Random(-this->h_, this->h_);
    shape_data.direction.Set(0.0, 0.0, 1.0);
    shape_data.position.Set(x, y, 0);
}

void Edge::Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) {
    float x = 0.0;
    if (this->arc_mode_ == ARC_MODE_TYPE::ARC_MODE_BURST_SPREAD) {
        x = ((int)opt.brust_index) % ((int) opt.brust_count) / (opt.brust_count - 1);
    } else {
        x = MathUtil::Random(0.0, 1.0);
    }
   
    
    shape_data.direction.Set(0.0, 1.0, 0.0);
    shape_data.position.Set((this->w_ * (x - 0.5)), 0.0, 0.0);

}

void RectAngleEdge::Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) {
    float arc = GetArcAngle(arc_, arc_mode_, opt) * DEG2RAG;
    
    shape_data.direction.Set(std::cos(arc), std::sin(arc), 0);
    
    float r0 = std::atan2(this->h_, this->w_);
    float tan = std::tan(arc);
    
    if (arc < r0) {
        shape_data.position.Set(w_, w_ * tan, 0);
    } else if (arc >= r0 && arc < M_PI - r0) {
        shape_data.position.Set(h_ / tan, h_, 0);
    } else if (arc < M_PI  + r0) {
        shape_data.position.Set(-w_, -w_ * tan, 0);
    } else if (arc < M_PI * 2 - r0) {
        shape_data.position.Set(-h_ / tan, -h_, 0);
    } else {
        shape_data.position.Set(w_, w_ * tan, 0);
    }
}

}
