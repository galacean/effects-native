//
//  vec3.hpp
//
//  Created by Zongming Liu on 2021/10/25.
//

#ifndef vec3_hpp
#define vec3_hpp

#include <stdio.h>
#include <stdlib.h>

namespace mn {

class Vec3 {
    
public:
    float m[3];
    
    Vec3();
    
    Vec3(const Vec3& vec3);
    
    Vec3(float x, float y, float z);
        
    ~Vec3();
    
    void Set(const Vec3& vec3);
    
    void Set(float x, float y, float z);
    
    const float* Data() const {
        return m;
    };
    
    size_t Length() const {
        return length_;
    }
    
    void Add(const Vec3& vec);
    
    void Multi(const Vec3& vec);
    
    void Copy(float *ret);
    
    void Divide(float v);

    
private:
        
    size_t length_;
    
};

}

#endif /* vec3_hpp */
