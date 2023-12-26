//
//  bezier_value_getter.cpp
//
//  Created by Zongming Liu on 2022/1/19.
//

#include "bezier_value_getter.hpp"

namespace mn {

BezierValueGetter::BezierValueGetter(path_key_array2d key_array, path_value_array2d value_array, bezier_caps_array2d caps_array, size_t path_length, size_t cap_length) : PathValueGetter(key_array, value_array, path_length) {
    type_ = ValueGetterType::BEZIER_TYPE;
    caps_array_ = caps_array;
    caps_length_ = cap_length;
}

BezierValueGetter::~BezierValueGetter() {
    if (caps_array_) {
        delete [] caps_array_;
    }
}

void BezierValueGetter::CalculateVec(size_t index, float dt, float* ret, size_t length) {
    auto vec0 = this->value_array_[index];
    auto vec1 = this->value_array_[index + 1];
    
    auto out_cap = this->caps_array_[index + index];
    auto in_cap = this->caps_array_[index + index + 1];
    
    float ddt = 1 - dt;
    float a = ddt * ddt * ddt;
    float b = 3 * dt * ddt * ddt;
    float c = 3 * dt * dt * ddt;
    float d = dt * dt * dt;
    for (int i=0; i<length; i++) {
        ret[i] = a * vec0[i] + b * out_cap[i] + c * in_cap[i] + d * vec1[i];
    }
}

}
