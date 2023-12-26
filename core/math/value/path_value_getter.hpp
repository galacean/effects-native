//
//  path_value_getter.hpp
//
//  Created by Zongming Liu on 2022/1/13.
//

#ifndef path_value_getter_hpp
#define path_value_getter_hpp

#include <stdio.h>
#include "math/value/value_getter.hpp"

namespace mn {

class PathValueGetter : public ValueGetter {

public:
    
    PathValueGetter(path_key_array2d key_array, path_value_array2d value_array, size_t length);
    
    ~PathValueGetter();
      
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;
    
protected:
#ifdef UNITTEST
public:
#endif
    
    virtual void CalculateVec(size_t index, float dt, float* ret, size_t length);
    
    size_t length_;
    
    path_key_array2d key_array_;
    
    path_value_array2d value_array_;
    
};

}
#endif /* path_value_getter_hpp */
