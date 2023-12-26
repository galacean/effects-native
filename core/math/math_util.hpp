//
//  math_util.hpp
//
//  Created by Zongming Liu on 2021/12/14.
//

#ifndef math_util_hpp
#define math_util_hpp

#include <stdio.h>
#include <vector>
#include "math/mat4.hpp"
#include "math/mat3.hpp"
#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/quaternion.hpp"

namespace mn {

class MathUtil {
  
public:
    
    static void Mat4Perspective(Mat4& out, float fovy, float aspect, float near, float far, bool reverse = false);

    static void Mat4FromRotationTranslationScale(Mat4& out, const Vec4& quad, const Vec3& pos, const Vec3& scale);
    
    static void Mat4FromTranslationQuaternRotationScale(Mat4& out, const Vec3& translation, const Quaternion& rotation, const Vec3& scale);
    
    static void Mat4Compose(Mat4& out, const Vec3& translation, const Quaternion& rotation, const Vec3& scale);
    
    static void Mat4Decompose(const Mat4&, Vec3& translation, Quaternion& rotation, Vec3& scale);
    
    static void CopyValueFromeMat4(const Mat4& mat, uint8_t* data);
    
    static float Mat4Determinate(const Mat4& a);
    
    static void GetMat4TR(const Mat4& m4, Vec3* translate, Vec4* quat, Vec3* scaling);

    static void GetMat4TRS(const Mat4& m4, Vec3* translate, Vec4* quat, Vec3* scaling);
    
    static void Mat4Multiply(Mat4& out, const Mat4& a, const Mat4& b);

    static void Mat4Invert(Mat4& out, const Mat4& in);
    
    static void Mat4Transpose(Mat4& out, const Mat4& in);
    
    static void Mat3Multiply(Mat3& out, const Mat3& a, const Mat3& b);
    
    static void QuatFromRotation(Vec4& out, float x, float y, float z);
    
    static void QuatStar(Vec4& out, const Vec4& quat);

    static void RotateByQuat(Vec3& out, const Vec3& a, const Vec4& quat);
    
    static void Vec3MulMat3(Vec3& out, const Vec3& in, const Mat3& mat);

    static void Vec3MulCombine(Vec3& out, const Vec3* a, const Vec3* b);
    
    static void Vec4MulCombine(Vec4& out, const Vec4* a, const Vec4* b);
    
    static void Vec3RotateByMat4(Vec3& out, const Vec3& in, const Mat4& mat);
    
    static void Vec3Cross(Vec3& out, const Vec3& a, const Vec3& b);
    
    static void RotationFromMat3(Vec3& out, const Mat3& mat3);
        
    static void Vec3MulMat4(Vec3& out, const Vec3& in, const Mat4& mat);
    
    static void Mat3FromRotation(Mat3& out, const Vec3& rot);
    
    static void Mat3FromRotationZ(Mat3& out, float rad);

    static void Mat3FromQuat(Mat3& out, const Vec4 quat);
    
    static void Vec3Dot(Vec3& out, const Vec3& in, float b);
    
    static void RotateVec2(Vec2& out, const Vec2& vec2, const float angle);
    
    static bool IsZeroVec3(const Vec3& vec);
    
    static float Random(float min, float max);
    
    static void RandomArrItem(Vec4& out, const std::vector<Vec4>& in);
    
    static float Clamp(float val, float a, float b);
    
    static void Vec3Normolize(Vec3& vec);

    static bool IsZero(float val);
    
    static void MockRandom(bool mock);
    
    static void MultiplyByPointAsVector(const Mat4& matrix, const Vec3& cartesian, Vec3& result);

private:
    
    static void RotationZYXFromMat3(Vec3& out, const Mat3& mat3);
    
    static void Mat3FromRotationZYX(Mat3& out, float x, float y, float z);
    
};

}


#endif /* math_util_hpp */
