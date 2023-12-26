//
//  value_getter.hpp
//
//  Created by Zongming Liu on 2021/12/17.
//

#ifndef value_getter_hpp
#define value_getter_hpp

#include <stdio.h>
#include <vector>
#include "math/vec4.hpp"

#define PATH_KEY_ROW 4
#define PATH_VALUE_ROW 3
#define CURVE_ROW 4
#define BEZIER_CAP_ROW 3

#define LINE_ROW 2

using line_arr_2d = float(*)[LINE_ROW];
using curve_array_2d = float(*)[CURVE_ROW];
using path_key_array2d = float(*)[PATH_KEY_ROW];
using path_value_array2d = float(*)[PATH_VALUE_ROW];
using bezier_caps_array2d = float(*)[BEZIER_CAP_ROW];

const size_t CURVE_PRO_TIME = 0;
const size_t CURVE_PRO_VALUE = 1;
const size_t CURVE_PRO_IN_TANGENT = 2;
const size_t CURVE_PRO_OUT_TANGENT = 3;

namespace mn {

enum ValueGetterType {
    STATIC_TYPE = 0,
    LINEAR_TYPE,
    LINEAR_SEGMENT_TYPE,
    CURVE_TYPE,
    CONSTANT_TYPE,
    PATH_TYPE,
    BEZIER_TYPE,
    GRADIENT,
};

struct KeyFrameMeta {
    
    KeyFrameMeta() {
        index = 0.0;
        max = 0.0;
        line_seg_count = 0;
        curve_count = 0;
    }
        
    std::vector<Vec4> frame_datas;
    
    float index;
    
    float max;
    
    int line_seg_count;
    
    int curve_count;
    
};

class ValueGetter {
    
public:
    
    ValueGetter() {};
    
    virtual ~ValueGetter() {};
      
    virtual float GetValue(float t) = 0;
    
    virtual void GetValues(float t, float* ret, size_t length) = 0;
    
    virtual float GetIntegrateValue(float t0, float t1, float time_scale) = 0;
    
    virtual float GetIntegrateByTime(float t0, float t1) = 0;
    
    virtual void ToUniform(KeyFrameMeta& meta, float* ret) {

    };
    
    virtual void map() {};
    
    virtual void ScaleXCoord(float scale) {}
    
    virtual ValueGetterType Type() {
        return type_;
    };
    
protected:
    
    ValueGetterType type_;
    
};

}

#endif /* value_getter_hpp */
