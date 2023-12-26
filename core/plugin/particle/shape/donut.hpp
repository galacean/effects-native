//
//  donut.hpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#ifndef donut_hpp
#define donut_hpp

#include <stdio.h>
#include "plugin/particle/shape/shape.hpp"

namespace mn {

class Donut : public Shape {
    
public:
    
    Donut();
    
    Donut(float arc, ARC_MODE_TYPE arc_mode, float radius);
    
    ~Donut();
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;

private:
    
};

}

#endif /* donut_hpp */
