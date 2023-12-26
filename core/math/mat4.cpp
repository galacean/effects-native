//
//  mat4.cpp
//
//  Created by Zongming Liu on 2021/11/23.
//

#include "mat4.hpp"
#include "math_util.hpp"
#include "util/log_util.hpp"

namespace mn {

Mat4::Mat4() : length_(16) {
    *this = ZERO;
}

Mat4::Mat4(float m11, float m21, float m31, float m41, float m12, float m22, float m32, float m42,
           float m13, float m23, float m33, float m43, float m14, float m24, float m34, float m44) : length_(16){
    Set(m11, m12, m13, m14, m21, m22, m23, m24, m31, m32, m33, m34, m41, m42, m43, m44);
}

Mat4::Mat4(const Mat4 &copy) : length_(16){
    m[0]  = copy.m[0];
    m[1]  = copy.m[1];
    m[2]  = copy.m[2];
    m[3]  = copy.m[3];
    m[4]  = copy.m[4];
    m[5]  = copy.m[5];
    m[6]  = copy.m[6];
    m[7]  = copy.m[7];
    m[8]  = copy.m[8];
    m[9]  = copy.m[9];
    m[10] = copy.m[10];
    m[11] = copy.m[11];
    m[12] = copy.m[12];
    m[13] = copy.m[13];
    m[14] = copy.m[14];
    m[15] = copy.m[15];
}

Mat4::Mat4(float* data) : length_(16) {
    if (data) {
        for (int i=0; i<16; i++) {
            m[i] = *(data + i);
        }
    } else {
        MLOGD("Mat4 Init Error");
    }
}

Mat4::~Mat4() {
    
}

void Mat4::Set(const Mat4 &copy) {
    Set(
        copy.m[0], copy.m[1], copy.m[2], copy.m[3],
        copy.m[4], copy.m[5], copy.m[6], copy.m[7],
        copy.m[8], copy.m[9], copy.m[10], copy.m[11],
        copy.m[12], copy.m[13], copy.m[14], copy.m[15]
    );
}

void Mat4::Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
               float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44) {
    m[0]  = m11;
    m[1]  = m21;
    m[2]  = m31;
    m[3]  = m41;
    m[4]  = m12;
    m[5]  = m22;
    m[6]  = m32;
    m[7]  = m42;
    m[8]  = m13;
    m[9]  = m23;
    m[10] = m33;
    m[11] = m43;
    m[12] = m14;
    m[13] = m24;
    m[14] = m34;
    m[15] = m44;
}

Mat4 Mat4::Clone() const {
    Mat4 mat;
    mat.m[0]  = m[0];
    mat.m[1]  = m[1];
    mat.m[2]  = m[2];
    mat.m[3]  = m[3];
    mat.m[4]  = m[4];
    mat.m[5]  = m[5];
    mat.m[6]  = m[6];
    mat.m[7]  = m[7];
    mat.m[8]  = m[8];
    mat.m[9]  = m[9];
    mat.m[10] = m[10];
    mat.m[11] = m[11];
    mat.m[12] = m[12];
    mat.m[13] = m[13];
    mat.m[14] = m[14];
    mat.m[15] = m[15];

    return mat;
}

void Mat4::Inverse() {
    MathUtil::Mat4Invert(*this, *this);
}

void Mat4::Transpose() {
    MathUtil::Mat4Transpose(*this, *this);
}

void Mat4::Unpack(void* data) {
    float* data_view = (float *)data;
    for (size_t i=0; i<16; i++) {
        m[i] = data_view[i];
    }
}

const Mat4 Mat4::IDENTITY = Mat4(
     1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0);

const Mat4 Mat4::ZERO = Mat4(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0);

}
