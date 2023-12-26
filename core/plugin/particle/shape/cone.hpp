//
//  cone.hpp
//
//  Created by Zongming Liu on 2022/3/28.
//

#ifndef cone_hpp
#define cone_hpp

#include <stdio.h>
#include "plugin/particle/shape/shape.hpp"

namespace mn {

class Cone : public Shape {
    
public:
    
    Cone();
    
    Cone(float arc, ARC_MODE_TYPE arc_mode, float radius, float angle);
    
    void Generate(const ShapeGeneratorOption& opt, ShapeData& shape_data) override;
    
private:
    float angle_ = 0;
    
};

}


#endif /* cone_hpp */
