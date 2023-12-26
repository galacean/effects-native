//
//  shape.hpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#ifndef shape_hpp
#define shape_hpp

#include <stdio.h>
#include <memory>
#include <cmath>
#include "util/constant.hpp"
#include "util/log_util.hpp"
#include "math/math_util.hpp"
#include "plugin/types.h"

namespace mn {

struct ShapeGeneratorOption {
    
    ShapeGeneratorOption() {
        total = 0;
        index = 0;
        brust_count = 0;
        brust_index = 0;
    };
    
    // todo: 数据类型
    float total;
    
    float index;
    
    float brust_index;
    
    float brust_count;
    
};

struct ShapeData {
    
    ShapeData() {};
    
    Vec3 position;
    
    Vec3 direction;
    
};


class Shape {
  
public:
    
    static std::shared_ptr<Shape> CreateShape(ParticleShapeOption* shape_option);
    
    Shape() {};
    
    Shape(float arc, ARC_MODE_TYPE arc_mode, float radius) {
        arc_ = arc;
        arc_mode_ = arc_mode;
        radius_ = radius;
        reverse_direction_ = false;
    }
    
    bool ReverseDirection() {
        return reverse_direction_;
    }
    
    bool AlignSpeedDirection() {
        return align_speed_direction_;
    }
    
    virtual ~Shape() {};
    
    virtual void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) = 0;
    
    virtual float GetHorizontalAngle() {
        return 1.0;
    };

protected:
    
    bool reverse_direction_;
    
    bool align_speed_direction_;
    
    static float GetArcAngle(float arc, int arc_mode, const ShapeGeneratorOption& opt) {
        if (arc_mode == ARC_MODE_RANDOM) {
            arc = MathUtil::Random(0, arc);
        } else if (arc_mode == ARC_MODE_LOOP) {
            int index = (int) opt.index;
            int total = (int) (opt.total + 1);
            float d = index % total;
            arc = arc / (float) opt.total * d;
        } else if (arc_mode == ARC_MODE_PINGPONG) {
            float d = opt.index / (opt.total + 1);
            float i = d - std::floor(d);
            arc = arc * (((int) std::floor(d) % 2) ? (1 - i) : i);
        } else if (arc_mode == ARC_MODE_BURST_SPREAD) {
            arc = arc * (float) opt.brust_index / (float) opt.brust_count;
        } else {
            MLOGD("Get Arc Angle not support arc mode");
        }
        
        return arc;
    }
        
    float arc_;
    
    ARC_MODE_TYPE arc_mode_;
    
    float radius_;
    
};

class ShapeNone : public Shape {
public:
    ShapeNone() : Shape() {}
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override {
        shape_data.direction.Set(0, 0, 0);
        shape_data.position.Set(0, 0, 0);
    }
};

class Circle : public Shape {
    
public:
    
    Circle() : Shape() {
        
    }
    
    Circle(float arc, ARC_MODE_TYPE arc_mode, float radius) : Shape(arc, arc_mode, radius){
        
    }
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;

};


class RectAngle : public Shape {

public:
    RectAngle() : Shape() {
        
    }
    
    RectAngle(float arc, ARC_MODE_TYPE arc_mode, float radius, float width, float height) :
            Shape(arc, arc_mode, radius) {
        w_ = width / 2.0;
        h_ = height / 2.0;
    }
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;
    
private:
    
    float w_;
    
    float h_;
};


class Edge : public Shape {
    
public:
    
    Edge() : Shape() {
        
    }
    
    Edge(float arc, ARC_MODE_TYPE arc_mode, float radius, float width) :
            Shape(arc, arc_mode, radius) {
        w_ = width / 2.0;
    }
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;
    
private:
    
    float w_;
};

class RectAngleEdge : public Shape {
    
public:
    
    RectAngleEdge() : Shape() {
        
    }
    
    RectAngleEdge(float arc, ARC_MODE_TYPE arc_mode, float radius, float width, float height) :
            Shape(arc, arc_mode, radius) {
        w_ = width / 2.0;
        h_ = height / 2.0;
    }
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;
    
private:

    float w_;
    
    float h_;

};

}

#endif /* shape_hpp */
