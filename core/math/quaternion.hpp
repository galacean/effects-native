//
//  quaternion.hpp
//  PlayGround
//
//  Created by Zongming Liu on 2022/10/9.
//

#ifndef quaternion_hpp
#define quaternion_hpp

#include <stdio.h>
#include "math/vec4.hpp"

namespace mn {

enum EulerOrder {
  XYZ = 0,
  XZY = 1,
  YXZ = 2,
  YZX = 3,
  ZXY = 4,
  ZYX = 5,
};

class Euler {
  
public:
    
    Euler(float x, float y, float z, EulerOrder order = EulerOrder::ZYX) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        order_ = order;
    }
    
    float GetX() const {
        return data_[0];
    }
    
    float GetY() const {
        return data_[1];
    }
    
    float GetZ() const {
        return data_[2];
    }
    
    EulerOrder GetOrder() const {
        return order_;
    }
    
private:
    
    float data_[3];
        
    EulerOrder order_;
    
};

class Quaternion {
  
public:
        
    float m[4];

    Quaternion();
    
    Quaternion(float x, float y, float z, float w);
    
    float GetX() const {
        return m[0];
    }
    
    float GetY() const {
        return m[1];
    }
    
    float GetZ() const {
        return m[2];
    }
    
    float GetW() const {
        return m[3];
    }
    
    void SetQuaternion(const Vec4& quat) {
        m[0] = quat.m[0];
        m[1] = quat.m[1];
        m[2] = quat.m[2];
        m[3] = quat.m[3];
    }
    
    void SetFromEuler(const Euler& euler);
    
private:
    
};

}

#endif /* quaternion_hpp */
