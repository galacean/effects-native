//
//  path_value_getter.cpp
//
//  Created by Zongming Liu on 2022/1/13.
//

#include "util/log_util.hpp"
#include "path_value_getter.hpp"
#include "math/value/value_getter_utils.hpp"

namespace mn {

PathValueGetter::PathValueGetter(path_key_array2d key_array, path_value_array2d value_array, size_t length) {
    key_array_ = key_array;
    value_array_ = value_array;
    length_ = length;
    type_ = ValueGetterType::PATH_TYPE;
};

PathValueGetter::~PathValueGetter() {
    if (key_array_) {
        delete [] key_array_;
    }
    
    if (value_array_) {
        delete [] value_array_;
    }
};
  
// pathSegment只有GetValues方法。
float PathValueGetter::GetValue(float t) {
    MLOGE("PathValueGetter GetValue not supported");
    return 1.0;
}

void PathValueGetter::GetValues(float time, float* ret, size_t length) {
    if (time <= key_array_[0][0]) {
        for (size_t i=0; i<length; i++) {
            ret[i] = value_array_[0][i];
        }
        return ;
    }

    size_t end = length_ - 1;
    for (size_t i=0; i<end; i++) {
        auto key0 = key_array_[i];
        auto key1 = key_array_[i+1];
        
        if (key0[0] <= time && key1[0] >= time) {
            float dist = key1[1] - key0[1];
            float dt = 0.0;
            if (dist == 0) {
                dt = (time - key0[0]) / (key1[0] - key0[0]);
            } else {
                float curve_val = ValueGetterUtils::CurveValueEvaluate(time, key0, key1);
                dt = (curve_val - key0[1]) / dist;
            }
            
            return this->CalculateVec(i, dt, ret, length);
        }
    }

    for (size_t i=0; i<length; i++) {
        ret[i] = value_array_[end][i];
    }
    
    return ;
}

float PathValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    return 1.0;
}

float PathValueGetter::GetIntegrateByTime(float t0, float t1) {
    return 1.0;
}

void PathValueGetter::CalculateVec(size_t index, float dt, float* ret, size_t length) {
    auto value0 = value_array_[index];
    auto value1 = value_array_[index + 1];
    
    for (size_t i=0; i<length; i++) {
        ret[i] = value0[i] * (1 - dt) + value1[i] * dt;
    }
}

}
