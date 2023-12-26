//
//  linear_value_getter.cpp
//
//  Created by Zongming Liu on 2021/12/20.
//

#include "linear_value_getter.hpp"
#include "util/log_util.hpp"
#include "math/value/value_getter_utils.hpp"

namespace mn {

LinearValueGetter::LinearValueGetter(float min, float max) {
    min_ = min;
    max_ = max;
    x_coord_ = 1;
    type_ = ValueGetterType::LINEAR_TYPE;
}

LinearValueGetter::~LinearValueGetter() {
    MLOGD("~LinearValueGetter");
}

float LinearValueGetter::GetValue(float t) {
    t /= x_coord_;
    return min_ * (1 - t) + max_ * t;
}

void LinearValueGetter::GetValues(float t, float* ret, size_t length) {
    MLOGE("LinearValueGetter GetValues not supported");
}

float LinearValueGetter::GetIntegrateValue(float t0, float t1, float time_scale) {
    if (!time_scale) {
        time_scale = 1;
    }
    float min = this->min_;
    float max = this->max_;
    float ts = this->x_coord_ * time_scale;
    float v1 = min + (max - min) * (t1 / ts);
    float v0 = min + (max - min) * (t0 / ts);
    return ((v1 + min) * t1 - (v0 + min) * t0) / 2;

}

float LinearValueGetter::GetIntegrateByTime(float t0, float t1) {
    return ValueGetterUtils::LineSegIntegrateByTime(t1, 0, this->x_coord_, this->min_, this->max_) - ValueGetterUtils::LineSegIntegrateByTime(t0, 0, this->x_coord_, this->min_, this->max_);
}

void LinearValueGetter::ToUniform(KeyFrameMeta& meta, float* ret) {
    ret[0] = 1.0;
    ret[1] = min_;
    ret[2] = max_;
    ret[3] = x_coord_;
}

}
