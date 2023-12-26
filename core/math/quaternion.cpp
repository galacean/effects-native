//
//  quaternion.cpp
//
//  Created by Zongming Liu on 2022/10/9.
//

#include "quaternion.hpp"
#include <cmath>

namespace mn {

Quaternion::Quaternion() {
    m[0] = 0;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
    m[0] = x;
    m[1] = y;
    m[2] = z;
    m[3] = w;
}

/**
 将欧拉角转化为四元素
 */
void Quaternion::SetFromEuler(const Euler& euler) {
    float x = euler.GetX();
    float y = euler.GetY();
    float z = euler.GetZ();
    EulerOrder order = euler.GetOrder();
    
    float c1 = std::cos(x / 2);
    float c2 = std::cos(y / 2);
    float c3 = std::cos(z / 2);
    
    float s1 = std::sin(x / 2);
    float s2 = std::sin(y / 2);
    float s3 = std::sin(z / 2);
    
    switch (order) {
        case EulerOrder::XYZ: {
            m[0] = s1 * c2 * c3 + c1 * s2 * s3;
            m[1] = c1 * s2 * c3 - s1 * c2 * s3;
            m[2] = c1 * c2 * s3 + s1 * s2 * c3;
            m[3] = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        }
        case EulerOrder::YXZ: {
            m[0] = s1 * c2 * c3 + c1 * s2 * s3;
            m[1] = c1 * s2 * c3 - s1 * c2 * s3;
            m[2] = c1 * c2 * s3 - s1 * s2 * c3;
            m[3] = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        }
        case EulerOrder::ZXY: {
            m[0] = s1 * c2 * c3 - c1 * s2 * s3;
            m[1] = c1 * s2 * c3 + s1 * c2 * s3;
            m[2] = c1 * c2 * s3 + s1 * s2 * c3;
            m[3] = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        }
        case EulerOrder::ZYX: {
            m[0] = s1 * c2 * c3 - c1 * s2 * s3;
            m[1] = c1 * s2 * c3 + s1 * c2 * s3;
            m[2] = c1 * c2 * s3 - s1 * s2 * c3;
            m[3] = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        }
        case EulerOrder::YZX: {
            m[0] = s1 * c2 * c3 + c1 * s2 * s3;
            m[1] = c1 * s2 * c3 + s1 * c2 * s3;
            m[2] = c1 * c2 * s3 - s1 * s2 * c3;
            m[3] = c1 * c2 * c3 - s1 * s2 * s3;
            break;
        }
        case EulerOrder::XZY: {
            m[0] = s1 * c2 * c3 - c1 * s2 * s3;
            m[1] = c1 * s2 * c3 - s1 * c2 * s3;
            m[2] = c1 * c2 * s3 + s1 * s2 * c3;
            m[3] = c1 * c2 * c3 + s1 * s2 * s3;
            break;
        }
        default:
            break;
    }
}

}
