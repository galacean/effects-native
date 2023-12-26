//
//  math_util.cpp
//
//  Created by Zongming Liu on 2021/12/14.
//

#include "math_util.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>
#ifdef BUILD_IOT
#include "mersenne_twister.h"
#endif
#include "util/constant.hpp"

namespace mn {

static const double pi = std::acos(-1);
static const double d2r = pi / 180;
static const double r2d = 180 / pi;
static const float ZERO = 0.00001f;

static bool s_mock_random = false;

void MathUtil::Mat4Perspective(Mat4& out, float fovy, float aspect, float near, float far, bool reverse) {
    float f = 1.0 / std::tan((fovy * d2r) / 2);
    out.m[0] = reverse ? f : f / aspect;
    out.m[1] = 0;
    out.m[2] = 0;
    out.m[3] = 0;
    out.m[4] = 0;
    out.m[5] = reverse ? f * aspect : f;
    out.m[6] = 0;
    out.m[7] = 0;
    out.m[8] = 0;
    out.m[9] = 0;
    out.m[10] = 0;
    out.m[11] = -1.0;
    out.m[12] = 0;
    out.m[13] = 0;
    out.m[14] = 0;
    out.m[15] = 0;

    float nf = 1 / (near - far);
    out.m[10] = (far + near) * nf;
    out.m[14] = 2 * far * near * nf;
}

void MathUtil::Mat4FromRotationTranslationScale(Mat4& out, const Vec4& quad, const Vec3& pos, const Vec3& scale) {
    float x = quad.m[0];
    float y = quad.m[1];
    float z = quad.m[2];
    float w = quad.m[3];
    
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    
    float xx = x * x2;
    float xy = x * y2;
    float xz = x * z2;
    
    float yy = y * y2;
    float yz = y * z2;
    float zz = z * z2;
    
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;
    
    float sx = scale.m[0];
    float sy = scale.m[1];
    float sz = scale.m[2];
    
    out.m[0] = (1 - (yy + zz)) * sx;
    out.m[1] = (xy + wz) * sx;
    out.m[2] = (xz - wy) * sx;
    out.m[3] = 0;
    out.m[4] = (xy - wz) * sy;
    out.m[5] = (1 - (xx + zz)) * sy;
    out.m[6] = (yz + wx) * sy;
    out.m[7] = 0;
    out.m[8] = (xz + wy) * sz;
    out.m[9] = (yz - wx) * sz;
    out.m[10] = (1 - (xx + yy)) * sz;
    out.m[11] = 0;
    out.m[12] = pos.m[0];
    out.m[13] = pos.m[1];
    out.m[14] = pos.m[2];
    out.m[15] = 1;
}

// todo: 和3d中的旋转矩阵实现不太一致；
void MathUtil::Mat4FromTranslationQuaternRotationScale(Mat4& out, const Vec3& translation, const Quaternion& rotation, const Vec3& scale) {
    
    float scaleX = scale.m[0];
    float scaleY = scale.m[1];
    float scaleZ = scale.m[2];

    float x2 = rotation.GetX() * rotation.GetX();
    float xy = rotation.GetX() * rotation.GetY();
    float xz = rotation.GetX() * rotation.GetZ();
    float xw = rotation.GetX() * rotation.GetW();
    float y2 = rotation.GetY() * rotation.GetY();
    float yz = rotation.GetY() * rotation.GetZ();
    float yw = rotation.GetY() * rotation.GetW();
    float z2 = rotation.GetZ() * rotation.GetZ();
    float zw = rotation.GetZ() * rotation.GetW();
    float w2 = rotation.GetW() * rotation.GetW();
    
    float m00 = x2 - y2 - z2 + w2;
    float m01 = 2.0 * (xy - zw);
    float m02 = 2.0 * (xz + yw);

    float m10 = 2.0 * (xy + zw);
    float m11 = -x2 + y2 - z2 + w2;
    float m12 = 2.0 * (yz - xw);

    float m20 = 2.0 * (xz - yw);
    float m21 = 2.0 * (yz + xw);
    float m22 = -x2 - y2 + z2 + w2;
    
    out.m[0] = m00 * scaleX;
    out.m[1] = m10 * scaleX;
    out.m[2] = m20 * scaleX;
    out.m[3] = 0.0;
    out.m[4] = m01 * scaleY;
    out.m[5] = m11 * scaleY;
    out.m[6] = m21 * scaleY;
    out.m[7] = 0.0;
    out.m[8] = m02 * scaleZ;
    out.m[9] = m12 * scaleZ;
    out.m[10] = m22 * scaleZ;
    out.m[11] = 0.0;
    out.m[12] = translation.m[0];
    out.m[13] = translation.m[1];
    out.m[14] = translation.m[2];
    out.m[15] = 1.0;
}

// 从平移、旋转、缩放合成一个新变换矩阵
void MathUtil::Mat4Compose(Mat4& out, const Vec3& translation, const Quaternion& rotation, const Vec3& scale) {
    MathUtil::Mat4FromTranslationQuaternRotationScale(out, translation, rotation, scale);
}

void MathUtil::Mat4Decompose(const Mat4& mat, Vec3& translation, Quaternion& rotation, Vec3& scale) {
    Vec4 quat;
    MathUtil::GetMat4TRS(mat, &translation, &quat, &scale);
    rotation.SetQuaternion(quat);
}

float MathUtil::Mat4Determinate(const Mat4& a) {
    const float a00 = a.m[0],
        a01 = a.m[1],
        a02 = a.m[2],
        a03 = a.m[3];
    const float a10 = a.m[4],
        a11 = a.m[5],
        a12 = a.m[6],
        a13 = a.m[7];
    const float a20 = a.m[8],
        a21 = a.m[9],
        a22 = a.m[10],
        a23 = a.m[11];
    const float a30 = a.m[12],
        a31 = a.m[13],
        a32 = a.m[14],
        a33 = a.m[15];
    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;
    return b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
}

void MathUtil::GetMat4TR(const Mat4& mat4, Vec3* translate, Vec4* quat, Vec3* scaling) {
    float m11 = mat4.m[0];
    float m12 = mat4.m[1];
    float m13 = mat4.m[2];
    float m21 = mat4.m[4];
    float m22 = mat4.m[5];
    float m23 = mat4.m[6];
    float m31 = mat4.m[8];
    float m32 = mat4.m[9];
    float m33 = mat4.m[10];
    
    float matrixRotation[9];
    float quatOut[4] = {0, 0, 0, 1};
    
    if (quat) {
        float is1 = 1 / scaling->m[0];
        float is2 = 1 / scaling->m[1];
        float is3 = 1 / scaling->m[2];
        
        matrixRotation[0] = m11 * is1;
        matrixRotation[1] = m12 * is1;
        matrixRotation[2] = m13 * is1;

        matrixRotation[3] = m21 * is2;
        matrixRotation[4] = m22 * is2;
        matrixRotation[5] = m23 * is2;

        matrixRotation[6] = m31 * is3;
        matrixRotation[7] = m32 * is3;
        matrixRotation[8] = m33 * is3;

        float fTrace = matrixRotation[0] + matrixRotation[4] + matrixRotation[8];
        float fRoot;
        if (fTrace > 0.0) {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = sqrtf(fTrace + 1.0); // 2w
            quatOut[3] = 0.5 * fRoot;
            fRoot = 0.5 / fRoot; // 1/(4w)
            quatOut[0] = (matrixRotation[5] - matrixRotation[7]) * fRoot;
            quatOut[1] = (matrixRotation[6] - matrixRotation[2]) * fRoot;
            quatOut[2] = (matrixRotation[1] - matrixRotation[3]) * fRoot;
        } else {
            // |w| <= 1/2
            int i = 0;
            if (matrixRotation[4] > matrixRotation[0]) i = 1;
            if (matrixRotation[8] > matrixRotation[i * 3 + i]) i = 2;
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;
            fRoot = sqrtf(matrixRotation[i * 3 + i] - matrixRotation[j * 3 + j] - matrixRotation[k * 3 + k] + 1.0);
            quatOut[i] = 0.5 * fRoot;
            fRoot = 0.5 / fRoot;
            quatOut[3] = (matrixRotation[j * 3 + k] - matrixRotation[k * 3 + j]) * fRoot;
            quatOut[j] = (matrixRotation[j * 3 + i] + matrixRotation[i * 3 + j]) * fRoot;
            quatOut[k] = (matrixRotation[k * 3 + i] + matrixRotation[i * 3 + k]) * fRoot;
        }

        quat->m[0] = quatOut[0];
        quat->m[1] = quatOut[1];
        quat->m[2] = quatOut[2];
        quat->m[3] = quatOut[3];
    }

    if (translate) {
        translate->m[0] = mat4.m[12];
        translate->m[1] = mat4.m[13];
        translate->m[2] = mat4.m[14];
    }
}

void MathUtil::GetMat4TRS(const Mat4& mat4, Vec3* translate, Vec4* quat, Vec3* scaling) {
    float m11 = mat4.m[0];
    float m12 = mat4.m[1];
    float m13 = mat4.m[2];
    float m21 = mat4.m[4];
    float m22 = mat4.m[5];
    float m23 = mat4.m[6];
    float m31 = mat4.m[8];
    float m32 = mat4.m[9];
    float m33 = mat4.m[10];
    const float det = Mat4Determinate(mat4);
    Vec3 sc;
    sc.m[0] = hypot(hypot(m11, m12), m13);
    sc.m[1] = hypot(hypot(m21, m22), m23);
    sc.m[2] = hypot(hypot(m31, m32), m33);
    if (det < 0) {
        sc.m[0] = -sc.m[0];
    }
    if (scaling) {
        scaling->Set(sc);
    }
    return GetMat4TR(mat4, translate, quat, &sc);
}

// todo:反的？
void MathUtil::Mat4Multiply(Mat4& out, const Mat4& a, const Mat4& b) {
    float a00 = a.m[0];
    float a01 = a.m[1];
    float a02 = a.m[2];
    float a03 = a.m[3];
    float a10 = a.m[4];
    float a11 = a.m[5];
    float a12 = a.m[6];
    float a13 = a.m[7];
    float a20 = a.m[8];
    float a21 = a.m[9];
    float a22 = a.m[10];
    float a23 = a.m[11];
    float a30 = a.m[12];
    float a31 = a.m[13];
    float a32 = a.m[14];
    float a33 = a.m[15];
    
    for (int i=0; i<4; i++) {
        
        float b0 = b.m[i*4 + 0];
        float b1 = b.m[i*4 + 1];
        float b2 = b.m[i*4 + 2];
        float b3 = b.m[i*4 + 3];
        
        out.m[i*4 + 0] = b0 * a00 + b1 * a10 + b2 * a20 + b3 * a30;
        out.m[i*4 + 1] = b0 * a01 + b1 * a11 + b2 * a21 + b3 * a31;
        out.m[i*4 + 2] = b0 * a02 + b1 * a12 + b2 * a22 + b3 * a32;
        out.m[i*4 + 3] = b0 * a03 + b1 * a13 + b2 * a23 + b3 * a33;
    }
}


void MathUtil::CopyValueFromeMat4(const Mat4& mat, uint8_t* data) {
    float* data_view = (float *)data;
    data_view[0] = mat.m[0];
    data_view[1] = mat.m[1];
    data_view[2] = mat.m[2];
    data_view[3] = mat.m[3];
    data_view[4] = mat.m[4];
    data_view[5] = mat.m[5];
    data_view[6] = mat.m[6];
    data_view[7] = mat.m[7];
    data_view[8] = mat.m[8];
    data_view[9] = mat.m[9];
    data_view[10] = mat.m[10];
    data_view[11] = mat.m[11];
    data_view[12] = mat.m[12];
    data_view[13] = mat.m[13];
    data_view[14] = mat.m[14];
    data_view[15] = mat.m[15];
}


void MathUtil::Mat4Invert(Mat4& out, const Mat4& in) {
    float a00 = in.m[0];
    float a01 = in.m[1];
    float a02 = in.m[2];
    float a03 = in.m[3];
    float a10 = in.m[4];
    float a11 = in.m[5];
    float a12 = in.m[6];
    float a13 = in.m[7];
    float a20 = in.m[8];
    float a21 = in.m[9];
    float a22 = in.m[10];
    float a23 = in.m[11];
    float a30 = in.m[12];
    float a31 = in.m[13];
    float a32 = in.m[14];
    float a33 = in.m[15];
    
    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;
    
    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    
    det = 1.0 / det;
    out.m[0] = (a11 * b11 - a12 * b10 + a13 * b09) * det;
    out.m[1] = (a02 * b10 - a01 * b11 - a03 * b09) * det;
    out.m[2] = (a31 * b05 - a32 * b04 + a33 * b03) * det;
    out.m[3] = (a22 * b04 - a21 * b05 - a23 * b03) * det;
    out.m[4] = (a12 * b08 - a10 * b11 - a13 * b07) * det;
    out.m[5] = (a00 * b11 - a02 * b08 + a03 * b07) * det;
    out.m[6] = (a32 * b02 - a30 * b05 - a33 * b01) * det;
    out.m[7] = (a20 * b05 - a22 * b02 + a23 * b01) * det;
    out.m[8] = (a10 * b10 - a11 * b08 + a13 * b06) * det;
    out.m[9] = (a01 * b08 - a00 * b10 - a03 * b06) * det;
    out.m[10] = (a30 * b04 - a31 * b02 + a33 * b00) * det;
    out.m[11] = (a21 * b02 - a20 * b04 - a23 * b00) * det;
    out.m[12] = (a11 * b07 - a10 * b09 - a12 * b06) * det;
    out.m[13] = (a00 * b09 - a01 * b07 + a02 * b06) * det;
    out.m[14] = (a31 * b01 - a30 * b03 - a32 * b00) * det;
    out.m[15] = (a20 * b03 - a21 * b01 + a22 * b00) * det;
}

void MathUtil::Mat4Transpose(Mat4& out, const Mat4& in) {
    float matrix1 = in.m[1];
    float matrix2 = in.m[2];
    float matrix3 = in.m[3];
    float matrix6 = in.m[6];
    float matrix7 = in.m[7];
    float matrix11 = in.m[11];

    out.m[0] = in.m[0];
    out.m[1] = in.m[4];
    out.m[2] = in.m[8];
    out.m[3] = in.m[12];
    out.m[4] = matrix1;
    out.m[5] = in.m[5];
    out.m[6] = in.m[9];
    out.m[7] = in.m[13];
    out.m[8] = matrix2;
    out.m[9] = matrix6;
    out.m[10] = in.m[10];
    out.m[11] = in.m[14];
    out.m[12] = matrix3;
    out.m[13] = matrix7;
    out.m[14] = matrix11;
    out.m[15] = in.m[15];
}

// a * b
void MathUtil::Mat3Multiply(Mat3& out, const Mat3& a, const Mat3& b) {
    float a00 = a.m[0];
    float a01 = a.m[1];
    float a02 = a.m[2];
    
    float a10 = a.m[3];
    float a11 = a.m[4];
    float a12 = a.m[5];
 
    float a20 = a.m[6];
    float a21 = a.m[7];
    float a22 = a.m[8];
    
    float b00 = b.m[0];
    float b01 = b.m[1];
    float b02 = b.m[2];
    
    float b10 = b.m[3];
    float b11 = b.m[4];
    float b12 = b.m[5];
 
    float b20 = b.m[6];
    float b21 = b.m[7];
    float b22 = b.m[8];
    
    out.m[ 0 ] = b00 * a00 + b01 * a10 + b02 * a20;
    out.m[ 1 ] = b00 * a01 + b01 * a11 + b02 * a21;
    out.m[ 2 ] = b00 * a02 + b01 * a12 + b02 * a22;
    out.m[ 3 ] = b10 * a00 + b11 * a10 + b12 * a20;
    out.m[ 4 ] = b10 * a01 + b11 * a11 + b12 * a21;
    out.m[ 5 ] = b10 * a02 + b11 * a12 + b12 * a22;
    out.m[ 6 ] = b20 * a00 + b21 * a10 + b22 * a20;
    out.m[ 7 ] = b20 * a01 + b21 * a11 + b22 * a21;
    out.m[ 8 ] = b20 * a02 + b21 * a12 + b22 * a22;
}

void MathUtil::QuatFromRotation(Vec4& out, float x, float y, float z) {
    float c1 = std::cos((x * d2r) / 2);
    float c2 = std::cos((y * d2r) / 2);
    float c3 = std::cos((z * d2r) / 2);
    
    float s1 = std::sin((x * d2r) / 2);
    float s2 = std::sin((y * d2r) / 2);
    float s3 = std::sin((z * d2r) / 2);
    
    out.m[0] = s1 * c2 * c3 - c1 * s2 * s3;
    out.m[1] = c1 * s2 * c3 + s1 * c2 * s3;
    out.m[2] = c1 * c2 * s3 - s1 * s2 * c3;
    out.m[3] = c1 * c2 * c3 + s1 * s2 * s3;
}

void MathUtil::QuatStar(Vec4& out, const Vec4& quat) {
    float x = quat.m[0], y = quat.m[1], z = quat.m[2], w = quat.m[3];
    out.Set(-x, -y, -z, w);
}

void MathUtil::RotateByQuat(Vec3& out, const Vec3& a, const Vec4& quat) {
    float x = quat.m[0], y = quat.m[1], z = quat.m[2], w = quat.m[3];
    Vec3 qvec(x, y, z);
    Vec3 uv;
    Vec3Cross(uv, qvec, a);
    Vec3 uuv;
    Vec3Cross(uuv, qvec, uv);
    Vec3Dot(uuv, uuv, 2);
    Vec3Dot(uv, uv, 2 * w);
    uuv.Add(uv);
    out.Set(a);
    out.Add(uuv);
}

void MathUtil::Vec3MulMat3(Vec3& out, const Vec3& in, const Mat3& mat) {
    float x = in.m[0];
    float y = in.m[1];
    float z = in.m[2];
    
    out.m[ 0 ] = x * mat.m[ 0 ] + y * mat.m[ 3 ] + z * mat.m[ 6 ];
    out.m[ 1 ] = x * mat.m[ 1 ] + y * mat.m[ 4 ] + z * mat.m[ 7 ];
    out.m[ 2 ] = x * mat.m[ 2 ] + y * mat.m[ 5 ] + z * mat.m[ 8 ];
}

void MathUtil::Vec3MulMat4(Vec3& out, const Vec3& in, const Mat4& mat) {
    float x = in.m[0];
    float y = in.m[1];
    float z = in.m[2];
    
    float w = mat.m[ 3 ] * x + mat.m[ 7 ] * y + mat.m[ 11 ] * z + mat.m[ 15 ];
    if (w == 0) {
        w = 1;
    }
    out.m[ 0 ] = (mat.m[ 0 ] * x + mat.m[ 4 ] * y + mat.m[ 8 ] * z + mat.m[ 12 ]) / w;
    out.m[ 1 ] = (mat.m[ 1 ] * x + mat.m[ 5 ] * y + mat.m[ 9 ] * z + mat.m[ 13 ]) / w;
    out.m[ 2 ] = (mat.m[ 2 ] * x + mat.m[ 6 ] * y + mat.m[ 10 ] * z + mat.m[ 14 ]) / w;
}

void MathUtil::Vec3MulCombine(Vec3& out, const Vec3* a, const Vec3* b) {
    if (a && b) {
        for (int i=0; i<3; i++) {
            out.m[i] = a->m[i] * b->m[i];
        }
    } else if (a) {
        for (int i=0; i<3; i++) {
            out.m[i] = a->m[i];
        }
    } else if (b) {
        for (int i=0; i<3; i++) {
            out.m[i] = b->m[i];
        }
    }
}

void MathUtil::Vec4MulCombine(Vec4& out, const Vec4* a, const Vec4* b) {
    if (a && b) {
        for (int i=0; i<4; i++) {
            out.m[i] = a->m[i] * b->m[i];
        }
    } else if (a) {
        for (int i=0; i<4; i++) {
            out.m[i] = a->m[i];
        }
    } else if (b) {
        for (int i=0; i<4; i++) {
            out.m[i] = b->m[i];
        }
    }
}

void MathUtil::Vec3RotateByMat4(Vec3& out, const Vec3& in, const Mat4& mat) {
    float x = in.m[0];
    float y = in.m[1];
    float z = in.m[2];
    
    float w = (mat.m[ 3 ] * x + mat.m[ 7 ] * y + mat.m[ 11 ] * z + mat.m[ 15 ]);
    if (w == 0) {
        w = 1;
    }
    out.m[ 0 ] = (mat.m[ 0 ] * x + mat.m[ 4 ] * y + mat.m[ 8 ] * z) / w;
    out.m[ 1 ] = (mat.m[ 1 ] * x + mat.m[ 5 ] * y + mat.m[ 9 ] * z) / w;
    out.m[ 2 ] = (mat.m[ 2 ] * x + mat.m[ 6 ] * y + mat.m[ 10 ] * z) / w;
}

void MathUtil::Vec3Cross(Vec3& out, const Vec3& a, const Vec3& b) {
    float ax = a.m[0], ay = a.m[1], az = a.m[2];
    float bx = b.m[0], by = b.m[1], bz = b.m[2];
    out.m[0] = ay * bz - az * by;
    out.m[1] = az * bx - ax * bz;
    out.m[2] = ax * by - ay * bx;
}

void MathUtil::RotationFromMat3(Vec3& out, const Mat3& mat3) {
    RotationZYXFromMat3(out, mat3);
}

void MathUtil::RotationZYXFromMat3(Vec3& out, const Mat3& mat3) {
    const float* const te = mat3.m;
    const float m11 = te[0], m12 = te[3], m13 = te[6];
    const float m21 = te[1], m22 = te[4], m23 = te[7];
    const float m31 = te[2], m32 = te[5], m33 = te[8];

    out.m[1] = asin(-Clamp(m31, -1, 1)) * r2d;

    if (fabs(m31) < 0.9999999) {
        out.m[0] = atan2(m32, m33) * r2d;
        out.m[2] = atan2(m21, m11) * r2d;
    } else {
        out.m[0] = 0;
        out.m[2] = atan2(-m12, m22) * r2d;
    }
}

void MathUtil::Mat3FromRotation(Mat3& out, const Vec3& rot) {
    MathUtil::Mat3FromRotationZYX(out, -rot.m[0], -rot.m[1], -rot.m[2]);
}

void MathUtil::Mat3FromRotationZ(Mat3& out, float rad) {
    float s = std::sin(rad);
    float c = std::cos(rad);
    
    out.m[ 0 ] = c;
    out.m[ 1 ] = s;
    out.m[ 2 ] = 0;
    out.m[ 3 ] = -s;
    out.m[ 4 ] = c;
    out.m[ 5 ] = 0;
    out.m[ 6 ] = 0;
    out.m[ 7 ] = 0;
    out.m[ 8 ] = 1;
}

void MathUtil::Mat3FromQuat(Mat3& out, const Vec4 quat) {
    float x = quat.m[0],
    y = quat.m[1],
    z = quat.m[2],
    w = quat.m[3];
    float x2 = x + x;
    float y2 = y + y;
    float z2 = z + z;
    float xx = x * x2;
    float yx = y * x2;
    float yy = y * y2;
    float zx = z * x2;
    float zy = z * y2;
    float zz = z * z2;
    float wx = w * x2;
    float wy = w * y2;
    float wz = w * z2;
    out.m[0] = 1 - yy - zz;
    out.m[3] = yx - wz;
    out.m[6] = zx + wy;
    out.m[1] = yx + wz;
    out.m[4] = 1 - xx - zz;
    out.m[7] = zy - wx;
    out.m[2] = zx - wy;
    out.m[5] = zy + wx;
    out.m[8] = 1 - xx - yy;
}

void MathUtil::Mat3FromRotationZYX(Mat3& out, float x, float y, float z) {
    float cosRx = std::cos(x * DEG2RAG);
    float cosRy = std::cos(y * DEG2RAG);
    float cosRz = std::cos(z * DEG2RAG);
    float sinRx = std::sin(x * DEG2RAG);
    float sinRy = std::sin(y * DEG2RAG);
    float sinRz = std::sin(z * DEG2RAG);

    out.m[ 0 ] = cosRy * cosRz;
    out.m[ 1 ] = cosRy * sinRz;
    out.m[ 2 ] = -sinRy;
    out.m[ 3 ] = -cosRx * sinRz + sinRx * sinRy * cosRz;
    out.m[ 4 ] = cosRx * cosRz + sinRx * sinRy * sinRz;
    out.m[ 5 ] = sinRx * cosRy;
    out.m[ 6 ] = sinRz * sinRx + cosRx * sinRy * cosRz;
    out.m[ 7 ] = -sinRx * cosRz + cosRx * sinRy * sinRz;
    out.m[ 8 ] = cosRx * cosRy;
}

void MathUtil::Vec3Dot(Vec3& out, const Vec3& in, float b) {
    size_t len = out.Length();
    for (size_t i=0; i<len; i++) {
        out.m[i] = in.m[i] * b;
    }
}

void MathUtil::RotateVec2(Vec2& out, const Vec2& vec2, const float angle) {
    const float c = cos(angle);
    const float s = sin(angle);
    const float x = vec2.m[0];
    const float y = vec2.m[1];
    out.m[0] = c * x + s * y;
    out.m[1] = -s * x + c * y;
}

bool MathUtil::IsZeroVec3(const Vec3& vec) {
    static float s_number_epsilon = powf(2, -32);
    for (int i = 0; i < vec.Length(); i++) {
        if (fabsf(vec.m[i]) > s_number_epsilon) {
            return false;
        }
    }
    return true;
}

#ifdef BUILD_IOT
static MTRandom mt_random;
#endif
float MathUtil::Random(float min, float max) {
//    std::srand((unsigned int)std::time(nullptr));
#ifdef BUILD_IOT
    double seed = mt_random.NextFloat();
#else
    double seed = (double) std::rand() / (RAND_MAX);
#endif
    if (s_mock_random) {
        seed = 0.5f;
    }
    return min + seed * (max - min);
}

void MathUtil::RandomArrItem(Vec4& out, const std::vector<Vec4>& in) {
    int index = (int) std::floor(MathUtil::Random(0.0, 1.0) * in.size());
    if (index >= in.size()) {
        index = in.size() - 1;
    } else if (index < 0) {
        index = 0;
    }
    out = in[index];
}

float MathUtil::Clamp(float val, float a, float b) {
    if (val < a) {
        return a;
    }
    if (val > b) {
        return b;
    }
    return val;
}

void MathUtil::Vec3Normolize(Vec3& vec) {
    size_t len = vec.Length();
    float sum = 0.0;
    for (size_t i=0; i<len; i++) {
        sum += vec.m[i] * vec.m[i];
    }
    
    sum = std::sqrt(sum);
    if (sum != 0.0) {
        vec.Divide(sum);
    }
}

bool MathUtil::IsZero(float val) {
    return (val > -ZERO) && (val < ZERO);
}

void MathUtil::MockRandom(bool mock) {
    s_mock_random = mock;
}

void MathUtil::MultiplyByPointAsVector(const Mat4& matrix, const Vec3& cartesian, Vec3& result) {
    float vX = cartesian.m[0];
    float vY = cartesian.m[1];
    float vZ = cartesian.m[2];
    
    float x = matrix.m[0] * vX + matrix.m[4] * vY + matrix.m[8] * vZ;
    float y = matrix.m[1] * vX + matrix.m[5] * vY + matrix.m[9] * vZ;
    float z = matrix.m[2] * vX + matrix.m[6] * vY + matrix.m[10] * vZ;
    
    result.m[0] = x;
    result.m[1] = y;
    result.m[2] = z;
}


}
