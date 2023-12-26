//
//  geometry_2d.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/18.
//

#ifndef geometry_2d_hpp
#define geometry_2d_hpp

#include <vector>

namespace mn {

struct ShapeGeometryData;
class SpriteMeshGeometry;

class Geometry2D {
public:
    static SpriteMeshGeometry* GetGeometry2DTriangles(ShapeGeometryData* geometry, int index_base, const std::vector<float>* const uv_transform);
};

}

#endif /* geometry_2d_hpp */
