//
//  mat3.hpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#ifndef mat3_hpp
#define mat3_hpp

#include <stdio.h>

namespace mn {

class Mat3 {
    
public:
    
    float m[9];
    
    Mat3();
    
    Mat3(const Mat3& copy);

    Mat3(float m11, float m21, float m31, float m12, float m22, float m32, float m13, float m23, float m33);
    
    ~Mat3();
    
    size_t Length() const {
        return length_;
    }
    
    static const Mat3 ZERO;
    
    static const Mat3 IDENTITY;
    
private:
    
    void Set(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33);

    size_t length_;
};

}

#endif /* mat3_hpp */
