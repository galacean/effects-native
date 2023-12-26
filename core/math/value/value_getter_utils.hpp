//
//  value_getter_utils.hpp
//
//  Created by Zongming Liu on 2022/1/14.
//

#ifndef value_getter_utils_hpp
#define value_getter_utils_hpp

#include <stdio.h>

namespace mn {

class ValueGetterUtils {
    
public:
    
    static float CurveValueEvaluate(float time, float key_frame0[], float key_frame1[]);

    static float CurveValueIntegrate(float time, float key_frame0[], float key_frame1[]);
    
    static float CurveValueIntegrateByTime(float time, float key_frame0[], float key_frame1[]);
    
    static float LineSegIntegrateByTime(float t, float t0, float t1, float y0, float y1);

    static float LineSegIntegrate(float t, float t0, float t1, float y0, float y1);

};

}

#endif /* value_getter_utils_hpp */
