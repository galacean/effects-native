//
//  vec4.hpp
//
//  Created by Zongming Liu on 2021/11/24.
//

#ifndef vec4_hpp
#define vec4_hpp

#include <stdio.h>
#include <stdlib.h>

namespace mn {

// todo: 优化拷贝构造函数
class Vec4 {
      
public:
    
    float m[4];
    
    Vec4();
    
    Vec4(const Vec4& vec);
    
    Vec4(float x, float y, float z, float w);
    
    void Set(const Vec4& vec4);
    
    void Set(float x, float y, float z, float w);
    
    const float* Data() const {
        return m;
    };
    
    size_t Length() const {
        return length_;
    }
    
    void Add(const Vec4& vec);
    
    void Multi(const Vec4& vec);
    
    void Copy(float *ret);
    
    void Divide(float v);


    ~Vec4();

private:
        
    size_t length_;
    
};

}

#endif /* vec4_hpp */
