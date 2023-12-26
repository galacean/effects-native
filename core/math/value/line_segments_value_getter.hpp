//
//  line_segments_value_getter.hpp
//
//  Created by Zongming Liu on 2022/1/13.
//

#ifndef line_segments_value_getter_hpp
#define line_segments_value_getter_hpp

#include <stdio.h>
#include "math/value/value_getter.hpp"

/**
 "size": [
   "lines",
   [
     [
       0,
       1
     ],
     [
       0.497,
       0.494
     ],
     [
       1,
       1
     ]
   ]
 ]
 */

namespace mn {

class LineSegmentsValueGetter : public ValueGetter {

public:
    
    LineSegmentsValueGetter(line_arr_2d value, size_t length);
    
    ~LineSegmentsValueGetter();
      
    float GetValue(float t) override;
    
    void GetValues(float t, float* ret, size_t length) override;
    
    float GetIntegrateValue(float t0, float t1, float time_scale) override;
    
    float GetIntegrateByTime(float t0, float t1) override;

    void ToUniform(KeyFrameMeta& meta, float* ret) override;
    
    void ScaleXCoord(float scale) override {
        for (int i = 0; i < length_; i++) {
            key_values_[i][0] = scale * key_values_[i][0];
        }
    }
  
private:
    
    float Integrate(float time, bool by_time, float ts);
    
    line_arr_2d key_values_;
    
    size_t length_;
};

}

#endif /* line_segments_value_getter_hpp */
