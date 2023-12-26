//
//  gradient_value_getter.hpp
//
//  Created by Zongming Liu on 2022/2/21.
//

#ifndef gradient_value_getter_hpp
#define gradient_value_getter_hpp

#include <stdio.h>
#include <vector>
#include "math/value/value_getter.hpp"
#include "util/color_utils.hpp"
#include "math/vec4.hpp"

namespace mn {

class GradientValueGetter : public ValueGetter {
  
public:
    
    GradientValueGetter();
    
    ~GradientValueGetter();
    
    void AddColorStop(const ColorStop& color_stop);
      
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
    
private:
#ifdef UNITTEST
public:
#endif
    
    std::vector<ColorStop> stops_;
    
};

}

#endif /* gradient_value_getter_hpp */
