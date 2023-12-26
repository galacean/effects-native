//
//  random_set_value.hpp
//
//  Created by Zongming Liu on 2022/2/16.
//

#ifndef random_set_value_hpp
#define random_set_value_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "math/vec4.hpp"
#include "math/value/value_getter.hpp"

namespace mn {

class RandomValueGetter : public ValueGetter {
    
public:
    
    RandomValueGetter(float min, float max);
          
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
    
    void ToUniform(KeyFrameMeta& meta, float* ret) override;
    
private:
    
    float min_;
    
    float max_;
    
};

class RandomSetValue : public ValueGetter {
    
public:
    
    RandomSetValue();
    
    ~RandomSetValue();
    
    void AddColor(float r, float g, float b, float a);
      
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
            
    ValueGetterType Type() override {
        return type_;
    };
    
private:
#ifdef UNITTEST
public:
#endif

    std::vector<Vec4> colors_;
    
};

}

#endif /* random_set_value_hpp */
