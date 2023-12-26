//
//  m_camera.hpp
//
//  Created by Zongming Liu on 2022/10/12.
//

#ifndef m_camera_hpp
#define m_camera_hpp

#include <stdio.h>
#include <vector>
#include "model/renderer/m_entity.hpp"
#include "model/model_vfx_item.hpp"
#include "math/math_util.hpp"

namespace mn {

class MMCamera : public MEntity {
    
public:
    
    MMCamera();
    
    ~MMCamera();
    
    MEntityType GetType() override {
        return MEntityType::Camera;
    }
    
    void Create(std::shared_ptr<MarsItemCamera> camera_option, int width, int height, std::shared_ptr<ModelVFXItem> owner_item);
    
    void Update();

    float near_;
    
    float far_;
    
    float fovy_;
    
    float aspect_;
    
    bool reversed_;
    
    std::weak_ptr<ModelVFXItem> owner_item_;
    
    const Mat4& GetProjectionMatrix() {
        return projection_matrix_;
    }
    
    void GetNewProjectionMatrix(Mat4& out);
    
    const Mat4& GetViewMatrix() {
        
        return view_matrix_;
    }
    
    void SetEye(const Vec3& pos);
    
    const Vec3& GetEye();
        
private:
    
    Mat4 projection_matrix_;
    
    Mat4 view_matrix_;
        
    std::string name_;
    
};

class MCameraManager {
    
public:
    
    MCameraManager();
    
    ~MCameraManager();
    
    void Initial(int width, int height);
    
    std::shared_ptr<MMCamera> Insert(std::shared_ptr<MarsItemCamera> camera, std::shared_ptr<ModelVFXItem> owner_item);
    
    void Remove(std::shared_ptr<MMCamera> camera);
    
    void UpdateDefaultCamera(std::shared_ptr<CameraParams> camera_params);
    
    size_t CameraCount() {
        return camera_list_.size();
    }
    
    std::shared_ptr<MMCamera> DefaultCamera() {
        return default_camera_;
    }
    
private:
    
    std::vector<std::shared_ptr<MMCamera>> camera_list_;
    
    std::shared_ptr<MMCamera> default_camera_;
        
    int width_;
    
    int height_;

};
}

#endif /* m_camera_hpp */
