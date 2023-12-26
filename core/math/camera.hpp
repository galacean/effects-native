//
//  camera.hpp
//  PlayGroundIOS
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef camera_hpp
#define camera_hpp

#include <stdio.h>
#include <string>

#include "math/mat4.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "util/constant.hpp"

namespace mn {

struct CameraParams {
    float near = 0.1f;
    float far = 20;
    float fov = 60;
    float aspect = 1;
    Vec3 position;
    std::shared_ptr<Vec3> rotation;
    int clip_mode = CAMERA_CLIP_MODE_NORMAL;
    std::shared_ptr<Vec4> quat;
};

class Camera {
    
public:
    
    Camera(const char* name = "camera");
    
    ~Camera();
    
    const Mat4& View();
    
    const Mat4& Projection();
    
    const Mat4& ViewProjection();
    
    const Mat4& InverseView();

    void UpdateMatrix();
    
    void SetParams(std::shared_ptr<CameraParams> params);
    
    float GetAspect() const;
    
    float GetPositionZ() const;
    
    float GetFOV() const;

private:
    void GetQuat(Vec4& out);

private:
    
    std::string name_;
    
    Mat4 view_;
    
    Mat4 projection_;
    
    Mat4 view_projection_;
    
    Mat4 inv_view_;
    
    bool dirty_ = false;
    
    CameraParams params_;
    
};

}

#endif /* camera_hpp */
