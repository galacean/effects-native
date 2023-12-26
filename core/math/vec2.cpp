//
//  vec2.cpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#include "vec2.hpp"

namespace mn {

Vec2::Vec2() {
    length_ = 2;
    m[0] = 0;
    m[1] = 0;
}

Vec2::Vec2(const Vec2& vec) {
    length_ = vec.length_;
    m[0] = vec.m[0];
    m[1] = vec.m[1];
}

Vec2::Vec2(float x, float y) {
    length_ = 2;
    m[0] = x;
    m[1] = y;
}

Vec2::~Vec2() {
    
}

void Vec2::Set(float x, float y) {
    m[0] = x;
    m[1] = y;
}

void Vec2::Set(const Vec2& vec2) {
    m[0] = vec2.m[0];
    m[1] = vec2.m[1];
}

void Vec2::Add(const Vec2& vec) {
    m[0] += vec.m[0];
    m[1] += vec.m[1];
}

void Vec2::Multi(const Vec2& vec) {
    m[0] *= vec.m[0];
    m[1] *= vec.m[1];
}

void Vec2::Divide(float v) {
    m[0] /= v;
    m[1] /= v;
}


}
