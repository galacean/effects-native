//
//  geometry_2d.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/18.
//

#include "geometry_2d.hpp"
#include <math.h>
#include "sceneData/mars_data_base.h"
#include "sceneData/data/sprite_mesh_geometry.hpp"
#include "math/vec2.hpp"
#include "math/math_util.hpp"
#include "earcut.hpp"
#include "util/constant.hpp"

namespace mn {

static void GetBezier2DValue(Vec2& out, float t, const std::vector<float>& p0, const std::vector<float>& p1, float cpx0, float cpy0, float cpx1, float cpy1) {
    const float ddt = 1 - t;
    const float a = ddt * ddt * ddt;
    const float b = 3 * t * ddt * ddt;
    const float c = 3 * t * t * ddt;
    const float d = t * t * t;
    out.m[0] = a * p0[0] + b * cpx0 + c * cpx1 + d * p1[0];
    out.m[1] = a * p0[1] + b * cpy0 + c * cpy1 + d * p1[1];
}

SpriteMeshGeometry* Geometry2D::GetGeometry2DTriangles(ShapeGeometryData* geometry, int index_base, const std::vector<float>* const uv_transform) {
    int point_count = 0;
    const std::vector<std::vector<float>>& segments = geometry->segments_;
    for (int i = 0; i < segments.size(); i++) {
        const std::vector<float>& segment = segments[i];
        point_count += segment.size() - 1;
    }
    SpriteMeshGeometry* ret = new SpriteMeshGeometry();
    ret->a_point_.resize(point_count * SPRITE_VERTEX_STRIDE);
    int index = 0;
    float dx = 0, dy = 0, sw = 1, sh = 1, r = 0;
    if (uv_transform) {
        dx = uv_transform->at(0);
        dy = uv_transform->at(1);
        r = uv_transform->at(4);
        sw = (r != 0) ? uv_transform->at(3) : uv_transform->at(2);
        sh = (r != 0) ? uv_transform->at(2) : uv_transform->at(3);
    }
    Vec2 temp;
    const float angle = (r == 0) ? 0 : -M_PI / 2.0f;
    
    const std::vector<std::vector<float>>& points = geometry->points_;
    for (int i = 0; i < segments.size(); i++) {
        const std::vector<float>& segment = segments[i];
        const std::vector<float>& p0 = points[i];
        const std::vector<float>& p1 = (i < segments.size() - 1) ? points[i + 1] : points[0];
        const std::vector<float>& keys = segment;
        Vec2 point;
        for (int j = 0; j < keys.size() - 1; j++) {
            float key = keys[j];
            GetBezier2DValue(point, key, p0, p1, p0[4], p0[5], p1[2], p1[3]);
            // setPoint
            float x = point.m[0];
            float y = point.m[1];
            ret->a_point_[index++] = x / 2;
            ret->a_point_[index++] = y / 2;
            if (uv_transform) {
                temp.Set(x, y);
                MathUtil::RotateVec2(temp, temp, angle);
                ret->a_point_[index++] = dx + (temp.m[0] + 1) / 2 * sw;
                ret->a_point_[index++] = dy + (temp.m[1] + 1) / 2 * sh;
            } else {
                ret->a_point_[index++] = (x + 1) / 2;
                ret->a_point_[index++] = (y + 1) / 2;
            }
            index += 2;
        }
    }
    Earcut::Execute(ret->index_, ret->a_point_, SPRITE_VERTEX_STRIDE, index_base);
    
    return ret;
}

}
