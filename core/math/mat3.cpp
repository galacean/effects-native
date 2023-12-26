//
//  mat3.cpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#include "mat3.hpp"

namespace mn {

Mat3::Mat3() : length_(9){
    *this = ZERO;
}

Mat3::Mat3(const Mat3& copy) : length_(9){
    m[0] = copy.m[0];
    m[1] = copy.m[1];
    m[2] = copy.m[2];
    m[3] = copy.m[3];
    m[4] = copy.m[4];
    m[5] = copy.m[5];
    m[6] = copy.m[6];
    m[7] = copy.m[7];
    m[8] = copy.m[8];
}

Mat3::Mat3(float m11, float m21, float m31, float m12, float m22, float m32, float m13, float m23, float m33) : length_(9) {
    Set(m11, m12, m13, m21, m22, m23, m31, m32, m33);
}

Mat3::~Mat3() {
    
}

void Mat3::Set(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33) {
    m[0] = m11;
    m[1] = m21;
    m[2] = m31;
    m[3] = m12;
    m[4] = m22;
    m[5] = m32;
    m[6] = m13;
    m[7] = m23;
    m[8] = m33;
}

const Mat3 Mat3::IDENTITY = Mat3(
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0);

const Mat3 Mat3::ZERO = Mat3(
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0);

}
