//
//  vec3.cpp
//
//  Created by Zongming Liu on 2021/10/25.
//

#include "vec3.hpp"
#include <string>

namespace mn {

Vec3::Vec3() {
    length_ = 3;
    m[0] = 0;
    m[1] = 0;
    m[2] = 0;
}

Vec3::Vec3(const Vec3& vec3) {
    length_ = vec3.length_;
    m[0] = vec3.m[0];
    m[1] = vec3.m[1];
    m[2] = vec3.m[2];
}

Vec3::Vec3(float x, float y, float z) {
    length_ = 3;
    m[0] = x;
    m[1] = y;
    m[2] = z;
}

void Vec3::Set(const Vec3& vec3) {
    m[0] = vec3.m[0];
    m[1] = vec3.m[1];
    m[2] = vec3.m[2];
}

void Vec3::Set(float x, float y, float z) {
    m[0] = x;
    m[1] = y;
    m[2] = z;
}


void Vec3::Add(const Vec3& vec) {
    m[0] += vec.m[0];
    m[1] += vec.m[1];
    m[2] += vec.m[2];
}

void Vec3::Multi(const Vec3& vec) {
    m[0] *= vec.m[0];
    m[1] *= vec.m[1];
    m[2] *= vec.m[2];
}

void Vec3::Copy(float *ret) {
    ret[0] = m[0];
    ret[1] = m[1];
    ret[2] = m[2];
}

void Vec3::Divide(float v) {
    m[0] /= v;
    m[1] /= v;
    m[2] /= v;
}


Vec3::~Vec3() {
    
}

}
