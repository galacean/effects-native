//
//  bezier_value_getter.hpp
//
//  Created by Zongming Liu on 2022/1/19.
//

#ifndef bezier_value_getter_hpp
#define bezier_value_getter_hpp

#include <stdio.h>
#include "math/value/path_value_getter.hpp"

namespace mn {

class BezierValueGetter : public PathValueGetter {
    
public:
    
    BezierValueGetter(path_key_array2d key_array, path_value_array2d value_array, bezier_caps_array2d caps_array, size_t path_length, size_t cap_length);
    
    ~BezierValueGetter();
    
    void CalculateVec(size_t index, float dt, float* ret, size_t length) override;

private:
    
    size_t caps_length_;

    bezier_caps_array2d caps_array_;
};

}


#endif /* bezier_value_getter_hpp */
