//
//  sprite_mesh_geometry.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/18.
//

#ifndef sprite_mesh_geometry_hpp
#define sprite_mesh_geometry_hpp

#include "sceneData/mars_data_base.h"

namespace mn {

struct ShapeGeometryData {
    std::vector<std::vector<float>> segments_;
    std::vector<std::vector<float>> points_;
};

class GeometryShapeData {
public:
    GeometryShapeData() {}

    ~GeometryShapeData() {}

    std::vector<std::shared_ptr<ShapeGeometryData>> geometries_;
};

class SpriteMeshGeometry {
public:
    SpriteMeshGeometry() {
    }
    
    ~SpriteMeshGeometry() {}
    
    void Copy(SpriteMeshGeometry* other);
    
    static SpriteMeshGeometry* GetGeometryByShape(GeometryShapeData* shape_data, const std::vector<float>* const split);
    
public:
    std::vector<float> a_point_;
    std::vector<uint16_t> index_;
};

}

#endif /* sprite_mesh_geometry_hpp */
