//
//  static_value_getter.cpp
//
//  Created by Zongming Liu on 2022/1/12.
//

#include "util/log_util.hpp"
#include "static_value_getter.hpp"
#include <memory>

namespace mn {

StaticValueGetter::StaticValueGetter(float value) {
    value_ = value;
    type_ = ValueGetterType::STATIC_TYPE;
    length_ = 0;
    values_ = nullptr;
}

StaticValueGetter::StaticValueGetter(float* values, size_t length) {
    value_ = 0;
    values_ = (float *) malloc(length * sizeof(float));
    length_ = length;
    for (size_t i=0; i<length; i++) {
        values_[i] = values[i];
    }
}


StaticValueGetter::~StaticValueGetter() {
    if (values_) {
        free(values_);
    }
}
  
float StaticValueGetter::GetValue(float t) {
    return value_;
}

// hack:color
void StaticValueGetter::GetValues(float t, float* ret, size_t length) {
    if (values_) {
        for (size_t i=0;i<length; i++) {
            ret[i] = values_[i];
        }
    }
}

float StaticValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    return value_ * (t1 - t0);
}

float StaticValueGetter::GetIntegrateByTime(float t0, float t1) {
    return 0.5 * value_ * (t1 * t1 - t0 * t0);
}

// ToUnifom确保传入的参数是4位;
void StaticValueGetter::ToUniform(KeyFrameMeta& meta, float* ret) {
    ret[0] = 0.0;
    ret[1] = value_;
    ret[2] = 0.0;
    ret[3] = 0.0;
}

}
