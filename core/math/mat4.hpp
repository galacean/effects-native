//
//  mat4.hpp
//
//  Created by Zongming Liu on 2021/11/23.
//

#ifndef mat4_hpp
#define mat4_hpp

#include <stdio.h>

namespace mn {

class Mat4 {
    
public:
    
    float m[16];
    
    Mat4();
    
    Mat4(float m11, float m21, float m31, float m41, float m12, float m22, float m32, float m42,
         float m13, float m23, float m33, float m43, float m14, float m24, float m34, float m44);
    
    Mat4(const Mat4 &copy);
    
    Mat4(float* data);
    
    ~Mat4();
    
    size_t Length() const {
        return length_;
    }
    
    Mat4 Clone() const;
    
    void Inverse();
    
    void Transpose();
    
    // 将uint8 数组转化为mat；
    void Unpack(void* data);
    
    void Set(const Mat4 &copy);
    
    static const Mat4 ZERO;
    
    static const Mat4 IDENTITY;
    
private:
    
    size_t length_;
    
    void Set(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
             float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
    
};

}

#endif /* mat4_hpp */
