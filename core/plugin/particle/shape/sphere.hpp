//
//  sphere.hpp
//
//  Created by Zongming Liu on 2022/1/20.
//

#ifndef sphere_hpp
#define sphere_hpp

#include <stdio.h>
#include "plugin/particle/shape/shape.hpp"

namespace mn {

class Sphere : public Shape {
  
public:
    
    Sphere();
    
    Sphere(float arc, ARC_MODE_TYPE arc_mode, float radius);
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;
        
    float GetHorizontalAngle() override;
        
};

class HemiSpere : public Sphere {
    
public:
    
    HemiSpere();
    
    HemiSpere(float arc, ARC_MODE_TYPE arc_mode, float radius);

    float GetHorizontalAngle() override;

};

}




#endif /* sphere_hpp */
