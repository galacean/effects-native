//
//  static_value_getter.hpp
//
//  Created by Zongming Liu on 2022/1/12.
//

#ifndef static_value_getter_hpp
#define static_value_getter_hpp

#include <stdio.h>
#include "math/value/value_getter.hpp"

namespace mn {

class StaticValueGetter : public ValueGetter {
    
public:
    
    StaticValueGetter(float value);
    
    StaticValueGetter(float* values, size_t length);
    
    ~StaticValueGetter();
      
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
    
    void ToUniform(KeyFrameMeta& meta, float* ret) override;

private:
    
    float value_;
    
    float* values_;
    
    size_t length_;
    
};

}

#endif /* static_value_getter_hpp */
