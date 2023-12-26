//
//  linear_value_getter.hpp
//
//  Created by Zongming Liu on 2021/12/20.
//

#ifndef linear_value_getter_hpp
#define linear_value_getter_hpp

#include <stdio.h>
#include "math/value/value_getter.hpp"

namespace mn {

class LinearValueGetter : public ValueGetter {
    
public:

    LinearValueGetter(float min, float max);
    
    ~LinearValueGetter();
        
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
        
    void ToUniform(KeyFrameMeta& meta, float* ret) override;
    
    void ScaleXCoord(float scale) override {
        x_coord_ = scale;
    }

private:
    
    float min_;
    
    float max_;
    
    float x_coord_;
    
};

}

#endif /* linear_value_getter_hpp */
