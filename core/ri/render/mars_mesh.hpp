//
//  mars_mesh.hpp
//
//  Created by Zongming Liu on 2022/4/6.
//

#ifndef mars_mesh_hpp
#define mars_mesh_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "ri/render/mars_geometry.hpp"
#include "ri/render/mars_material.hpp"

#include "math/mat4.hpp"

namespace mn {

class MarsRenderer;

class MarsMesh {
    
public:
    
    MarsMesh();
    
    MarsMesh(MarsRenderer *renderer, const std::string& name = "default");
    
    MarsMesh(MarsRenderer *renderer, const std::string& name, int priority, const Mat4& mat);
    
    ~MarsMesh();
    
    void SetWolrdMatrix(const Mat4& mat4);
    
    // todo: 优化下;
    void SetMaterial(const MaterialOptions& material_option);
    
    void SetMaterial(std::shared_ptr<MarsMaterial> material);
    
    void SetGeometry(std::shared_ptr<MarsGeometry> geometry);
    
    void AssignRenderer(MarsRenderer *renderer);
    
    // todo:?
    // void SetGeometrys();
    
    std::shared_ptr<MarsMaterial> GetMaterial();
    
    std::shared_ptr<MarsGeometry> GetGeometry();
    
    Mat4& WorldMatrix() {
        return world_matrix_;
    }
    
    std::vector<std::shared_ptr<MarsGeometry>>& GetGeometries() {
        return geometries_;
    }
    
    void Destroy();
    
    const std::string& Name() {
        return name_;
    }
    
//private:
    
    MarsRenderer *renderer_;
    
    Mat4 world_matrix_;
    
    std::shared_ptr<MarsMaterial> material_;
    
    // tood: ?为什么设计成数组
    std::vector<std::shared_ptr<MarsGeometry>> geometries_;

    std::string name_;
    
    int priority_;
    
    bool hide_;
    
};

}

#endif /* mars_mesh_hpp */
