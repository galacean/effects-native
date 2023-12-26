//
//  vec2.hpp
//
//  Created by Zongming Liu on 2022/1/25.
//

#ifndef vec2_hpp
#define vec2_hpp

#include <stdio.h>

namespace mn {

class Vec2 {
  
public:
    
    float m[2];
    
    Vec2();
    
    Vec2(const Vec2& vec);
    
    Vec2(float x, float y);
    
    ~Vec2();

    void Set(const Vec2& vec2);
    
    void Set(float x, float y);
    
    float* Data() {
        return m;
    };
    
    size_t Length() const {
        return length_;
    }
    
    void Add(const Vec2& vec);
    
    void Multi(const Vec2& vec);
    
    void Divide(float v);
    
private:
        
    size_t length_;

};

}

#endif /* vec2_hpp */
