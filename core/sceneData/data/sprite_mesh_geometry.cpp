//
//  sprite_mesh_geometry.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/18.
//

#include "sprite_mesh_geometry.hpp"
#include "plugin/particle/shape/geometry_2d.hpp"

namespace mn {

void SpriteMeshGeometry::Copy(SpriteMeshGeometry* other) {
    for (int i = 0; i < other->a_point_.size(); i++) {
        a_point_.push_back(other->a_point_[i]);
    }
    for (int i = 0; i < other->index_.size(); i++) {
        index_.push_back(other->index_[i]);
    }
}

SpriteMeshGeometry* SpriteMeshGeometry::GetGeometryByShape(GeometryShapeData* shape_data, const std::vector<float>* const split) {
    // todo: 优化，只创建一个SpriteMeshGeometry
    std::vector<SpriteMeshGeometry*> datas;
    const std::vector<std::shared_ptr<ShapeGeometryData>>& geometries = shape_data->geometries_;
    int index_base = 0;
    int a_point = 0;
    int index = 0;
    
    for (int i = 0; i < geometries.size(); i++) {
        const std::shared_ptr<ShapeGeometryData>& geometry = geometries[i];
        SpriteMeshGeometry* data = Geometry2D::GetGeometry2DTriangles(geometry.get(), index_base, split);
        index_base += data->a_point_.size() / 5;
        datas.push_back(data);
        a_point += data->a_point_.size();
        index += data->index_.size();
    }
    if (datas.size() == 1) {
        return datas[0];
    }
    SpriteMeshGeometry* ret = new SpriteMeshGeometry;
    for (int i = 0; i < datas.size(); i++) {
        SpriteMeshGeometry* data = datas[i];
        for (int j = 0; j < data->a_point_.size(); j++) {
            ret->a_point_.push_back(data->a_point_[j]);
        }
        for (int j = 0; j < data->index_.size(); j++) {
            ret->index_.push_back(data->index_[j]);
        }
        
        delete data;
    }
    return ret;
}

}
