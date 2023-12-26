//
//  random_set_value.cpp
//
//  Created by Zongming Liu on 2022/2/16.
//

#include <vector>
#include <assert.h>
#include "math/math_util.hpp"
#include "random_value_getter.hpp"

namespace mn {

RandomValueGetter::RandomValueGetter(float min, float max):min_(min), max_(max) {};

float RandomValueGetter::GetValue(float t) {
    return MathUtil::Random(min_, max_);
}

void RandomValueGetter::GetValues(float t, float* ret, size_t length) {

}

float RandomValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    return 1.0;
}

float RandomValueGetter::GetIntegrateByTime(float t0, float t1) {
    return 1.0;
}

void RandomValueGetter::ToUniform(KeyFrameMeta& meta, float* ret) {
    ret[0] = 4.0;
    ret[1] = min_;
    ret[2] = max_;
    ret[3] = 0.0;
}

RandomSetValue::RandomSetValue() {
    // todo: color parse;
};

RandomSetValue::~RandomSetValue() {
    
};


void RandomSetValue::AddColor(float r, float g, float b, float a) {
    Vec4 color(r, g, b, a);
    colors_.push_back(color);
}
  
float RandomSetValue::GetValue(float t) {
    return 1.0;
}

void RandomSetValue::GetValues(float t, float* ret, size_t length) {
    size_t color_len = colors_.size();
    size_t index = MathUtil::Random(0, 1) * color_len;
    Vec4& vec = colors_[index];

    if (length != vec.Length()) {
        assert(false);
        return;
    }

    for (size_t i=0; i<length; i++) {
        ret[i] = vec.m[i];
    }
    
    return;
}

float RandomSetValue::GetIntegrateValue(float t0, float t1, float time_scale) {
    return 1.0;
}

float RandomSetValue::GetIntegrateByTime(float t0, float t1) {
    return 1.0;
}

}
