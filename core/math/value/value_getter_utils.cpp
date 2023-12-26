//
//  value_getter_utils.cpp
//
//  Created by Zongming Liu on 2022/1/14.
//

#include "value_getter_utils.hpp"
#include "math/value/value_getter.hpp"

namespace mn {

float ValueGetterUtils::CurveValueEvaluate(float time, float key_frame0[], float key_frame1[]) {
    float dt = key_frame1[ CURVE_PRO_TIME ] - key_frame0[ CURVE_PRO_TIME ];

    float m0 = key_frame0[ CURVE_PRO_OUT_TANGENT ] * dt;
    float m1 = key_frame1[ CURVE_PRO_IN_TANGENT ] * dt;

    float t = (time - key_frame0[ CURVE_PRO_TIME ]) / dt;
    float t2 = t * t;
    float t3 = t2 * t;

    float a = 2 * t3 - 3 * t2 + 1;
    float b = t3 - 2 * t2 + t;
    float c = t3 - t2;
    float d = -2 * t3 + 3 * t2;
    return a * key_frame0[ CURVE_PRO_VALUE ] + b * m0 + c * m1 + d * key_frame1[ CURVE_PRO_VALUE ];
}

float ValueGetterUtils::CurveValueIntegrate(float time, float key_frame0[], float key_frame1[]) {
    float k = key_frame1[ CURVE_PRO_TIME ] - key_frame0[ CURVE_PRO_TIME ];
    float m0 = key_frame0[ CURVE_PRO_OUT_TANGENT ] * k;
    float m1 = key_frame1[ CURVE_PRO_IN_TANGENT ] * k;
    float t0 = key_frame0[ CURVE_PRO_TIME ];
    float v0 = key_frame0[ CURVE_PRO_VALUE ];
    float v1 = key_frame1[ CURVE_PRO_VALUE ];

    float dt = t0 - time;
    float dt2 = dt * dt;
    float dt3 = dt2 * dt;
    return (m0 + m1 + 2 * v0 - 2 * v1) * dt3 * dt / (4 * k * k * k) +
        (2 * m0 + m1 + 3 * v0 - 3 * v1) * dt3 / (3 * k * k) +
        m0 * dt2 / 2 / k - v0 * dt;
}

float ValueGetterUtils::CurveValueIntegrateByTime(float time, float key_frame0[], float key_frame1[]) {
    float k = key_frame1[ CURVE_PRO_TIME ] - key_frame0[ CURVE_PRO_TIME ];
    float m0 = key_frame0[ CURVE_PRO_OUT_TANGENT ] * k;
    float m1 = key_frame1[ CURVE_PRO_IN_TANGENT ] * k;
    float t0 = key_frame0[ CURVE_PRO_TIME ];
    float v0 = key_frame0[ CURVE_PRO_VALUE ];
    float v1 = key_frame1[ CURVE_PRO_VALUE ];
    
    float dt = t0 - time;
    float dt2 = dt * dt;
    float dt3 = dt2 * dt;
    float k2 = k * k;
    float k3 = k2 * k;
    
    float ret = -30 * k3 * v0 * (t0 + time) * dt +
        10 * k2 * m0 * (t0 + 2 * time) * dt2 +
        5 * k * (t0 + 3 * time) * (2 * m0 + m1 + 3 * v0 - 3 * v1) * dt3 +
        3 * (t0 + 4 * time) * (m0 + m1 + 2 * v0 - 2 * v1) * dt3 * dt;
    
    return ret / 60 / k3;
}

float ValueGetterUtils::LineSegIntegrateByTime(float t, float t0, float t1, float y0, float y1) {
    float t2 = t * t;
    float t3 = t2 * t;
    float t02 = t0 * t0;
    float t03 = t02 * t0;
    return (2 * t3 * (y0 - y1) + 3 * t2 * (t0 * y1 - t1 * y0) - t03 * (2 * y0 + y1) + 3 * t02 * t1 * y0) / (6 * (t0 - t1));
}

float ValueGetterUtils::LineSegIntegrate(float t, float t0, float t1, float y0, float y1) {
    float h = t - t0;
    return (y0 + y0 + (y1 - y0) * h / (t1 - t0)) * h / 2;
}


}
