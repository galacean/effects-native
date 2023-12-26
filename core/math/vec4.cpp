//
//  vec4.cpp
//
//  Created by Zongming Liu on 2021/11/24.
//

#include <string>
#include "vec4.hpp"
#include "util/log_util.hpp"

namespace mn {

Vec4::Vec4() {
    length_ = 4;
    m[0] = 0;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
}

Vec4::Vec4(const Vec4& vec) {
    length_ = vec.length_;
    m[0] = vec.m[0];
    m[1] = vec.m[1];
    m[2] = vec.m[2];
    m[3] = vec.m[3];
}


Vec4::Vec4(float x, float y, float z, float w) {
    length_ = 4;
    m[0] = x;
    m[1] = y;
    m[2] = z;
    m[3] = w;
}

void Vec4::Set(const Vec4& vec4) {
    m[0] = vec4.m[0];
    m[1] = vec4.m[1];
    m[2] = vec4.m[2];
    m[3] = vec4.m[3];
}

void Vec4::Set(float x, float y, float z, float w) {
    m[0] = x;
    m[1] = y;
    m[2] = z;
    m[3] = w;
}

void Vec4::Add(const Vec4& vec) {
    m[0] += vec.m[0];
    m[1] += vec.m[1];
    m[2] += vec.m[2];
    m[3] += vec.m[3];
}

void Vec4::Multi(const Vec4& vec) {
    m[0] *= vec.m[0];
    m[1] *= vec.m[1];
    m[2] *= vec.m[2];
    m[3] *= vec.m[3];
}

void Vec4::Copy(float *ret) {
    ret[0] = m[0];
    ret[1] = m[1];
    ret[2] = m[2];
    ret[3] = m[3];
}

void Vec4::Divide(float v) {
    m[0] /= v;
    m[1] /= v;
    m[2] /= v;
    m[3] /= v;
}

Vec4::~Vec4() {

}

}
