//
//  m_transform.hpp
//  PlayGround
//
//  Created by Zongming Liu on 2022/10/9.
//

#ifndef m_transform_hpp
#define m_transform_hpp

#include <stdio.h>
#include "math/vec3.hpp"
#include "math/quaternion.hpp"
#include "math/mat4.hpp"
#include "math/transform.h"

namespace mn {

class MTransfrom {
    
public:
    
    MTransfrom();
    
    ~MTransfrom();
    
    void SetTranslation(const Vec3& value);
    
    const Vec3& Translation();
    
    void SetScale(const Vec3& value);
    
    const Vec3& Scale();
    
    void SetRotation(const Quaternion& value);
    
    void SetRotation(const Vec3& value);
    
    const Quaternion& Rotation();
    
    Mat4& Matrix();
    
    void SetMatrix(const Mat4& mat);
    
    void FromMarsTransform(std::shared_ptr<Transform> transform);
    
private:
    
    Vec3 translation_;
    
    Vec3 scale_;
    
    Quaternion rotation_;
    
    Mat4 mat_;
    
};

}

#endif /* m_transform_hpp */
